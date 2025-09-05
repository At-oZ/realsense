#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <thread>
#include <mutex>
#include <filesystem>

// OpenCV Headers
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaarithm.hpp>

// シミュレーションパラメータを格納する構造体
struct SimParams {
    double panel_size = 180.0;
    double z0 = 1000.0;
    double final_angle_deg = 30.0;
    int final_resolution_W = 60;
    int final_resolution_H = 60;
    double std_angle_deg = 60.0;
    int std_resolution = 300;
};

// スレッドに渡すタスクの情報を格納する構造体
struct Task {
    double xo, yo;
    int i, j;
};

// スレッドからの結果を格納する構造体
struct Result {
    int i, j;
    cv::cuda::GpuMat image;
};

// 基準となる広角画像を生成し、GPUメモリにアップロードする関数
std::pair<cv::cuda::GpuMat, double> createStandardImageGpu(const cv::Mat& input_image, const SimParams& params) {
    // スケール計算
    double std_angle_rad = params.std_angle_deg * CV_PI / 180.0;
    double std_world_view_width = 2.0 * params.z0 * std::tan(std_angle_rad / 2.0);
    double px_per_mm_std = static_cast<double>(params.std_resolution) / std_world_view_width;

    // パネルのリサイズ
    int panel_size_px_std = static_cast<int>(params.panel_size * px_per_mm_std);
    cv::Mat resized_panel;
    cv::resize(input_image, resized_panel, cv::Size(panel_size_px_std, panel_size_px_std), 0, 0, cv::INTER_AREA);

    // 基準画像の作成 (CPU)
    cv::Mat standard_image_cpu = cv::Mat::zeros(params.std_resolution, params.std_resolution, CV_8UC3);
    int x_offset = (params.std_resolution - panel_size_px_std) / 2;
    int y_offset = (params.std_resolution - panel_size_px_std) / 2;
    resized_panel.copyTo(standard_image_cpu(cv::Rect(x_offset, y_offset, panel_size_px_std, panel_size_px_std)));

    // GPUにアップロード
    cv::cuda::GpuMat standard_image_gpu;
    standard_image_gpu.upload(standard_image_cpu);

    return { standard_image_gpu, px_per_mm_std };
}

// 1つの視点に対する画像をGPU上で高速に生成する関数
cv::cuda::GpuMat processSingleViewGpu(const cv::cuda::GpuMat& standard_image_gpu, double px_per_mm_std, double xo, double yo, const SimParams& params) {
    // 切り出しサイズの計算
    double final_angle_rad = params.final_angle_deg * CV_PI / 180.0;
    double final_world_view_width = 2.0 * params.z0 * std::tan(final_angle_rad / 2.0);
    int crop_size_px = static_cast<int>(final_world_view_width * px_per_mm_std);

    // 視点移動をピクセルシフト量に変換
    double x_shift_px = -xo * px_per_mm_std;
    double y_shift_px = yo * px_per_mm_std;

    // --- GPUによる高速処理 ---
    // 1. 切り出し元の座標を計算
    int src_x1 = static_cast<int>(std::round((params.std_resolution - crop_size_px) / 2.0 - x_shift_px));
    int src_y1 = static_cast<int>(std::round((params.std_resolution - crop_size_px) / 2.0 - y_shift_px));

    // 2. 領域外を白で埋めるため、まず白地の画像をGPU上に用意
    cv::cuda::GpuMat cropped_region(crop_size_px, crop_size_px, CV_8UC3, cv::Scalar(255, 255, 255));

    // 3. 基準画像と切り出し領域が重なる部分を計算
    int clipped_src_x1 = std::max(src_x1, 0);
    int clipped_src_y1 = std::max(src_y1, 0);
    int clipped_src_x2 = std::min(src_x1 + crop_size_px, params.std_resolution);
    int clipped_src_y2 = std::min(src_y1 + crop_size_px, params.std_resolution);

    int copy_width = clipped_src_x2 - clipped_src_x1;
    int copy_height = clipped_src_y2 - clipped_src_y1;

    // 4. 重なりがある場合のみ、GPU上でピクセルをコピー
    if (copy_width > 0 && copy_height > 0) {
        int dst_x1 = clipped_src_x1 - src_x1;
        int dst_y1 = clipped_src_y1 - src_y1;

        cv::Rect src_roi(clipped_src_x1, clipped_src_y1, copy_width, copy_height);
        cv::Rect dst_roi(dst_x1, dst_y1, copy_width, copy_height);

        standard_image_gpu(src_roi).copyTo(cropped_region(dst_roi));
    }

    // 5. GPU上でリサイズ
    cv::cuda::GpuMat resized_gpu;
    cv::cuda::resize(cropped_region, resized_gpu, cv::Size(params.final_resolution_W, params.final_resolution_H), 0, 0, cv::INTER_AREA);

    return resized_gpu;
}

// GPU上の画像を結合して一枚の画像として保存する関数
void combineImagesGpu(const std::vector<Result>& results, int grid_count, const SimParams& params) {
    int img_h = params.final_resolution_H;
    int img_w = params.final_resolution_W;
    int combined_h = grid_count * img_h;
    int combined_w = grid_count * img_w;

    // 結合用の大きな画像をGPU上に確保
    cv::cuda::GpuMat combined_image_gpu(combined_h, combined_w, CV_8UC3, cv::Scalar(0, 0, 0));

    std::cout << "\nGPU上での結合処理を開始します..." << std::endl;
    for (const auto& result : results) {
        int i = result.i;
        int j = result.j;
        // y方向の配置を上下反転
        int y_pos = (grid_count - 1 - i) * img_h;
        int x_pos = j * img_w;
        result.image.copyTo(combined_image_gpu(cv::Rect(x_pos, y_pos, img_w, img_h)));
    }

    // 最終的な画像をCPUにダウンロード
    cv::Mat combined_image_cpu;
    combined_image_gpu.download(combined_image_cpu);

    std::string output_filename = "combined_view.png";
    cv::imwrite(output_filename, combined_image_cpu);
    std::cout << "結合画像を '" << output_filename << "' として保存しました。" << std::endl;
}

// 各スレッドで実行されるワーカー関数
void worker(
    const std::vector<Task>* tasks,
    std::vector<Result>* results,
    std::mutex* mtx,
    int start_idx,
    int end_idx,
    const cv::cuda::GpuMat* standard_image_gpu,
    double px_per_mm_std,
    const SimParams* params
) {
    std::vector<Result> local_results;
    for (int i = start_idx; i < end_idx; ++i) {
        const auto& task = (*tasks)[i];
        cv::cuda::GpuMat captured_image_gpu = processSingleViewGpu(
            *standard_image_gpu, px_per_mm_std, task.xo, task.yo, *params
        );
        local_results.push_back({ task.i, task.j, captured_image_gpu });
    }

    // 計算が終わったらロックを取得して全体の結果にマージ
    std::lock_guard<std::mutex> lock(*mtx);
    results->insert(results->end(), local_results.begin(), local_results.end());
}


int main() {
    // --- 実行設定 ---
    const std::string input_image_path = "./images/standard/grid_image.png";
    const int num_threads = 12;
    const int grid_count = 40;
    const double grid_space = 5.0;
    const SimParams params;

    // --- 準備 ---
    cv::Mat input_image = cv::imread(input_image_path);
    if (input_image.empty()) {
        std::cout << "'" << input_image_path << "' が見つかりません。ダミー画像を生成します。" << std::endl;
        input_image = cv::Mat::zeros(200, 200, CV_8UC3);
        int cell_size = 25;
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                if ((r + c) % 2 == 0) {
                    cv::rectangle(input_image, cv::Rect(c * cell_size, r * cell_size, cell_size, cell_size), cv::Scalar(255, 255, 255), -1);
                }
            }
        }
        cv::imwrite(input_image_path, input_image);
    }

    // --- 1. 基準画像を生成し、GPUに転送 ---
    std::cout << "基準となる広角画像を生成し、GPUに転送中..." << std::endl;
    auto [standard_image_gpu, px_per_mm_std] = createStandardImageGpu(input_image, params);
    std::cout << "GPUへの転送完了。" << std::endl;

    // --- 2. 全タスクのリストを作成 ---
    std::vector<Task> tasks;
    tasks.reserve(grid_count * grid_count);
    for (int i = 0; i < grid_count; ++i) {
        for (int j = 0; j < grid_count; ++j) {
            double yo = (static_cast<double>(i) - (grid_count - 1.0) / 2.0) * grid_space;
            double xo = (static_cast<double>(j) - (grid_count - 1.0) / 2.0) * grid_space;
            tasks.push_back({ xo, yo, i, j });
        }
    }

    // --- 3. 並列処理の実行と結合 ---
    std::cout << "----------------------------------------" << std::endl;
    std::cout << num_threads << "スレッドによる並列処理を開始します: " << tasks.size() << " 枚の画像を生成します。" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    std::vector<Result> results;
    std::mutex mtx;
    results.reserve(tasks.size());

    int tasks_per_thread = tasks.size() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        int start_idx = i * tasks_per_thread;
        int end_idx = (i == num_threads - 1) ? tasks.size() : start_idx + tasks_per_thread;
        threads.emplace_back(worker, &tasks, &results, &mtx, start_idx, end_idx, &standard_image_gpu, px_per_mm_std, &params);
    }

    for (auto& th : threads) {
        th.join();
    }

    // 全画像の結合
    combineImagesGpu(results, grid_count, params);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> process_time = end_time - start_time;

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "全処理が完了しました。" << std::endl;
    std::cout << "総生成・結合画像数: " << results.size() << " 枚" << std::endl;
    std::cout << "総処理時間 (process_time): " << std::fixed << std::setprecision(2) << process_time.count() << " 秒" << std::endl;
    if (!results.empty()) {
        std::cout << "1画像あたりの平均処理時間: " << std::fixed << std::setprecision(2) << process_time.count() * 1000.0 / results.size() << " ミリ秒" << std::endl;
    }

    return 0;
}

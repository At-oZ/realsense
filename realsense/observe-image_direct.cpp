/* ピンホールを介して要素画像群を観察するシミュレーション */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <Windows.h>

#define NOMINMAX

using namespace cv;
using namespace std;

cv::Mat direct_observe(cv::Mat input_image, double observer_x, double observer_y, double observer_z, double pixel_size, int display_image_px, double tile_size_mm, int image_size_px, double tile_pos);

int main(int argc, char* argv[]) {

    ostringstream stream;

    // ピクセルサイズの計算
    const double coef = 1.0; // 倍率
    const double display_px_size = 13.4 * 25.4 / std::sqrt(3840 * 3840 + 2400 * 2400) / coef; // ディスプレイのピクセルサイズ

    // 観察者と表示系の設定
    const int nph = 40;
    const int element_image_px = 60;
    const double display_area_size = element_image_px * display_px_size * nph;
    const double pinhole_pitch = element_image_px * display_px_size;    // ピンホール間の間隔（mm）
    const double pinhole_array_size = pinhole_pitch * nph;
    const double observer_z = 1000.0;
    const double focal_length = observer_z / (3 * nph - 1); // レンズアレイの焦点距離
    const double observer_x = 0;
    const double observer_y = 0.0;

    std::cout << "observe x:" << observer_x << ", observe y:" << observer_y << std::endl;
    
    // 画像を読み込む
    std::string filename_tile = "./images/standard/mandrill.bmp";
    cv::Mat tile_image = cv::imread(filename_tile);
    const int tile_px = 256; // タイルの解像度
    const double tile_pos = 300.0; // タイルの位置
    const double tile_size = display_area_size * (tile_pos + observer_z) / observer_z; // 拡大の場合 * (subz + observer_z_min) / observer_z_minを付ける
    const double tile_px_size = tile_size / tile_px; // タイルに貼る画像のピクセルサイズ（mm）

    if (tile_image.empty())
    {
        std::cout << "画像を開くことができませんでした。\n";
        return -1;
    }

    // 観察画像の生成
    cv::Mat observe_image = direct_observe(tile_image, observer_x, observer_y, -observer_z, pinhole_pitch, nph, tile_size, tile_px, tile_pos); // 解像度を2400 x 2400にする場合

    // 観察画像の保存
    stream.str("");
    stream << "D:/EvacuatedStorage/observe-images/direct/observe-image_direct-mandrill_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << tile_size << "_zi" << (int)tile_pos << "_xo" << std::fixed << std::setprecision(2) << observer_x << "_yo" << std::fixed << std::setprecision(2) << observer_y << "_zo" << std::fixed << std::setprecision(2) << observer_z << ".png";
    std::string filenameout = stream.str();
    //std::string filenameout = "D:/EvacuatedStorage/observe-images/IE-prop-normal-v1/observe-image_IE-prop-normal-v1-grid_f7.9667_subsize90.20_zi300.png";
    cv::Mat resized_obsv_img;
    cv::resize(observe_image, resized_obsv_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
    cv::imwrite(filenameout, resized_obsv_img);

    MessageBeep(-1);
    return 0;
}

cv::Mat direct_observe(cv::Mat input_image, double observer_x, double observer_y, double observer_z, double pixel_size, int display_image_px, double tile_size_mm, int image_size_px, double tile_pos) {

    // ディスプレイの設定
    const double display_area_size = display_image_px * pixel_size;

    // タイルの設定
    const double pixel_size_mm = tile_size_mm / image_size_px; // タイルに貼る画像のピクセルサイズ（mm）

    // ピンホールの設定
    const double pinhole_spacing = display_area_size / display_image_px;    // ピンホール間の間隔（mm）
    const double pinhole_size = pinhole_spacing;        // ピンホールの一辺の長さ（mm）(lennaの画像を直接みるときはpinhole_spacingと同じ, 表示系の再現の場合は指定する)

    // 観察者とタイルの位置
    const int image_resolution = static_cast<int>(floor(display_image_px / display_image_px)); // 画像の解像度（ピクセル）

    // ピンホールの位置を計算
    std::vector<cv::Point2d> pinhole_positions;
    double offset = -((display_image_px - 1) * pinhole_spacing) / 2.0;

    for (int i = 0; i < display_image_px; ++i)
    {
        for (int j = 0; j < display_image_px; ++j)
        {
            double x = (offset + j * pinhole_spacing);
            double y = (offset + i * pinhole_spacing);
            pinhole_positions.emplace_back(x, y);
        }
    }

    // 出力画像を作成
    cv::Mat output_image(display_image_px, display_image_px, CV_8UC3);

    // 各ピンホールについて観察される色を計算
    int index = 0;
    for (int i = 0; i < display_image_px; ++i)
    {
        for (int j = 0; j < display_image_px; ++j)
        {
            const auto& pinhole_pos = pinhole_positions[index++];

            // ピンホールを通して見えるタイル上の領域を計算
            // 視線を通してピンホールからタイルへの射影を計算
            double scale = (tile_pos - observer_z) / (0.0 - observer_z); // z=observe_zからz=tile_posへの拡大率
            double view_x = (pinhole_pos.x - observer_x) * scale + observer_x;
            double view_y = (pinhole_pos.y - observer_y) * scale + observer_y;

            // タイル上での可視領域（ピンホールの開口を考慮）
            double pinhole_half = pinhole_size / 2.0;
            double view_pinhole_half = pinhole_half * scale;

            double visible_area_min_x = view_x - view_pinhole_half;
            double visible_area_max_x = view_x + view_pinhole_half;
            double visible_area_min_y = view_y - view_pinhole_half;
            double visible_area_max_y = view_y + view_pinhole_half;

            // タイルの範囲内に制限
            visible_area_min_x = (std::max)(-tile_size_mm / 2.0, visible_area_min_x);
            visible_area_max_x = (std::min)(tile_size_mm / 2.0, visible_area_max_x);
            visible_area_min_y = (std::max)(-tile_size_mm / 2.0, visible_area_min_y);
            visible_area_max_y = (std::min)(tile_size_mm / 2.0, visible_area_max_y);

            // 可視領域がタイル内に存在しない場合
            if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
            {
                // 黒色を設定
                output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                continue;
            }

            // ピクセルインデックスの範囲を計算
            int x_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_x + tile_size_mm / 2) / pixel_size_mm)));
            int x_max_idx = (std::min)(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_x + tile_size_mm / 2) / pixel_size_mm)));
            int y_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_y + tile_size_mm / 2) / pixel_size_mm)));
            int y_max_idx = (std::min)(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_y + tile_size_mm / 2) / pixel_size_mm)));

            //std::cout << "x min idx:" << x_min_idx << ", x max idx:" << x_max_idx << ", y min idx:" << y_min_idx << ", y max idx:" << y_max_idx << std::endl;

            // 観察される色の加重平均を計算
            cv::Vec3d weighted_color(0, 0, 0);
            double total_area = 0.0;

            for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
            {
                // ピクセルのx座標範囲を計算
                double x_pixel_min = x_idx * pixel_size_mm - tile_size_mm / 2;
                double x_pixel_max = (x_idx + 1) * pixel_size_mm - tile_size_mm / 2;

                double x_overlap = (std::max)(0.0, (std::min)(visible_area_max_x, x_pixel_max) - (std::max)(visible_area_min_x, x_pixel_min));

                if (x_overlap <= 0) continue;

                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
                {
                    // ピクセルのy座標範囲を計算
                    double y_pixel_min = y_idx * pixel_size_mm - tile_size_mm / 2;
                    double y_pixel_max = (y_idx + 1) * pixel_size_mm - tile_size_mm / 2;

                    double y_overlap = (std::max)(0.0, (std::min)(visible_area_max_y, y_pixel_max) - (std::max)(visible_area_min_y, y_pixel_min));

                    if (y_overlap <= 0) continue;

                    double overlap_area = x_overlap * y_overlap;
                    total_area += overlap_area;

                    // ピクセル値を取得し、加重平均に加算
                    cv::Vec3b pixel = input_image.at<cv::Vec3b>(y_idx, x_idx);
                    weighted_color += cv::Vec3d(pixel) * overlap_area;
                }
            }

            cv::Vec3b final_color;
            if (total_area > 0)
            {
                weighted_color /= total_area;
                final_color = cv::Vec3b(
                    static_cast<uchar>(weighted_color[0]),
                    static_cast<uchar>(weighted_color[1]),
                    static_cast<uchar>(weighted_color[2])
                );
            }
            else
            {
                // 観察可能な領域がない場合は黒色
                final_color = cv::Vec3b(0, 0, 0);
            }

            // 出力画像に色を設定
            output_image.at<cv::Vec3b>(i, j) = final_color;
        }
    }

    return output_image;
}

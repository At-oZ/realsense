//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//
//int main()
//{
//
//    for (int nph = 200; nph >= 50; nph /= 2) {
//
//
//        for (int nzl = 128; nzl >= 64; nzl /= 2) {
//
//
//            for (int subz = 1024; subz >= 256; subz /= 2) {
//
//
//                for (int pt = 1; pt <= 5; pt++) {
//
//                    // ピクセルサイズの計算
//                    const double pixel_size = 13.4 / std::sqrt(3840 * 3840 + 2400 * 2400) * 25.4;
//                    std::cout << "camera pixel size:" << pixel_size << std::endl;
//
//                    // ディスプレイの設定
//                    const int display_image_px = 2400;
//                    const double display_area_size = display_image_px * pixel_size;
//                    cv::Mat display_image = cv::Mat::zeros(cv::Size(display_image_px, display_image_px), CV_8UC3);
//
//                    // タイルの設定
//                    const double tile_size_mm = display_area_size;  // タイルのサイズ（mm）
//                    const int image_size_px = display_image_px;      // タイルに貼る画像の解像度（lennaの画像を直接みるときは256px, 表示系の再現の場合はdisplay_image_pxと同じ）
//                    const double pixel_size_mm = tile_size_mm / image_size_px; // タイルに貼る画像のピクセルサイズ（mm）
//                    std::cout << "pixel size:" << pixel_size_mm << std::endl;
//
//                    // ピンホールの設定
//                    const int num_pinhole_per_axis = nph;     // 各軸のピンホール数
//                    const double pinhole_spacing = display_area_size / num_pinhole_per_axis;    // ピンホール間の間隔（mm）
//                    const double pinhole_size = 0.2;        // ピンホールの一辺の長さ（mm）
//
//                    // 観察者とタイルの位置
//                    const double observer_z = -1450.0; // mm
//                    const double tile_pos = 4.0; // mm
//                    const int image_resolution = static_cast<int>(floor(display_image_px / num_pinhole_per_axis)); // 画像の解像度（ピクセル）
//                    const double subject_z = subz;
//
//                    // 点群の設定
//                    const int num_z_level = nzl;
//                    const double Ddash = 250.0;
//                    const double ptimes = pt;
//
//                    // lennaの画像を直接見るとき用
//                    //double tile_pos = subject_z;
//
//                    // 画像を読み込む
//                    //std::string filenamein = "./images/lenna/camera-array-sim/weightedAveraging_captured_image_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                    //std::string filenamein = "./images/lenna.bmp";
//                    //std::string filenamein = "./images/lenna/prop-reconstruction/v2-2/prop-lenna-v2-2_ImgDisplay_NumZLevel" + std::to_string(num_z_level) + "_Ddash" + std::to_string(static_cast<int>(Ddash)) + "_pitchTimes" + std::to_string(static_cast<int>(ptimes)) + "_subjectZ" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                     std::string filenamein = "./images/lenna/prop-reconstruction/v2-2/comparison/prop-lenna-v2-2_ImgDisplay_NumPinhole" + std::to_string(num_pinhole_per_axis) + "NumZLevel" + std::to_string(num_z_level) + "_pitchTimes" + std::to_string((int)ptimes) + "_subjectZ" + std::to_string((int)subject_z) + ".png";
//                    //std::string filenamein = "./images/lenna/prop-reconstruction/DirectProjection/prop-lenna-DP_ImgDisplay_gridSize" + std::to_string(num_pinhole_per_axis) + "_subjectZ" + std::to_string((int)subject_z) + ".png";
//                    cv::Mat image = cv::imread(filenamein);
//
//                    if (image.empty())
//                    {
//                        std::cout << "画像を開くことができませんでした。\n";
//                        return -1;
//                    }
//
//
//                    // ピンホールの位置を計算（x, y軸の正負を反転）
//                    std::vector<cv::Point2d> pinhole_positions;
//                    double offset = -((num_pinhole_per_axis - 1) * pinhole_spacing) / 2.0;
//
//                    for (int i = 0; i < num_pinhole_per_axis; ++i)
//                    {
//                        for (int j = 0; j < num_pinhole_per_axis; ++j)
//                        {
//                            double x = (offset + j * pinhole_spacing);  // x軸はそのまま
//                            double y = (offset + i * pinhole_spacing);  // y軸の反転
//                            pinhole_positions.emplace_back(x, y);
//                        }
//                    }
//
//                    // 出力画像を作成（4x4ピクセル）
//                    cv::Mat output_image(num_pinhole_per_axis, num_pinhole_per_axis, CV_8UC3);
//
//                    // 各ピンホールについて観察される色を計算
//                    int index = 0;
//                    for (int i = 0; i < num_pinhole_per_axis; ++i)
//                    {
//                        for (int j = 0; j < num_pinhole_per_axis; ++j)
//                        {
//                            const auto& pinhole_pos = pinhole_positions[index++];
//
//                            // ピンホールを通して見えるタイル上の領域を計算
//                            // 視線を通してピンホールからタイルへの射影を計算
//                            double scale = (tile_pos - observer_z) / (0.0 - observer_z); // z=0からz=tile_posへの拡大率
//                            double view_x = pinhole_pos.x * scale;
//                            double view_y = pinhole_pos.y * scale;
//
//                            // タイル上での可視領域（ピンホールの開口を考慮）
//                            double pinhole_half = pinhole_size / 2.0;
//                            double view_pinhole_half = pinhole_half * scale;
//
//                            double visible_area_min_x = view_x - view_pinhole_half;
//                            double visible_area_max_x = view_x + view_pinhole_half;
//                            double visible_area_min_y = view_y - view_pinhole_half;
//                            double visible_area_max_y = view_y + view_pinhole_half;
//
//                            // タイルの範囲内に制限
//                            visible_area_min_x = std::max(-tile_size_mm / 2.0, visible_area_min_x);
//                            visible_area_max_x = std::min(tile_size_mm / 2.0, visible_area_max_x);
//                            visible_area_min_y = std::max(-tile_size_mm / 2.0, visible_area_min_y);
//                            visible_area_max_y = std::min(tile_size_mm / 2.0, visible_area_max_y);
//
//                            // 可視領域がタイル内に存在しない場合
//                            if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
//                            {
//                                // 黒色を設定
//                                output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                                continue;
//                            }
//
//                            // ピクセルインデックスの範囲を計算
//                            int x_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_x + tile_size_mm / 2) / pixel_size_mm)));
//                            int x_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_x + tile_size_mm / 2) / pixel_size_mm)));
//                            int y_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_y + tile_size_mm / 2) / pixel_size_mm)));
//                            int y_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_y + tile_size_mm / 2) / pixel_size_mm)));
//
//                            // 観察される色の加重平均を計算
//                            cv::Vec3d weighted_color(0, 0, 0);
//                            double total_area = 0.0;
//
//                            for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
//                            {
//                                // ピクセルのx座標範囲を計算
//                                double x_pixel_min = x_idx * pixel_size_mm - tile_size_mm / 2;
//                                double x_pixel_max = (x_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                                double x_overlap = std::max(0.0, std::min(visible_area_max_x, x_pixel_max) - std::max(visible_area_min_x, x_pixel_min));
//
//                                if (x_overlap <= 0) continue;
//
//                                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                                {
//                                    // ピクセルのy座標範囲を計算
//                                    double y_pixel_min = y_idx * pixel_size_mm - tile_size_mm / 2;
//                                    double y_pixel_max = (y_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                                    double y_overlap = std::max(0.0, std::min(visible_area_max_y, y_pixel_max) - std::max(visible_area_min_y, y_pixel_min));
//
//                                    if (y_overlap <= 0) continue;
//
//                                    double overlap_area = x_overlap * y_overlap;
//                                    total_area += overlap_area;
//
//                                    // ピクセル値を取得し、加重平均に加算
//                                    cv::Vec3b pixel = image.at<cv::Vec3b>(y_idx, x_idx);
//                                    weighted_color += cv::Vec3d(pixel) * overlap_area;
//                                }
//                            }
//
//                            cv::Vec3b final_color;
//                            if (total_area > 0)
//                            {
//                                weighted_color /= total_area;
//                                final_color = cv::Vec3b(
//                                    static_cast<uchar>(weighted_color[0]),
//                                    static_cast<uchar>(weighted_color[1]),
//                                    static_cast<uchar>(weighted_color[2])
//                                );
//                            }
//                            else
//                            {
//                                // 観察可能な領域がない場合は黒色
//                                final_color = cv::Vec3b(0, 0, 0);
//                            }
//
//                            // 出力画像に色を設定
//                            output_image.at<cv::Vec3b>(i, j) = final_color;
//                        }
//                    }
//
//                    // 出力画像を保存
//                    //std::string filenameout = "./images/lenna/observe-image/gridSize" + std::to_string(num_pinhole_per_axis) + "/real-observe-lenna-throughPinhole_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                    //std::string filenameout = "./images/lenna/observe-image/gridSize" + std::to_string(num_pinhole_per_axis) + "/weightAveraging-observe-lenna-throughPinhole_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                    std::string filenameout = "./images/lenna/observe-image/gridSize" + std::to_string(num_pinhole_per_axis) + "/v2-2-comparison-observe-lenna-throughPinhole_NumZLevel" + std::to_string(num_z_level) + "_Ddash" + std::to_string(static_cast<int>(Ddash)) + "_pitchTimes" + std::to_string(static_cast<int>(ptimes)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                    //std::string filenameout = "./images/lenna/observe-image/gridSize" + std::to_string(num_pinhole_per_axis) + "/DP-observe-lenna-throughPinhole_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                    cv::Mat resizedOutput;
//                    cv::resize(output_image, resizedOutput, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//                    cv::imwrite(filenameout, resizedOutput);
//
//                    std::cout << "出力画像を" << filenameout << "として保存しました。\n";
//
//                }
//            }
//        }
//    }
//
//    return 0;
//}

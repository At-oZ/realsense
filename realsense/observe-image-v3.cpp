//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//
//int main()
//{
//
//    // 画像のサイズとピクセルサイズを計算
//    const double tile_size_mm = 170.0;  // タイルのサイズ（mm）
//    const int image_size_px = 2261;      // 画像の解像度（ピクセル）
//    const double pixel_size_mm = tile_size_mm / image_size_px; // ピクセルサイズ（mm）
//    std::cout << "pixel size:" << pixel_size_mm << std::endl;
//
//    // ピンホールの設定
//    const int num_pinhole_per_axis = 34;     // 各軸のピンホール数
//    const double pinhole_spacing = 5.0;    // ピンホール間の間隔（mm）
//    const double pinhole_size = 0.2;        // ピンホールの一辺の長さ（mm）
//
//    // 観察者とタイルの位置
//    const double observer_z = -1450.0; // mm
//    const double tile_pos = 4.0; // mm
//    //const double subject_z = 512.0;
//
//    for (double subject_z = 4.0; subject_z <= 1024; subject_z *= 2) {
//
//        // 画像を読み込む
//        //std::string filenamein = "./images/lenna/weightedAveraging_captured_image_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//        std::string filenamein = "./images/lenna/weightedAveraging_captured_image_imageResolution64_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//        //std::string filenamein = "./images/DirectProjection_pdBase_img_display_z0_g4_wp1.2_pd0.075_D100.png";
//        //std::string filenamein = "./images/lenna.bmp";
//        //std::string filenamein = "./images/prop-v1-10_ImgDisplay_NumZLevel65_Ddash250_pitchTimes0.25_boxSize3.png";
//        cv::Mat image = cv::imread(filenamein);
//
//        if (image.empty())
//        {
//            std::cout << "画像を開くことができませんでした。\n";
//            return -1;
//        }
//
//
//        // ピンホールの位置を計算（x, y軸の正負を反転）
//        std::vector<cv::Point2d> pinhole_positions;
//        double offset = -((num_pinhole_per_axis - 1) * pinhole_spacing) / 2.0;
//
//        for (int i = 0; i < num_pinhole_per_axis; ++i)
//        {
//            for (int j = 0; j < num_pinhole_per_axis; ++j)
//            {
//                double x = (offset + j * pinhole_spacing);  // x軸はそのまま
//                double y = (offset + i * pinhole_spacing);  // y軸の反転
//                pinhole_positions.emplace_back(x, y);
//            }
//        }
//
//        // 出力画像を作成（4x4ピクセル）
//        cv::Mat output_image(num_pinhole_per_axis, num_pinhole_per_axis, CV_8UC3);
//
//        // 各ピンホールについて観察される色を計算
//        int index = 0;
//        for (int i = 0; i < num_pinhole_per_axis; ++i)
//        {
//            for (int j = 0; j < num_pinhole_per_axis; ++j)
//            {
//                const auto& pinhole_pos = pinhole_positions[index++];
//
//                // ピンホールを通して見えるタイル上の領域を計算
//                // 視線を通してピンホールからタイルへの射影を計算
//                double scale = (tile_pos - observer_z) / (0.0 - observer_z); // z=0からz=tile_posへの拡大率
//                double view_x = pinhole_pos.x * scale;
//                double view_y = pinhole_pos.y * scale;
//
//                // タイル上での可視領域（ピンホールの開口を考慮）
//                double pinhole_half = pinhole_size / 2.0;
//                double view_pinhole_half = pinhole_half * scale;
//
//                double visible_area_min_x = view_x - view_pinhole_half;
//                double visible_area_max_x = view_x + view_pinhole_half;
//                double visible_area_min_y = view_y - view_pinhole_half;
//                double visible_area_max_y = view_y + view_pinhole_half;
//
//                // タイルの範囲内に制限
//                visible_area_min_x = std::max(-tile_size_mm / 2.0, visible_area_min_x);
//                visible_area_max_x = std::min(tile_size_mm / 2.0, visible_area_max_x);
//                visible_area_min_y = std::max(-tile_size_mm / 2.0, visible_area_min_y);
//                visible_area_max_y = std::min(tile_size_mm / 2.0, visible_area_max_y);
//
//                // 可視領域がタイル内に存在しない場合
//                if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
//                {
//                    // 黒色を設定
//                    output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                    continue;
//                }
//
//                // ピクセルインデックスの範囲を計算
//                int x_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_x + tile_size_mm / 2) / pixel_size_mm)));
//                int x_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_x + tile_size_mm / 2) / pixel_size_mm)));
//                int y_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_y + tile_size_mm / 2) / pixel_size_mm)));
//                int y_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_y + tile_size_mm / 2) / pixel_size_mm)));
//
//                // 観察される色の加重平均を計算
//                cv::Vec3d weighted_color(0, 0, 0);
//                double total_area = 0.0;
//
//                for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
//                {
//                    // ピクセルのx座標範囲を計算
//                    double x_pixel_min = x_idx * pixel_size_mm - tile_size_mm / 2;
//                    double x_pixel_max = (x_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                    double x_overlap = std::max(0.0, std::min(visible_area_max_x, x_pixel_max) - std::max(visible_area_min_x, x_pixel_min));
//
//                    if (x_overlap <= 0) continue;
//
//                    for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                    {
//                        // ピクセルのy座標範囲を計算
//                        double y_pixel_min = y_idx * pixel_size_mm - tile_size_mm / 2;
//                        double y_pixel_max = (y_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                        double y_overlap = std::max(0.0, std::min(visible_area_max_y, y_pixel_max) - std::max(visible_area_min_y, y_pixel_min));
//
//                        if (y_overlap <= 0) continue;
//
//                        double overlap_area = x_overlap * y_overlap;
//                        total_area += overlap_area;
//
//                        // ピクセル値を取得し、加重平均に加算
//                        cv::Vec3b pixel = image.at<cv::Vec3b>(y_idx, x_idx);
//                        weighted_color += cv::Vec3d(pixel) * overlap_area;
//                    }
//                }
//
//                cv::Vec3b final_color;
//                if (total_area > 0)
//                {
//                    weighted_color /= total_area;
//                    final_color = cv::Vec3b(
//                        static_cast<uchar>(weighted_color[0]),
//                        static_cast<uchar>(weighted_color[1]),
//                        static_cast<uchar>(weighted_color[2])
//                    );
//                }
//                else
//                {
//                    // 観察可能な領域がない場合は黒色
//                    final_color = cv::Vec3b(0, 0, 0);
//                }
//
//                // 出力画像に色を設定
//                output_image.at<cv::Vec3b>(i, j) = final_color;
//            }
//        }
//
//        // 出力画像を保存
//        //std::string filenameout = "./images/lenna/weightedAveraging_observe-lenna-throughDisplay_zi" + std::to_string(static_cast<int>(subject_z)) + "_pinholeSize" + std::to_string(static_cast<int>(pinhole_size*1000)) + ".png";
//        //std::string filenameout = "./images/v2-observe-DirectProjection_pdBase_img_display_z0_g4_wp1.2_pd0.075_D100.png";
//        std::string filenameout = "./images/lenna/observe-lenna-throughPinhole_imageResolution64_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//        //std::string filenameout = "./images/v2-observe_prop-v1-10_ImgDisplay_NumZLevel65_Ddash250_pitchTimes0.25_boxSize3.png";
//        cv::Mat resizedOutput;
//        cv::resize(output_image, resizedOutput, cv::Size(1024, 1024), 0, 0, cv::INTER_NEAREST);
//        cv::imwrite(filenameout, resizedOutput);
//
//        std::cout << "出力画像を" << filenameout << "として保存しました。\n";
//
//
//    }
//
//    return 0;
//}

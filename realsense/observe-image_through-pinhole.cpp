///* ピンホールを介して要素画像群を観察するシミュレーション */
//
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <random>
//#include <Windows.h>
//
//#define NOMINMAX
//
//using namespace cv;
//using namespace std;
//
//cv::Mat ideal_observe(cv::Mat input_image, int num_pinhole_per_axis, double pinhole_pitch, double focal_length, double observer_x, double observer_y, double observer_z, double pinhole_size, double display_image_size, double display_px_size, int display_image_px);
//
//int main(int argc, char* argv[]) {
//
//    ostringstream stream;
//
//    // ピクセルサイズの計算
//    const double coef = 1.0; // 倍率
//    const double display_px_size = 13.4 * 25.4 / std::sqrt(3840 * 3840 + 2400 * 2400) / coef; // ディスプレイのピクセルサイズ
//    int element_image_px = 60; // 要素画像の解像度
//    double intv = (double)element_image_px; // 要素画像の間隔
//
//    // 観察者と表示系の設定
//    const double observer_z = 1000.0;
//    const int num_pinhole = 40;
//    const double focal_length = observer_z / (3 * num_pinhole - 1); // レンズアレイの焦点距離
//    const double pinhole_pitch = element_image_px * display_px_size;    // ピンホール間の間隔（mm）(wideview:observer_z / (focal_length + observer_z) * )
//    double pinhole_array_size = pinhole_pitch * num_pinhole;   // 各軸方向のピンホールアレイのサイズ
//    double display_area_size = element_image_px * num_pinhole * display_px_size; //表示画像の大きさ
//    int display_px = element_image_px * num_pinhole; // 表示画像の解像度
//    const double pinhole_size = display_px_size;        // ピンホールの一辺の長さ（mm）
//    const double observer_x = 0; // (normal端:pinhole_pitch * (observer_z / focal_length - (num_pinhole - 1)) * 0.5, wideview端:observer_z / focal_length * element_image_px * display_px_size * 0.5)
//    const double observer_y = 0.0;
//
//    std::cout << "observe x:" << observer_x << ", observe y:" << observer_y << std::endl;
//    
//    // タイルの設定
//    const int tile_px = 256; // タイルの解像度
//    const double tile_pos = 300.0; // タイルの位置
//    const double tile_size = display_area_size * (tile_pos + observer_z) / observer_z; // 拡大の場合 * (subz + observer_z_min) / observer_z_minを付ける
//    const double tile_px_size = tile_size / tile_px; // タイルに貼る画像のピクセルサイズ（mm）
//
//
//    // 表示画像の設定
//    stream.str("");
//    stream << "D:/EvacuatedStorage/prop-reconstruction/IE-prop-normal-v1/prop-normal-v1-mandrill_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << tile_size << "_zi" << (int)tile_pos << ".png";
//    std::string filename_display_image = stream.str();
//    cv::Mat display_image = cv::imread(filename_display_image);
//    const int display_image_px = display_image.cols; // 画像から横方向の解像度を取得
//    //const double display_image_size = (focal_length + observer_z) / observer_z * pinhole_pitch * nph;
//    const double display_image_size = display_px_size * display_image_px;
//
//    std::cout << "display image px:" << display_image_px << ", display image size:" << display_image_size << std::endl;
//
//    // 観察画像の生成
//    cv::Mat observe_image = ideal_observe(display_image, num_pinhole, pinhole_pitch, focal_length, observer_x, observer_y, -observer_z, pinhole_size, display_image_size, display_px_size, display_image_px);
//    cv::Mat resized_obsv_img;
//    cv::resize(observe_image, resized_obsv_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//
//    // 観察画像の保存
//    stream.str("");
//    stream << "D:/EvacuatedStorage/observe-images/IE-prop-normal-v1/observe_IE-prop-normal-v1-mandrill_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << tile_size << "_zi" << (int)tile_pos << "_xo" << std::fixed << std::setprecision(2) << observer_x << "_yo" << std::fixed << std::setprecision(2) << observer_y << "_zo" << std::fixed << std::setprecision(2) << observer_z << ".png";
//    std::string filenameout = stream.str();
//    cv::imwrite(filenameout, resized_obsv_img);
//
//    MessageBeep(-1);
//    return 0;
//}
//
//cv::Mat ideal_observe(cv::Mat input_image, int num_pinhole_per_axis, double pinhole_pitch, double focal_length, double observer_x, double observer_y, double observer_z, double pinhole_size, double display_image_size, double display_px_size, int display_image_px) {
//
//    // ピンホールの位置を計算
//    std::vector<cv::Point2d> pinhole_positions;
//    double offset = -((num_pinhole_per_axis - 1) * pinhole_pitch) / 2.0;
//
//    for (int i = 0; i < num_pinhole_per_axis; ++i)
//    {
//        for (int j = 0; j < num_pinhole_per_axis; ++j)
//        {
//            double x = (offset + j * pinhole_pitch);
//            double y = (offset + i * pinhole_pitch);
//            pinhole_positions.emplace_back(x, y);
//        }
//    }
//
//    cv::Mat output_image(num_pinhole_per_axis, num_pinhole_per_axis, CV_8UC3);    // 各ピンホールについて観察される色を計算
//
//    // 各ピンホールについて観察される色を計算
//    int index = 0;
//    for (int i = 0; i < num_pinhole_per_axis; ++i)
//    {
//        for (int j = 0; j < num_pinhole_per_axis; ++j)
//        {
//            const auto& pinhole_pos = pinhole_positions[index++];
//
//            // ピンホールを通して見えるタイル上の領域を計算
//            // 視線を通してピンホールからタイルへの射影を計算
//            double scale = (focal_length - observer_z) / (0.0 - observer_z); // z=observe_zからz=focal_lengthへの拡大率
//            double view_x = (pinhole_pos.x - observer_x) * scale + observer_x;
//            double view_y = (pinhole_pos.y - observer_y) * scale + observer_y;
//
//            // タイル上での可視領域（ピンホールの開口を考慮）
//            double pinhole_half = pinhole_size / 2.0;
//            double view_pinhole_half = pinhole_half * scale;
//
//            double visible_area_min_x = view_x - view_pinhole_half;
//            double visible_area_max_x = view_x + view_pinhole_half;
//            double visible_area_min_y = view_y - view_pinhole_half;
//            double visible_area_max_y = view_y + view_pinhole_half;
//
//            // タイルの範囲内に制限
//            visible_area_min_x = (std::max)(-display_image_size / 2.0, visible_area_min_x);
//            visible_area_max_x = (std::min)(display_image_size / 2.0, visible_area_max_x);
//            visible_area_min_y = (std::max)(-display_image_size / 2.0, visible_area_min_y);
//            visible_area_max_y = (std::min)(display_image_size / 2.0, visible_area_max_y);
//
//            // 可視領域がタイル内に存在しない場合
//            if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
//            {
//                // 黒色を設定
//                output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                continue;
//            }
//
//            // ピクセルインデックスの範囲を計算
//            int x_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_x + display_image_size / 2) / display_px_size)));
//            int x_max_idx = (std::min)(display_image_px - 1, static_cast<int>(std::floor((visible_area_max_x + display_image_size / 2) / display_px_size)));
//            int y_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_y + display_image_size / 2) / display_px_size)));
//            int y_max_idx = (std::min)(display_image_px - 1, static_cast<int>(std::floor((visible_area_max_y + display_image_size / 2) / display_px_size)));
//
//            //std::cout << "x min idx:" << x_min_idx << ", x max idx:" << x_max_idx << ", y min idx:" << y_min_idx << ", y max idx:" << y_max_idx << std::endl;
//
//            // 観察される色の加重平均を計算
//            cv::Vec3d weighted_color(0, 0, 0);
//            double total_area = 0.0;
//
//            for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
//            {
//                // ピクセルのx座標範囲を計算
//                double x_pixel_min = x_idx * display_px_size - display_image_size / 2;
//                double x_pixel_max = (x_idx + 1) * display_px_size - display_image_size / 2;
//
//                double x_overlap = (std::max)(0.0, (std::min)(visible_area_max_x, x_pixel_max) - (std::max)(visible_area_min_x, x_pixel_min));
//
//                if (x_overlap <= 0) continue;
//
//                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                {
//                    // ピクセルのy座標範囲を計算
//                    double y_pixel_min = y_idx * display_px_size - display_image_size / 2;
//                    double y_pixel_max = (y_idx + 1) * display_px_size - display_image_size / 2;
//
//                    double y_overlap = (std::max)(0.0, (std::min)(visible_area_max_y, y_pixel_max) - (std::max)(visible_area_min_y, y_pixel_min));
//
//                    if (y_overlap <= 0) continue;
//
//                    double overlap_area = x_overlap * y_overlap;
//                    total_area += overlap_area;
//
//                    // ピクセル値を取得し、加重平均に加算
//                    cv::Vec3b pixel = input_image.at<cv::Vec3b>(y_idx, x_idx);
//                    weighted_color += cv::Vec3d(pixel) * overlap_area;
//                }
//            }
//
//            cv::Vec3b final_color;
//            if (total_area > 0)
//            {
//                weighted_color /= total_area;
//                final_color = cv::Vec3b(
//                    static_cast<uchar>(weighted_color[0]),
//                    static_cast<uchar>(weighted_color[1]),
//                    static_cast<uchar>(weighted_color[2])
//                );
//            }
//            else
//            {
//                // 観察可能な領域がない場合は黒色
//                final_color = cv::Vec3b(0, 0, 0);
//            }
//
//            // 出力画像に色を設定
//            output_image.at<cv::Vec3b>(i, j) = final_color;
//        }
//    }
//
//    return output_image;
//}

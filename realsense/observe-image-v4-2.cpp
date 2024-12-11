//// ピンホールを通して画像を観察したときの観察画像の作成
///*
//    視点を中央のみならず任意の視点から観察したときの観察画像を取得
//    （ピンホール越しに直接タイルを見たとき、点群を介さない方法で表示系を見たとき用）
//
//    注意！直接観察と理想表示の間で変えるときはimage_size_pxとtile_posを変えることを忘れずに！
//*/
//
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//#include <random>
//#include <filesystem>
//
//namespace fs = std::filesystem;
//
//int main()
//{
//
//    // ピクセルサイズの計算
//    const double coef = 1.0;
//    const double pixel_size = 13.4 / std::sqrt(3840 * 3840 + 2400 * 2400) * 25.4 / coef;
//    std::cout << "camera pixel size:" << pixel_size << std::endl;
//
//    // ディスプレイの設定
//    const int display_image_px = 2400 * coef;
//    const double display_area_size = display_image_px * pixel_size;
//    cv::Mat display_image = cv::Mat::zeros(cv::Size(display_image_px, display_image_px), CV_8UC3);
//
//    // タイルの設定
//    const double tile_size_mm = display_area_size;  // タイルのサイズ（mm）
//    const int image_size_px = display_image_px;      // タイルに貼る画像の解像度（lennaの画像を直接みるときは256px, 表示系の再現の場合はdisplay_image_pxと同じ）
//    const double pixel_size_mm = tile_size_mm / image_size_px; // タイルに貼る画像のピクセルサイズ（mm）
//    std::cout << "pixel size:" << pixel_size_mm << std::endl;
//
//    //std::random_device seed_gen;
//    int seedx = 0;
//    int seedy = 1;
//    std::mt19937 enginex(seedx);
//    std::mt19937 enginey(seedy);
//
//    std::uniform_real_distribution<double> dist(-display_area_size / 2.0, display_area_size / 2.0);
//
//    int rand_size = 1;
//    double* observer_x = (double*)malloc(sizeof(double) * rand_size);
//    double* observer_y = (double*)malloc(sizeof(double) * rand_size);
//
//    for (int i = 0; i < rand_size; i++) {
//        //observer_x[i] = dist(enginex);
//        //observer_y[i] = dist(enginey);
//        observer_x[i] = 0.0;
//        observer_y[i] = 0.0;
//
//        //std::cout << "observer_x:" << observer_x[i] << ", observer_y:" << observer_y[i] << std::endl;
//    }
//
//    int nphs[3] = { 40, 80, 160 };
//
//    for (int idx_nph = 0; idx_nph < 3; idx_nph++) {
//
//        int nph = nphs[idx_nph];
//
//        //fs::path dir = "./images/lenna/observe-image/ideal_pixelSize" + std::to_string(static_cast<int>(pixel_size * 1000)) + "um/gridSize" + std::to_string(nph); // 作成するディレクトリのパス
//
//        //// ディレクトリが存在しない場合に作成
//        //if (!fs::exists(dir)) {
//        //    fs::create_directory(dir);
//        //    std::cout << "ディレクトリを作成しました: " << dir << std::endl;
//        //}
//        //else {
//        //    std::cout << "ディレクトリは既に存在します: " << dir << std::endl;
//        //}
//
//        for (double subz = 512.0; subz <= 512.0; subz *= 2) {
//
//            // ピンホールの設定
//            const int num_pinhole_per_axis = nph;     // 各軸のピンホール数
//            const double pinhole_spacing = display_area_size / num_pinhole_per_axis;    // ピンホール間の間隔（mm）
//            const double pinhole_size = 0.15;        // ピンホールの一辺の長さ（mm）
//
//            // 観察者とタイルの位置
//            const double observer_z = -1500.0; // mm
//            const double tile_pos = -observer_z / (2.0 / (1.0 / (double)nph - 0.15 / (double)(display_image_px * pixel_size)) - 1.0); // mm (lennaの画像を直接みるときはsubzと同じ, 表示系の再現の場合はギャップと同じ（4.0 * 160 / (double)nph mm）)
//            const int image_resolution = static_cast<int>(floor(display_image_px / num_pinhole_per_axis)); // 画像の解像度（ピクセル）
//            const double subject_z = subz;
//
//            for (int nobs = 0; nobs < rand_size; nobs++) {
//
//
//                // 画像を読み込む
//                //std::string filenamein = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal_pixelSize" + std::to_string(static_cast<int>(pixel_size * 1000)) + "um/ideal_captured_image_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                std::string filenamein = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal/ideal_captured_image_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + ".png";
//                //std::string filenamein = "./images/lenna.bmp";
//                cv::Mat image = cv::imread(filenamein);
//
//                if (image.empty())
//                {
//                    std::cout << "画像を開くことができませんでした。\n";
//                    return -1;
//                }
//
//
//                // ピンホールの位置を計算
//                std::vector<cv::Point2d> pinhole_positions;
//                double offset = -((num_pinhole_per_axis - 1) * pinhole_spacing) / 2.0;
//
//                for (int i = 0; i < num_pinhole_per_axis; ++i)
//                {
//                    for (int j = 0; j < num_pinhole_per_axis; ++j)
//                    {
//                        double x = (offset + j * pinhole_spacing);
//                        double y = (offset + i * pinhole_spacing);
//                        pinhole_positions.emplace_back(x, y);
//                    }
//                }
//
//                // 出力画像を作成
//                cv::Mat output_image(num_pinhole_per_axis, num_pinhole_per_axis, CV_8UC3);
//
//                // 各ピンホールについて観察される色を計算
//                int index = 0;
//                for (int i = 0; i < num_pinhole_per_axis; ++i)
//                {
//                    for (int j = 0; j < num_pinhole_per_axis; ++j)
//                    {
//                        const auto& pinhole_pos = pinhole_positions[index++];
//
//                        // ピンホールを通して見えるタイル上の領域を計算
//                        // 視線を通してピンホールからタイルへの射影を計算
//                        double scale = (tile_pos - observer_z) / (0.0 - observer_z); // z=observe_zからz=tile_posへの拡大率
//                        double view_x = (pinhole_pos.x - observer_x[nobs]) * scale + observer_x[nobs];
//                        double view_y = (pinhole_pos.y - observer_y[nobs]) * scale + observer_y[nobs];
//
//                        // タイル上での可視領域（ピンホールの開口を考慮）
//                        double pinhole_half = pinhole_size / 2.0;
//                        double view_pinhole_half = pinhole_half * scale;
//
//                        double visible_area_min_x = view_x - view_pinhole_half;
//                        double visible_area_max_x = view_x + view_pinhole_half;
//                        double visible_area_min_y = view_y - view_pinhole_half;
//                        double visible_area_max_y = view_y + view_pinhole_half;
//
//                        // タイルの範囲内に制限
//                        visible_area_min_x = std::max(-tile_size_mm / 2.0, visible_area_min_x);
//                        visible_area_max_x = std::min(tile_size_mm / 2.0, visible_area_max_x);
//                        visible_area_min_y = std::max(-tile_size_mm / 2.0, visible_area_min_y);
//                        visible_area_max_y = std::min(tile_size_mm / 2.0, visible_area_max_y);
//
//                        // 可視領域がタイル内に存在しない場合
//                        if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
//                        {
//                            // 黒色を設定
//                            output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                            continue;
//                        }
//
//                        // ピクセルインデックスの範囲を計算
//                        int x_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_x + tile_size_mm / 2) / pixel_size_mm)));
//                        int x_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_x + tile_size_mm / 2) / pixel_size_mm)));
//                        int y_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_y + tile_size_mm / 2) / pixel_size_mm)));
//                        int y_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_y + tile_size_mm / 2) / pixel_size_mm)));
//
//                        //std::cout << "x min idx:" << x_min_idx << ", x max idx:" << x_max_idx << ", y min idx:" << y_min_idx << ", y max idx:" << y_max_idx << std::endl;
//
//                        // 観察される色の加重平均を計算
//                        cv::Vec3d weighted_color(0, 0, 0);
//                        double total_area = 0.0;
//
//                        for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
//                        {
//                            // ピクセルのx座標範囲を計算
//                            double x_pixel_min = x_idx * pixel_size_mm - tile_size_mm / 2;
//                            double x_pixel_max = (x_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                            double x_overlap = std::max(0.0, std::min(visible_area_max_x, x_pixel_max) - std::max(visible_area_min_x, x_pixel_min));
//
//                            if (x_overlap <= 0) continue;
//
//                            for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                            {
//                                // ピクセルのy座標範囲を計算
//                                double y_pixel_min = y_idx * pixel_size_mm - tile_size_mm / 2;
//                                double y_pixel_max = (y_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                                double y_overlap = std::max(0.0, std::min(visible_area_max_y, y_pixel_max) - std::max(visible_area_min_y, y_pixel_min));
//
//                                if (y_overlap <= 0) continue;
//
//                                double overlap_area = x_overlap * y_overlap;
//                                total_area += overlap_area;
//
//                                // ピクセル値を取得し、加重平均に加算
//                                cv::Vec3b pixel = image.at<cv::Vec3b>(y_idx, x_idx);
//                                weighted_color += cv::Vec3d(pixel) * overlap_area;
//                            }
//                        }
//
//                        cv::Vec3b final_color;
//                        if (total_area > 0)
//                        {
//                            weighted_color /= total_area;
//                            final_color = cv::Vec3b(
//                                static_cast<uchar>(weighted_color[0]),
//                                static_cast<uchar>(weighted_color[1]),
//                                static_cast<uchar>(weighted_color[2])
//                            );
//                        }
//                        else
//                        {
//                            // 観察可能な領域がない場合は黒色
//                            final_color = cv::Vec3b(0, 0, 0);
//                        }
//
//                        // 出力画像に色を設定
//                        output_image.at<cv::Vec3b>(i, j) = final_color;
//                    }
//                }
//
//                // 出力画像を保存
//                //std::string filenameout = "./images/lenna/observe-image/real/gridSize" + std::to_string(num_pinhole_per_axis) + "/real-observe-lenna_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                std::string filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/ideal/gridSize" + std::to_string(num_pinhole_per_axis) + "/ideal-observe-lenna-throughPinhole_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                //std::string filenameout = "V:/images/lenna/observe-image/real/gridSize" + std::to_string(num_pinhole_per_axis) + "/real-observe-lenna_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                //std::string filenameout = "V:/images/lenna/observe-image/ideal/gridSize" + std::to_string(num_pinhole_per_axis) + "/ideal-observe-lenna-throughPinhole_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                cv::Mat resizedOutput;
//                cv::resize(output_image, resizedOutput, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//                cv::imwrite(filenameout, resizedOutput);
//
//                std::cout << "出力画像を" << filenameout << "として保存しました。\n";
//
//            }
//        }
//    }
//
//    free(observer_x);
//    free(observer_y);
//
//    return 0;
//}

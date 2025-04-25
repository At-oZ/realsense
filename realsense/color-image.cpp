//// License: Apache 2.0. See LICENSE file in root directory.
//// Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.
//
//#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
//#include <opencv2/opencv.hpp>
//#include <algorithm>            // std::min, std::max
//#include <vector>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <chrono>
//#include <thread>
//#include <cmath>
//
//int writeCSV2(const std::vector<std::vector<double>> array);
//
//void insert_pixels(int Hpin, int Wpin, int N, int D, double g, double wp, double inv_wp, int starty, int endy, std::vector<std::vector<double>>& uo, std::vector<std::vector<double>>& vo, std::vector<std::vector<cv::Mat>>& elem_img, std::vector<std::vector<double>>& pointclouddata) {
//
//    double inv_d;
//    for (int k = 0; k < pointclouddata.size(); k++) {
//
//        if (k % 1000 == 0) {
//            std::cout << "k:" << k << "\n" << std::endl;
//        }
//
//        std::vector<std::vector<double>> newx(endy - starty, std::vector<double>(Wpin));
//        std::vector<std::vector<double>> newy(endy - starty, std::vector<double>(Wpin));
//
//        std::vector<std::vector<int>> nx(endy - starty, std::vector<int>(Wpin));
//        std::vector<std::vector<int>> ny(endy - starty, std::vector<int>(Wpin));
//
//        inv_d = 1.0 / (pointclouddata[k][2] * 1000 + D);
//
//        for (int i = 0; i < endy - starty; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//                newx[i][j] = -(g * inv_d) * (uo[i + starty][j] - pointclouddata[k][0] * 1000);
//                newy[i][j] = -(g * inv_d) * (vo[i + starty][j] - pointclouddata[k][1] * 1000);
//
//                nx[i][j] = static_cast<int>(floor((newx[i][j] + 0.5 * wp) * inv_wp * N));
//                ny[i][j] = static_cast<int>(floor((newy[i][j] + 0.5 * wp) * inv_wp * N));
//
//                //std::cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << std::endl;
//                //std::cout << "nx:" << nx[i][j] << ", ny:" << ny[i][j] << std::endl;
//
//                if (0 <= nx[i][j] && nx[i][j] < N && 0 <= ny[i][j] && ny[i][j] < N) {
//                    if (elem_img[i + starty][j].at<cv::Vec3b>((ny[i][j]), (nx[i][j]))[0] == 0 && elem_img[i + starty][j].at<cv::Vec3b>((ny[i][j]), (nx[i][j]))[1] == 0 && elem_img[i + starty][j].at<cv::Vec3b>((ny[i][j]), (nx[i][j]))[2] == 0) {
//
//                        elem_img[i + starty][j].at<cv::Vec3b>(static_cast<int>(ny[i][j]), static_cast<int>(nx[i][j]))[0] = (int)pointclouddata[k][3];
//                        elem_img[i + starty][j].at<cv::Vec3b>(static_cast<int>(ny[i][j]), static_cast<int>(nx[i][j]))[1] = (int)pointclouddata[k][4];
//                        elem_img[i + starty][j].at<cv::Vec3b>(static_cast<int>(ny[i][j]), static_cast<int>(nx[i][j]))[2] = (int)pointclouddata[k][5];
//
//                    }
//                }
//            }
//        }
//    }
//}
//
//int main(int argc, char* argv[]) try
//{
//
//    // 表示系のパラメータ
//    int W = 170, H = 170;                                                                       // 表示の縦横幅
//    double g = 4, wp = 2.5, a = 0.125;                                                          // ギャップ、ピッチ、ピンホール幅
//    double ld = 13.4 * 25.4;                                                                    // ディスプレイサイズ
//    int pw = 3840, ph = 2400;                                                                   // ディスプレイの縦横の解像度
//    double pd = ld / sqrtf(pw * pw + ph * ph);                                                  // 画素ピッチ
//    int N = static_cast<int>(floor(wp / pd));                                                   // 要素画像の解像度
//    int pw_disp = static_cast<int>(floor(W / pd)), ph_disp = static_cast<int>(floor(H / pd));   // 表示画像の縦横の解像度
//    int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp));         // 縦横のピンホール数
//    double intv = wp / pd;                                                                      // 要素画像の間隔
//
//    // 要素画像群
//    std::vector<std::vector<cv::Mat>> elem_img(Hpin, std::vector<cv::Mat>(Wpin));
//    for (int i = 0; i < Hpin; ++i) {
//        for (int j = 0; j < Wpin; ++j) {
//            elem_img[i][j] = cv::Mat(cv::Size(N, N), CV_8UC3, cv::Scalar(0, 0, 0));
//        }
//    }
//
//    // 視点パラメータ(mm)
//    std::vector<std::vector<double>> uo(Hpin, std::vector<double>(Wpin));
//    std::vector<std::vector<double>> vo(Hpin, std::vector<double>(Wpin));
//
//    for (int i = 0; i < Hpin; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            uo[i][j] = (j - static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp;
//            vo[i][j] = (i - static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp;
//
//            //std::cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << std::endl;
//
//        }
//    }
//
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//
//    int rows = WIDTH * HEIGHT;
//    std::vector<std::vector<double>> data(rows, std::vector<double>(6)); // 横：subz, 縦：ptimes
//
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
//
//    int D = 10;                     // 表示系とカメラとの距離(mm)
//    double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//    double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//    //std::cout << "inv_WIDTH" << inv_WIDTH << std::endl;
//
//    //rs2::sensor sensor;
//    //auto depth_scale = rs2::depth_sensor(sensor).get_depth_scale();
//    //double clip_distance = 1;
//    //clip_distance = clip_distance / depth_scale;
//
//    // Declare pointcloud object, for calculating pointclouds and texture mappings
//    rs2::pointcloud pc;
//    // We want the points object to be persistent so we can display the last cloud when a frame drops
//    rs2::points points;
//
//    // Declare RealSense pipeline, encapsulating the actual device and sensors
//    rs2::pipeline pipe;
//    // Start streaming with default recommended configuration
//    pipe.start();
//
//    auto profile = pipe.get_active_profile();
//    auto colorSensors = profile.get_device().query_sensors()[1];
//    auto irSensor = profile.get_device().query_sensors()[0];
//    std::cout << colorSensors.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
//    std::cout << irSensor.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
//    colorSensors.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
//    colorSensors.set_option(RS2_OPTION_EXPOSURE, 150);
//    colorSensors.set_option(RS2_OPTION_GAIN, 64);
//    irSensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
//    irSensor.set_option(RS2_OPTION_LASER_POWER, 360);
//
//    rs2::colorizer color_map;
//    rs2::align align(RS2_STREAM_COLOR);
//
//    for (int i = 0; i < 3; i++)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        cv::waitKey(10);
//    }
//
//    while(true) {
//
//        //std::cout << tt << std::endl;
//
//        // 計測開始時間
//        auto start = std::chrono::high_resolution_clock::now();
//
//        // Wait for the next set of frames from the camera
//        auto frames = pipe.wait_for_frames();
//        auto aligned_frames = align.process(frames);
//
//        //rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
//        rs2::video_frame color_frame = aligned_frames.get_color_frame();
//        rs2::video_frame depth_frame = aligned_frames.get_depth_frame();
//
//        cv::Mat color_image_rgb(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
//        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        cv::Mat color_image;
//        cv::cvtColor(color_image_rgb, color_image, cv::COLOR_RGB2BGR);
//
//        // Generate the pointcloud and texture mappings
//        points = pc.calculate(depth_frame);
//
//        // Tell pointcloud object to map to this color frame
//        pc.map_to(color_frame);
//
//        auto verts = points.get_vertices();
//
//        // 画像を表示
//        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        cv::imshow("Display window", color_image);
//
//        // 任意のキーが押されるまで待つ
//        if (cv::waitKey(10) == 27)  // ESCキーで終了
//        {
//
//            for (int k = 0; k < rows; k++) {
//
//                data[k][0] = verts[k].x * 1000.0;
//                data[k][1] = verts[k].y * 1000.0;
//                data[k][2] = verts[k].z * 1000.0 + D;
//                data[k][3] = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
//                data[k][4] = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
//                data[k][5] = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];
//            }
//
//            writeCSV2(data);
//            imwrite("new_color_image.png", color_image);
//            cv::destroyAllWindows;
//            pipe.stop();
//
//            break;
//
//        }
//    }
//
//
//    return EXIT_SUCCESS;
//}
//catch (const rs2::error& e)
//{
//    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
//    return EXIT_FAILURE;
//}
//catch (const std::exception& e)
//{
//    std::cerr << e.what() << std::endl;
//    return EXIT_FAILURE;
//}
//
//int writeCSV2(const std::vector<std::vector<double>> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/pointcloud.csv");
//
//    // ファイルが正しく開けたか確認
//    if (!file.is_open()) {
//        std::cerr << "ファイルが開けませんでした" << std::endl;
//        return 1;
//    }
//
//    // 2次元配列の内容をCSV形式でファイルに書き込む
//    for (const auto& row : array) {
//        for (size_t i = 0; i < row.size(); ++i) {
//            file << row[i];
//            if (i < row.size() - 1) {
//                file << ","; // 各要素の間にカンマを挿入
//            }
//        }
//        file << "\n"; // 行の終わりに改行を挿入
//    }
//
//    // ファイルを閉じる
//    file.close();
//
//    std::cout << "書き込みが完了しました。" << std::endl;
//
//    return 0;
//}

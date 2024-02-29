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
//void insert_pixels(int Wpin, int N, int k, int starty, int endy, std::vector<std::vector<int>>& nx, std::vector<std::vector<int>>& ny, std::vector<std::vector<cv::Mat>>& elem_img, std::vector<std::vector<double>>& pointclouddata) {
//
//    //std::vector<int> rgb(3);
//
//    for (int i = starty; i < endy; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            //std::cout << "i:" << i << ", j:" << j << std::endl;
//            //std::cout << "nx:" << nx[i][j] << ", ny:" << ny[i][j] << std::endl;
//
//            if (0 <= nx[i][j] && nx[i][j] < N && 0 <= ny[i][j] && ny[i][j] < N) {
//                if (elem_img[i][j].at<cv::Vec3b>((ny[i][j]), (nx[i][j]))[0] == 0 && elem_img[i][j].at<cv::Vec3b>((ny[i][j]), (nx[i][j]))[1] == 0 && elem_img[i][j].at<cv::Vec3b>((ny[i][j]), (nx[i][j]))[2] == 0) {
//
//                    elem_img[i][j].at<cv::Vec3b>(static_cast<int>(ny[i][j]), static_cast<int>(nx[i][j]))[0] = (int)pointclouddata[k][3];
//                    elem_img[i][j].at<cv::Vec3b>(static_cast<int>(ny[i][j]), static_cast<int>(nx[i][j]))[1] = (int)pointclouddata[k][4];
//                    elem_img[i][j].at<cv::Vec3b>(static_cast<int>(ny[i][j]), static_cast<int>(nx[i][j]))[2] = (int)pointclouddata[k][5];
//
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
//    //while (true) // Application still alive?
//    //{
//    //    // Wait for the next set of frames from the camera
//    //    auto frames = pipe.wait_for_frames();
//
//    //    auto color = frames.get_color_frame();
//
//    //    // For cameras that don't have RGB sensor, we'll map the pointcloud to infrared instead of color
//    //    if (!color)
//    //        color = frames.get_infrared_frame();
//
//    //    // Tell pointcloud object to map to this color frame
//    //    pc.map_to(color);
//
//    //    auto depth = frames.get_depth_frame();
//
//    //    // Generate the pointcloud and texture mappings
//    //    points = pc.calculate(depth);
//
//    //    // Upload the color frame to OpenGL
//    //    app_state.tex.upload(color);
//
//    //    // Draw the pointcloud
//    //    draw_pointcloud(app.width(), app.height(), app_state, points);
//    //}
//
//    for (int tt = 0; tt < 1; tt++) {
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
//        cv::Mat color_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
//        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        // 画像を表示
//        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        cv::imshow("Display window", color_image);
//
//        // 任意のキーが押されるまで待つ
//        cv::waitKey(0);
//
//        std::cout << "before pc calc" << std::endl;
//
//        // Generate the pointcloud and texture mappings
//        points = pc.calculate(depth_frame);
//
//        // Tell pointcloud object to map to this color frame
//        pc.map_to(color_frame);
//
//        auto verts = points.get_vertices();
//
//        cv::Mat img_display(cv::Size(ph_disp, pw_disp), CV_8UC3);
//
//        std::cout << "before raw_pointclouddata" << std::endl;
//
//        std::vector<std::vector<double>> raw_pointclouddata(WIDTH * HEIGHT, std::vector<double>(6));
//        for (int i = 0; i < WIDTH * HEIGHT; i++) {
//            for (int j = 0; j < 6; j++) {
//
//                //std::cout << "x:" << i % WIDTH << ", y:" << static_cast<int>(i * inv_WIDTH) << std::endl;
//
//                if (j == 0) {
//                    raw_pointclouddata[i][j] = verts[i].x;
//                }
//                else if (j == 1) {
//                    raw_pointclouddata[i][j] = verts[i].y;
//                }
//                else if (j == 2) {
//                    raw_pointclouddata[i][j] = verts[i].z;
//                }
//                else if (j == 3) {
//                    raw_pointclouddata[i][j] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[2];
//                }
//                else if (j == 4) {
//                    raw_pointclouddata[i][j] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[1];
//                }
//                else {
//                    raw_pointclouddata[i][j] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[0];
//                }
//
//            }
//        }
//
//        std::cout << "before sort raw_pointclouddata" << std::endl;
//
//        // ソートする列のインデックス（0から始まる）
//        int columnIndex = 2;
//
//        // カスタム比較関数
//        auto compare = [columnIndex](const std::vector<double>& a, const std::vector<double>& b) {
//            return a[columnIndex] < b[columnIndex];
//        };
//
//        // 特定の列に基づいて全体を並べ替え
//        std::sort(raw_pointclouddata.begin(), raw_pointclouddata.end(), compare);
//
//        std::cout << "before create pointclouddata" << std::endl;
//
//        // 条件に一致する行を削除する新たな二次元配列Bを作成
//        std::vector<std::vector<double>> pointclouddata;
//
//        // 2列目が0でない行だけをBにコピー
//        std::copy_if(raw_pointclouddata.begin(), raw_pointclouddata.end(), std::back_inserter(pointclouddata), [](const std::vector<double>& row) {
//            return row[2] > 0; // zが0ではない行を選択
//        });
//
//        //// 出力ファイルを開く
//        //std::ofstream outputFile("output.csv");
//
//        //if (!outputFile.is_open()) {
//        //    std::cerr << "ファイルを開けませんでした。" << std::endl;
//        //    return 1;
//        //}
//
//        //// 二次元配列をループで走査し、CSV形式でファイルに出力
//        //for (int i = 0; i < pointclouddata.size(); i++) {
//        //    for (int j = 0; j < 6; j++) {
//        //        outputFile << pointclouddata[i][j];
//        //        if (j < 5) {
//        //            outputFile << ","; // 列の間にカンマを出力
//        //        }
//        //    }
//        //    outputFile << "\n"; // 行の終わりに改行を出力
//        //}
//
//        //// ファイルを閉じる
//        //outputFile.close();
//
//        //std::cout << "CSVファイルへの出力が完了しました。" << std::endl;
//
//        double inv_d;
//        for (int k = 0; k < pointclouddata.size(); k++) {
//
//            if (k % 1000 == 0) {
//                std::cout << "k:" << k << std::endl;
//            }
//
//            std::vector<std::vector<double>> newx(Hpin, std::vector<double>(Wpin));
//            std::vector<std::vector<double>> newy(Hpin, std::vector<double>(Wpin));
//
//            std::vector<std::vector<int>> nx(Hpin, std::vector<int>(Wpin));
//            std::vector<std::vector<int>> ny(Hpin, std::vector<int>(Wpin));
//
//            inv_d = 1.0 / (pointclouddata[k][2] * 1000 + D);
//
//            for (int i = 0; i < Hpin; i++) {
//                for (int j = 0; j < Wpin; j++) {
//
//                    //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//                    newx[i][j] = -(g * inv_d) * (uo[i][j] - pointclouddata[k][0] * 1000);
//                    newy[i][j] = -(g * inv_d) * (vo[i][j] - pointclouddata[k][1] * 1000);
//
//                    nx[i][j] = static_cast<int>(floor((newx[i][j] + 0.5 * wp) * inv_wp * N));
//                    ny[i][j] = static_cast<int>(floor((newy[i][j] + 0.5 * wp) * inv_wp * N));
//
//                    //std::cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << std::endl;
//                    //std::cout << "nx:" << nx[i][j] << ", ny:" << ny[i][j] << std::endl;
//
//                }
//            }
//
//            //std::cout << "calculating x, y was good" << std::endl;
//
//            insert_pixels(Wpin, N, k, 0, Hpin, std::ref(nx), std::ref(ny), std::ref(elem_img), std::ref(pointclouddata));
//
//            //const int numThreads = 17;
//            //std::vector<std::thread> threads;
//            //int rowsPerThread = Hpin / numThreads;
//
//            //for (int i = 0; i < numThreads; i++) {
//            //    int startRow = i * rowsPerThread;
//            //    int endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//            //    threads.emplace_back(insert_pixels, Wpin, N, k, startRow, endRow, std::ref(nx), std::ref(ny), std::ref(elem_img), std::ref(pointclouddata));
//            //}
//
//            //for (auto& t : threads) {
//            //    t.join();
//            //}
//            
//            //std::cout << "inserting pixels was good" << std::endl;
//
//        }
//
//        int startv, startu;
//        for (int i = 0; i < Hpin; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                startv = static_cast<int>(i * intv);
//                startu = static_cast<int>(j * intv);
//
//                for (int v = 0; v < N; v++) {
//                    for (int u = 0; u < N; u++) {
//
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[i][j].at<cv::Vec3b>(v, u)[0];
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[i][j].at<cv::Vec3b>(v, u)[1];
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[i][j].at<cv::Vec3b>(v, u)[2];
//                    }
//                }
//
//            }
//        }
//
//        // 計測終了時間
//        auto finish = std::chrono::high_resolution_clock::now();
//
//        // 経過時間を出力
//        std::chrono::duration<double> elapsed = finish - start;
//        std::cout << "Time to fill the array: " << elapsed.count() << " seconds" << std::endl;
//
//        imwrite("img_display.png", img_display);
//    }
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

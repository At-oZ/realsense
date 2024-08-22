//// License: Apache 2.0. See LICENSE file in root directory.
//// Copyright(c) 2015-2017 Intel Corporation. All Rights Reserved.
//
//#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
//#include <opencv2/opencv.hpp>
//#include <algorithm>            // min, max
//#include <vector>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <string>
//#include <chrono>
//#include <thread>
//#include <mutex>
//#include <condition_variable>
//#include <cmath>
//#include <time.h>
//
//using namespace std;
//using namespace cv;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//// 表示系のパラメータ
//int W = 170, H = 170;                                                                       // 表示の縦横幅
//double g = 4, wp = 1.25, a = 0.125;                                                          // ギャップ、ピッチ、ピンホール幅
//double ld = 13.4 * 25.4;                                                                    // ディスプレイサイズ
//int pw = 3840, ph = 2400;                                                                   // ディスプレイの縦横の解像度
//double pd = ld / sqrtf(pw * pw + ph * ph);                                                  // 画素ピッチ
//int N = static_cast<int>(floor(wp / pd));                                                   // 要素画像の解像度
//int pw_disp = static_cast<int>(floor(W / pd)), ph_disp = static_cast<int>(floor(H / pd));   // 表示画像の縦横の解像度
//int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp));         // 縦横のピンホール数
//double intv = wp / pd;                                                                      // 要素画像の間隔
//
//int WIDTH = 640;
//int HEIGHT = 480;
//int FPS = 30;
//
//// 点群データ配列の行数と列数
//int rows = WIDTH * HEIGHT;
//int cols = 6;
//
//double D = 100.0;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//// パラメータ
//int num_z_level = 40; // イメージプレーンの層数
//double ptimes = 1; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//int box_size = 1; // 探索範囲
//
//int Ddash = 250;
//double coef = (double)num_z_level * (double)Ddash;
//
//double img_pitch = ptimes * pd;
//
//int px_height_img = static_cast<int>(round(55 / ptimes));
//int px_width_img = static_cast<int>(round(55 / ptimes));
//
//void insert_pixels(int start, int end, Mat img_display, int*** pcd_box, double* sp, double* tp) {
//
//    int nx, ny, tmp_pcd_color, sum_zi, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//    int alpha = 256 * 256 * 256;
//    bool flag1 = false;
//    // bool flag2 = false;
//    int startu, startv;
//    double u, v, xt, yt, zt;
//
//    // 各要素画像の各画素ごとに
//    sum_zi = 0;
//    int sum_pcd_r = 0;
//    int sum_pcd_g = 0;
//    int sum_pcd_b = 0;
//    int num_pcd = 0;
//
//    for (int m = start; m < end; m++) {
//
//        v = ((double)m - (double)(N - 1) * 0.5) * pd;
//
//        for (int n = 0; n < N; n++) {
//
//            u = ((double)n - (double)(N - 1) * 0.5) * pd;
//
//            // 各要素カメラごとに
//            for (int i = 0; i < Hpin; i++) {
//
//                // 表示画像中における要素画像の左上画素の画素番号
//                startv = static_cast<int>(i * intv);
//
//                for (int j = 0; j < Wpin; j++) {
//
//                    // 表示画像中における要素画像の左上画素の画素番号
//                    startu = static_cast<int>(j * intv);
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int nz = num_z_level - 1; nz >= 0; nz--) {
//
//                        zt = (double)nz / coef;
//                        xt = sp[i * Wpin + j] * zt + u / g;
//                        yt = tp[i * Wpin + j] * zt + v / g;
//
//                        nx = static_cast<int>(floor((g / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                        ny = static_cast<int>(floor((g / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//
//                        //cout << "nx:" << nx << ", ny:" << ny << endl;
//                        if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img) {
//                            tmp_pcd_color = pcd_box[nz][ny][nx];
//
//                            if (tmp_pcd_color > 0) {
//                                //img_display[startv + m][startu + n] = tmp_pcd_color - alpha;
//                                //img_display.at<cv::Vec3b>(startv + m, startu + n)[2] = static_cast<int>(floor((tmp_pcd_color - alpha) / (256 * 256)));
//                                //img_display.at<cv::Vec3b>(startv + m, startu + n)[1] = static_cast<int>(floor((tmp_pcd_color - alpha) / 256 - img_display.at<cv::Vec3b>(startv + m, startu + n)[2] * 256));
//                                //img_display.at<cv::Vec3b>(startv + m, startu + n)[0] = static_cast<int>(floor((tmp_pcd_color - alpha) - (img_display.at<cv::Vec3b>(startv + m, startu + n)[2] * 256 - img_display.at<cv::Vec3b>(startv + m, startu + n)[1]) * 256));
//
//                                img_display.at<cv::Vec3b>(startv + m + 40, startu + n + 790)[2] = static_cast<int>(floor((tmp_pcd_color - alpha) / (256 * 256)));
//                                img_display.at<cv::Vec3b>(startv + m + 40, startu + n + 790)[1] = static_cast<int>(floor((tmp_pcd_color - alpha) / 256 - img_display.at<cv::Vec3b>(startv + m, startu + n)[2] * 256));
//                                img_display.at<cv::Vec3b>(startv + m + 40, startu + n + 790)[0] = static_cast<int>(floor((tmp_pcd_color - alpha) - (img_display.at<cv::Vec3b>(startv + m, startu + n)[2] * 256 - img_display.at<cv::Vec3b>(startv + m, startu + n)[1]) * 256));
//
//                                // cout << "v, u:" << startv + m << ", " << startu + n << endl;
//                                break;
//                            }
//                            //else {
//                            //    for (int s = 1; s < static_cast<int>((box_size - 1) / 2); s++) {
//
//                            //        // 上辺
//                            //        if (0 <= ny - s && ny - s < px_height_img) {
//                            //            for (int k = 0; k < box_size; k++) {
//                            //                if (0 <= nx - s + k && nx - s + k < px_width_img) {
//                            //                    if (pcd_box[nz][ny - s][nx - s + k] > 0) {
//                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny - s][nx - s + k] - alpha;
//                            //                        flag1 = true;
//                            //                    }
//                            //                }
//                            //            }
//                            //        }
//
//                            //        if (flag1) break;
//
//                            //        // 右辺
//                            //        if (0 <= nx + s && nx + s < px_width_img) {
//                            //            for (int k = 1; k < box_size; k++) {
//                            //                if (0 <= ny - s + k && ny - s + k < px_height_img) {
//                            //                    if (pcd_box[nz][ny - s + k][nx + s] > 0) {
//                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny - s + k][nx + s] - alpha;
//                            //                        flag1 = true;
//                            //                    }
//                            //                }
//                            //            }
//                            //        }
//
//                            //        if (flag1) break;
//
//                            //        //下辺
//                            //        if (0 <= ny + s && ny + s < px_height_img) {
//                            //            for (int k = 1; k < box_size; k++) {
//                            //                if (0 <= nx + s - k && nx + s - k < px_width_img) {
//                            //                    if (pcd_box[nz][ny + s][nx + s - k] > 0) {
//                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny + s][nx + s - k] - alpha;
//                            //                        flag1 = true;
//                            //                    }
//                            //                }
//                            //            }
//                            //        }
//
//                            //        if (flag1) break;
//
//                            //        // 左辺
//                            //        if (0 <= nx - s && nx - s < px_width_img) {
//                            //            for (int k = 1; k < box_size - 1; k++) {
//                            //                if (0 <= ny + s - k && ny + s - k < px_height_img) {
//                            //                    if (pcd_box[nz][ny + s - k][nx - s] > 0) {
//                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny + s - k][nx - s] - alpha;
//                            //                        flag1 = true;
//                            //                    }
//                            //                }
//                            //            }
//                            //        }
//
//                            //        if (flag1) break;
//                            //    }
//
//
//                            //    if (flag1) {
//                            //        flag1 = false;
//                            //        break;
//                            //    }
//
//                            //}
//
//                        }
//
//                        //xt -= tmp_sp * coef;
//                        //yt -= tmp_tp * coef;
//                        //zt -= coef;
//
//
//                    }
//
//                }
//            }
//        }
//    }
//
//    // sum_zi = sum_zi / (2 * 16 * 136 * 136);
//    // cout << "average of zi:" << sum_zi << endl;
//
//    // スレッドの終了をカウントし、条件変数を通知する
//    {
//        std::lock_guard<std::mutex> lock(mtx);
//        finished_threads++;
//        conv.notify_one(); // 1つの待機スレッドに通知
//    }
//}
//
//
//int main(int argc, char* argv[]) try
//{
//
//    cout << "vt-v11-6-trial" << endl;
//
//    // 視点パラメータ(mm)
//    double* sp;
//    double* tp;
//    sp = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    tp = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    for (int i = 0; i < Hpin; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            sp[i * Wpin + j] = (j - (Wpin - 1) * 0.5) * wp;
//            tp[i * Wpin + j] = (i - (Hpin - 1) * 0.5) * wp;
//
//            //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//        }
//    }
//
//    int*** pcd_box;
//    double*** val_z;
//    pcd_box = (int***)malloc(sizeof(int**) * num_z_level);
//    val_z = (double***)malloc(sizeof(double**) * num_z_level);
//    for (int i = 0; i < num_z_level; i++) {
//        pcd_box[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
//        for (int j = 0; j < px_height_img; j++) {
//            pcd_box[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
//        }
//    }
//
//    //cv::Mat img_display = Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    cv::Mat img_display = Mat::zeros(cv::Size(pw, ph), CV_8UC3);
//
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
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
//    while (true) {
//        //std::cout << tt << std::endl;
//
//        //// 測定開始時刻を記録
//        //auto start = std::chrono::high_resolution_clock::now();
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
//        //// 画像を表示
//        //cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        //cv::imshow("Display window", color_image);
//
//        //// 任意のキーが押されるまで待つ
//        //cv::waitKey(0);
//
//        //std::cout << "before pc calc" << std::endl;
//
//        // Generate the pointcloud and texture mappings
//        points = pc.calculate(depth_frame);
//
//        // Tell pointcloud object to map to this color frame
//        pc.map_to(color_frame);
//
//        auto verts = points.get_vertices();
//
//        //std::cout << "before data" << std::endl;
//
//        //std::vector<std::vector<double>> data(WIDTH * HEIGHT, std::vector<double>(6));
//
//        double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double xt, yt, zt;
//        int nx, ny, nz;
//        //int pcd_count = 0;
//
//        img_display = cv::Scalar::all(0);
//        //for (int i = 0; i < ph_disp; i++) {
//        //    for (int j = 0; j < pw_disp; j++) {
//        //        img_display.at<cv::Vec3b>(i, j)[0] = 0;
//        //        img_display.at<cv::Vec3b>(i, j)[1] = 0;
//        //        img_display.at<cv::Vec3b>(i, j)[2] = 0;
//        //    }
//        //}
//        for (int i = 0; i < num_z_level; i++) {
//            for (int j = 0; j < px_height_img; j++) {
//                for (int k = 0; k < px_width_img; k++) {
//                    pcd_box[i][j][k] = 0;
//                    val_z[i][j][k] = 0;
//                }
//            }
//        }
//
//        for (int i = 0; i < rows; i++) {
//
//            //std::cout << "x:" << i % WIDTH << ", y:" << static_cast<int>(i * inv_WIDTH) << std::endl;
//            tmp_pcd_x = verts[i].x * 1000.0;
//            tmp_pcd_y = verts[i].y * 1000.0;
//            tmp_pcd_z = verts[i].z * 1000.0 + D;
//            tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[2];
//            tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[1];
//            tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[0];
//
//            zt = 1.0 / tmp_pcd_z;
//            xt = tmp_pcd_x * zt;
//            yt = tmp_pcd_y * zt;
//
//            nx = static_cast<int>(floor((g / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//            ny = static_cast<int>(floor((g / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//            nz = static_cast<int>(floor(coef * zt + 0.5));
//
//            //cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;
//
//            if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img && 0 <= nz && nz < num_z_level) {
//                //if (pcd_box[nz][ny][nx] > 0) {
//                //    pcd_count++;
//                //}
//
//                if (val_z[nz][ny][nx] < zt) {
//                    pcd_box[nz][ny][nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256 + 256 * 256 * 256;
//                    val_z[nz][ny][nx] = zt;
//                }
//                //pcd_box[nz][ny][nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256 + 256 * 256 * 256;
//
//                // cout << "pcd_box:" << pcd_box[nz][ny][nx] << endl;
//            }
//        }
//
//        //std::cout << "before calculation" << std::endl;
//
//        //insert_pixels(0, N, std::ref(img_display), std::ref(pcd_box), std::ref(sp), std::ref(tp));
//
//        const int numThreads = 16;
//        vector<thread> threads;
//        int rowsPerThread = N / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(pcd_box), std::ref(sp), std::ref(tp));
//        }
//
//        for (auto& t : threads) {
//            if (t.joinable()) { t.join(); }
//        }
//
//        // 全てのスレッドが終了するのを待つ
//        {
//            std::unique_lock<std::mutex> lock(mtx);
//            conv.wait(lock, [] {return finished_threads == numThreads; });
//        }
//
//        //cout << "calc finished" << endl;
//        finished_threads = 0;
//
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        //// 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        //cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        cv::imshow("images", img_display);
//        
//        if (cv::waitKey(10) == 27)  // ESCキーで終了
//        {
//            cv::destroyAllWindows;
//            break;
//        }
//
//    }
//    pipe.stop();
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

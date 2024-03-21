////// License: Apache 2.0. See LICENSE file in root directory.
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
//
//void insert_pixels(int Hpin, int Wpin, int N, double D, double g, double wp, double inv_wp, int starty, int endy, int pcd_size, double* uo, double* vo, int* elem_img, double* pointclouddata) {
//
//    double g_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//    std::cout << "pcd_size:" << pcd_size << "\n" << std::endl;
//    for (int k = 0; k < pcd_size; k++) {
//
//        tmp_pcd0 = pointclouddata[static_cast<int>(floor(k / 6)) * 6] * 1000;
//        tmp_pcd1 = pointclouddata[static_cast<int>(floor(k / 6)) * 6 + 1] * 1000;
//        tmp_pcd2 = pointclouddata[static_cast<int>(floor(k / 6)) * 6 + 2] * 1000;
//        tmp_pcd3 = pointclouddata[static_cast<int>(floor(k / 6)) * 6 + 3];
//        tmp_pcd4 = pointclouddata[static_cast<int>(floor(k / 6)) * 6 + 4];
//        tmp_pcd5 = pointclouddata[static_cast<int>(floor(k / 6)) * 6 + 5];
//
//        if (k % 10000 == 0) {
//            std::cout << "k:" << k << "\n" << std::endl;
//        }
//
//        g_d = g / (tmp_pcd2 + D);
//        double newx, newy;
//        int nx, ny;
//        for (int i = starty; i < endy; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//                newx = -g_d * (uo[i * Wpin + j] - tmp_pcd0 * 1000);
//                newy = -g_d * (vo[i * Wpin + j] - tmp_pcd1 * 1000);
//
//                nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//                ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//                //std::cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << std::endl;
//                //std::cout << "nx:" << nx << ", ny:" << ny << std::endl;
//
//                if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                    if (elem_img[(((i * Wpin + j) * N + ny) * N + nx) * 3] == 0 && elem_img[(((i * Wpin + j) * N + ny) * N + nx) * 3 + 1] == 0 && elem_img[(((i * Wpin + j) * N + ny) * N + nx) * 3 + 2] == 0) {
//
//                        elem_img[(((i * Wpin + j) * N + ny) * N + nx) * 3] = static_cast<int>(tmp_pcd3);
//                        elem_img[(((i * Wpin + j) * N + ny) * N + nx) * 3 + 1] = static_cast<int>(tmp_pcd4);
//                        elem_img[(((i * Wpin + j) * N + ny) * N + nx) * 3 + 2] = static_cast<int>(tmp_pcd5);
//
//                    }
//                }
//            }
//        }
//
//        //double newx, newy;
//        //int nx, ny;
//        //for (int i = starty * Wpin; i < endy * Wpin; i++) {
//
//        //    //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //    newx = -g_d * (uo[i] - tmp_pcd0);
//        //    newy = -g_d * (vo[i] - tmp_pcd1);
//
//        //    nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//        //    ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//        //    //std::cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << std::endl;
//        //    //std::cout << "nx:" << nx << ", ny:" << ny << std::endl;
//
//        //    if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//        //        if (elem_img[((i * N + ny) * N + nx) * 3] == 0 && elem_img[((i * N + ny) * N + nx) * 3 + 1] == 0 && elem_img[((i * N + ny) * N + nx) * 3 + 2] == 0) {
//
//        //            elem_img[((i * N + ny) * N + nx) * 3] = static_cast<int>(tmp_pcd3);
//        //            elem_img[((i * N + ny) * N + nx) * 3 + 1] = static_cast<int>(tmp_pcd4);
//        //            elem_img[((i * N + ny) * N + nx) * 3 + 2] = static_cast<int>(tmp_pcd5);
//
//        //        }
//        //    }
//        //}
//
//        //for (int i = starty * Wpin; i < endy * Wpin; i++) {
//        //    //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //    newx = -(g * inv_d) * (uo[i] - tmp_pcd0 * 1000);
//        //    newy = -(g * inv_d) * (vo[i] - tmp_pcd1 * 1000);
//
//        //    nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//        //    ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//        //    //std::cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << std::endl;
//        //    //std::cout << "nx:" << nx << ", ny:" << ny << std::endl;
//
//        //    if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//        //        if (elem_img[((i * N + nx) * N + ny) * 3] == 0 && elem_img[((i * N + nx) * N + ny) * 3 + 1] == 0 && elem_img[((i * N + nx) * N + ny) * 3 + 2] == 0) {
//
//        //            elem_img[((i * N + nx) * N + ny) * 3] = static_cast<int>(tmp_pcd3);
//        //            elem_img[((i * N + nx) * N + ny) * 3 + 1] = static_cast<int>(tmp_pcd4);
//        //            elem_img[((i * N + nx) * N + ny) * 3 + 2] = static_cast<int>(tmp_pcd5);
//
//        //        }
//        //    }
//        //}
//    }
//}
//
//int main(int argc, char* argv[]) try
//{
//
//    // 繰り返し変数
//    int u, v, i, j, t;
//
//    // 表示系のパラメータ(mm)
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
//    std::cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << std::endl;
//
//    // 要素画像群
//    int* elem_img;
//    elem_img = (int*)malloc(sizeof(int) * Hpin * Wpin * N * N * 3);
//
//    for (i = 0; i < Hpin * Wpin * N * N * 3; i++) {
//        elem_img[i] = 0;
//    }
//
//    // 視点パラメータ(mm)
//    double* uo;
//    double* vo;
//
//    uo = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    vo = (double*)malloc(sizeof(double) * Hpin * Wpin);
//
//    for (int k = 0; k < Hpin * Wpin; k++) {
//
//        j = k % Wpin;
//        i = static_cast<int>(floor(k / Wpin));
//        uo[k] = (j - static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp;
//        vo[k] = (i - static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp;
//
//        //std::cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << std::endl;
//    }
//
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
//
//    double D = 100;                     // 表示系とカメラとの距離(mm)
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
//    colorSensors.set_option(RS2_OPTION_EXPOSURE, 200);
//    colorSensors.set_option(RS2_OPTION_GAIN, 64);
//    irSensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
//    irSensor.set_option(RS2_OPTION_LASER_POWER, 360);
//
//    //rs2::colorizer color_map;
//    rs2::align align(RS2_STREAM_COLOR);
//
//    for (i = 0; i < 3; i++)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        cv::waitKey(10);
//    }
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
//        cv::Mat color_image_rgb(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
//        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        cv::Mat color_image;
//        cv::cvtColor(color_image_rgb, color_image, cv::COLOR_RGB2BGR);
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
//        cv::Mat img_display(cv::Size(pw_disp, ph_disp), CV_8UC3);
//        //int* img_display;
//        //img_display = (int*)malloc(sizeof(int) * pw_disp * ph_disp * 3);
//
//        std::cout << "before raw_pointclouddata" << std::endl;
//
//        //std::vector<std::vector<double>> raw_pointclouddata(WIDTH * HEIGHT, std::vector<double>(6));
//        double* raw_pointclouddata;
//        int pcd_size = WIDTH * HEIGHT * 6;
//        raw_pointclouddata = (double*)malloc(sizeof(double) * pcd_size);
//
//        double vtx_x, vtx_y, vtx_z;
//        for (i = 0; i < WIDTH * HEIGHT; i++) {
//
//            vtx_x = verts[i].x;
//            vtx_y = verts[i].y;
//            vtx_z = verts[i].z;
//
//            raw_pointclouddata[i * 6] = vtx_x;
//            raw_pointclouddata[i * 6 + 1] = vtx_y;
//            raw_pointclouddata[i * 6 + 2] = vtx_z;
//            raw_pointclouddata[i * 6 + 3] = color_image.at<cv::Vec3b>(static_cast<int>(static_cast<int>(floor(i / 6)) * inv_WIDTH), (static_cast<int>(floor(i / 6)) % WIDTH))[0];
//            raw_pointclouddata[i * 6 + 4] = color_image.at<cv::Vec3b>(static_cast<int>(static_cast<int>(floor(i / 6)) * inv_WIDTH), (static_cast<int>(floor(i / 6)) % WIDTH))[1];
//            raw_pointclouddata[i * 6 + 5] = color_image.at<cv::Vec3b>(static_cast<int>(static_cast<int>(floor(i / 6)) * inv_WIDTH), (static_cast<int>(floor(i / 6)) % WIDTH))[2];
//
//        }
//
//        //double vtx_x, vtx_y, vtx_z;
//        //for (i = 0; i < WIDTH * HEIGHT * 6; i++) {
//
//        //    vtx_x = verts[static_cast<int>(floor(i / 6))].x;
//        //    vtx_y = verts[static_cast<int>(floor(i / 6))].y;
//        //    vtx_z = verts[static_cast<int>(floor(i / 6))].z;
//
//        //    j = i % 6;
//        //    if (j == 0) {
//        //        raw_pointclouddata[i] = vtx_x;
//        //    }
//        //    else if (j == 1) {
//        //        raw_pointclouddata[i] = vtx_y;
//        //    }
//        //    else if (j == 2) {
//        //        raw_pointclouddata[i] = vtx_z;
//        //    }
//        //    else if (j == 3) {
//        //        raw_pointclouddata[i] = color_image.at<cv::Vec3b>(static_cast<int>(static_cast<int>(floor(i / 6)) * inv_WIDTH), (static_cast<int>(floor(i / 6)) % WIDTH))[0];
//        //    }
//        //    else if (j == 4) {
//        //        raw_pointclouddata[i] = color_image.at<cv::Vec3b>(static_cast<int>(static_cast<int>(floor(i / 6)) * inv_WIDTH), (static_cast<int>(floor(i / 6)) % WIDTH))[1];
//        //    }
//        //    else {
//        //        raw_pointclouddata[i] = color_image.at<cv::Vec3b>(static_cast<int>(static_cast<int>(floor(i / 6)) * inv_WIDTH), (static_cast<int>(floor(i / 6)) % WIDTH))[2];
//        //    }
//        //}
//        std::cout << "raw_pcd_size:" << sizeof(raw_pointclouddata) / sizeof(double) << std::endl;
//
//        std::cout << "before sort raw_pointclouddata" << std::endl;
//
//        //// カスタム比較関数
//        //auto compare = [](const double& a, const double& b) {
//        //    return a < b;
//        //};
//
//        //// 特定の列に基づいて全体を並べ替え
//        //std::sort(raw_pointclouddata.begin(), raw_pointclouddata.end(), compare);
//
//        //std::cout << "before create pointclouddata" << std::endl;
//
//        //// 条件に一致する行を削除する新たな二次元配列Bを作成
//        //std::vector<std::vector<double>> pointclouddata;
//
//        //// 2列目が0でない行だけをBにコピー
//        //std::copy_if(raw_pointclouddata.begin(), raw_pointclouddata.end(), std::back_inserter(pointclouddata), [](const std::vector<double>& row) {
//        //    return row[2] > 0; // zが0ではない行を選択
//        //    });
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
//        //insert_pixels(Hpin, Wpin, N, D, g, wp, inv_wp, 0, Hpin, uo, vo, elem_img, raw_pointclouddata);
//
//        const int numThreads = 17;
//        std::vector<std::thread> threads;
//        int rowsPerThread = Hpin / numThreads;
//
//        int startRow, endRow;
//        for (i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, Hpin, Wpin, N, D, g, wp, inv_wp, startRow, endRow, pcd_size, uo, vo, elem_img, raw_pointclouddata);
//        }
//
//        for (auto& t : threads) {
//            t.join();
//        }
//
//        int startv, startu;
//        for (int i = 0; i < Hpin; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//                startv = static_cast<int>(i * intv);
//                startu = static_cast<int>(j * intv);
//
//                for (int v = 0; v < N; v++) {
//                    for (int u = 0; u < N; u++) {
//
//                        //std::cout << "u:" << u << ", v:" << v << std::endl;
//
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[(((i * Wpin + j) * N + v) * N + u) * 3];
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[(((i * Wpin + j) * N + v) * N + u) * 3 + 1];
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[(((i * Wpin + j) * N + v) * N + u) * 3 + 2];
//                    }
//                }
//            }
//        }
//
//        //int startv, startu;
//        //for (int k = 0; k < Hpin * Wpin * N * N; k++) {
//
//        //    t = static_cast<int>(floor(k / (N * N)));
//        //    j = t % Wpin;
//        //    i = static_cast<int>(floor(t / Wpin));
//        //    //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //    startv = static_cast<int>(i * intv);
//        //    startu = static_cast<int>(j * intv);
//
//        //    t = k % (N * N);
//        //    u = t % N;
//        //    v = static_cast<int>(floor(t / N));
//
//        //    //std::cout << "u:" << u << ", v:" << v << std::endl;
//
//        //    img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[((i * Wpin + j) * N + v) * N + u];
//        //    img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[((i * Wpin + j) * N + v) * N + u + 1];
//        //    img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[((i * Wpin + j) * N + v) * N + u + 2];
//        //}
//
//        //int startv, startu, u, j;
//        //for (int i = 0; i < Hpin * Wpin; i++) {
//
//        //    j = i % Wpin;
//        //    i = static_cast<int>(floor(i / Wpin));
//        //    //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //    startv = static_cast<int>(i * intv);
//        //    startu = static_cast<int>(j * intv);
//
//        //    for (int v = 0; v < N * N; v++) {
//
//        //        u = v % N;
//        //        v = static_cast<int>(floor(v / N));
//
//        //        //std::cout << "u:" << u << ", v:" << v << std::endl;
//
//        //        img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[((i * Wpin + j) * N + v) * N + u];
//        //        img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[((i * Wpin + j) * N + v) * N + u + 1];
//        //        img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[((i * Wpin + j) * N + v) * N + u + 2];
//        //    }
//        //}
//
//        //int startv, startu, u;
//        //for (int i = 0; i < Hpin; i++) {
//        //    for (int j = 0; j < Wpin; j++) {
//
//        //        //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //        startv = static_cast<int>(i * intv);
//        //        startu = static_cast<int>(j * intv);
//
//        //        for (int v = 0; v < N * N; v++) {
//
//        //            u = v % N;
//        //            v = static_cast<int>(floor(v / N));
//
//        //            //std::cout << "u:" << u << ", v:" << v << std::endl;
//
//        //            img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[((i * Wpin + j) * N + v) * N + u];
//        //            img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[((i * Wpin + j) * N + v) * N + u + 1];
//        //            img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[((i * Wpin + j) * N + v) * N + u + 2];
//        //        }
//        //    }
//        //}
//
//        //int startv, startu;
//        //for (int i = 0; i < Hpin; i++) {
//        //    for (int j = 0; j < Wpin; j++) {
//
//        //        //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //        startv = static_cast<int>(i * intv);
//        //        startu = static_cast<int>(j * intv);
//
//        //        for (int v = 0; v < N; v++) {
//        //            for (int u = 0; u < N; u++) {
//
//        //                //std::cout << "u:" << u << ", v:" << v << std::endl;
//
//        //                img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[((i * Wpin + j) * N + v) * N + u];
//        //                img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[((i * Wpin + j) * N + v) * N + u + 1];
//        //                img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[((i * Wpin + j) * N + v) * N + u + 2];
//        //            }
//        //        }
//        //    }
//        //}
//
//        //int startv, startu, v, u;
//        //for (int i = 0; i < Hpin * Wpin * N * N; i++) {
//
//        //        //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//        //        startv = static_cast<int>(static_cast<int>(floor(i / (Wpin * N * N))) * intv);
//        //        startu = static_cast<int>((static_cast<int>(floor(i / (N * N))) % Wpin) * intv);
//
//        //        v = static_cast<int>(floor(i / (N * Hpin * Wpin)));
//        //        u = static_cast<int>(floor(v / (Hpin * Wpin))) % N;
//        //        //std::cout << "u:" << u << ", v:" << v << std::endl;
//
//        //        //img_display[(startv + v) * N + (startu + u) * 3] = elem_img[((i * N + v) * N + u) * 3];
//        //        //img_display[(startv + v) * N + (startu + u) * 3 + 1] = elem_img[((i * N + v) * N + u) * 3 + 1];
//        //        //img_display[(startv + v) * N + (startu + u) * 3 + 2] = elem_img[((i * N + v) * N + u) * 3 + 2];
//
//        //        img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[((i * N + v) * N + u) * 3];
//        //        img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[((i * N + v) * N + u) * 3 + 1];
//        //        img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[((i * N + v) * N + u) * 3 + 2];
//
//        //}
//
//        //for (int i = 1; i < ph_disp - 1; i++) {
//        //    for (int j = 1; j < pw_disp - 1; j++) {
//        //        if (img_display.at<cv::Vec3b>(i, j)[0] < 10 && img_display.at<cv::Vec3b>(i, j)[1] < 10 && img_display.at<cv::Vec3b>(i, j)[2] < 10) {
//
//        //            img_display.at<cv::Vec3b>(i, j)[0]
//        //                = static_cast<int>((img_display.at<cv::Vec3b>(i - 1, j - 1)[0] + img_display.at<cv::Vec3b>(i - 1, j)[0]
//        //                    + img_display.at<cv::Vec3b>(i - 1, j + 1)[0] + img_display.at<cv::Vec3b>(i, j - 1)[0]
//        //                    + img_display.at<cv::Vec3b>(i, j + 1)[0] + img_display.at<cv::Vec3b>(i + 1, j - 1)[0]
//        //                    + img_display.at<cv::Vec3b>(i + 1, j)[0] + img_display.at<cv::Vec3b>(i + 1, j + 1)[0]) / 8);
//
//        //            img_display.at<cv::Vec3b>(i, j)[1]
//        //                = static_cast<int>((img_display.at<cv::Vec3b>(i - 1, j - 1)[1] + img_display.at<cv::Vec3b>(i - 1, j)[1]
//        //                    + img_display.at<cv::Vec3b>(i - 1, j + 1)[1] + img_display.at<cv::Vec3b>(i, j - 1)[1]
//        //                    + img_display.at<cv::Vec3b>(i, j + 1)[1] + img_display.at<cv::Vec3b>(i + 1, j - 1)[1]
//        //                    + img_display.at<cv::Vec3b>(i + 1, j)[1] + img_display.at<cv::Vec3b>(i + 1, j + 1)[1]) / 8);
//
//        //            img_display.at<cv::Vec3b>(i, j)[2]
//        //                = static_cast<int>((img_display.at<cv::Vec3b>(i - 1, j - 1)[2] + img_display.at<cv::Vec3b>(i - 1, j)[2]
//        //                    + img_display.at<cv::Vec3b>(i - 1, j + 1)[2] + img_display.at<cv::Vec3b>(i, j - 1)[2]
//        //                    + img_display.at<cv::Vec3b>(i, j + 1)[2] + img_display.at<cv::Vec3b>(i + 1, j - 1)[2]
//        //                    + img_display.at<cv::Vec3b>(i + 1, j)[2] + img_display.at<cv::Vec3b>(i + 1, j + 1)[2]) / 8);
//
//        //            //std::cout << "img_display 0:" << img_display.at<cv::Vec3b>(i, j)[0] << ", 1:" << img_display.at<cv::Vec3b>(i, j)[1] << ", 2:" << img_display.at<cv::Vec3b>(i, j)[2] << std::endl;
//        //        }
//        //    }
//        //}
//
//        // 計測終了時間
//        auto finish = std::chrono::high_resolution_clock::now();
//
//        // 経過時間を出力
//        std::chrono::duration<double> elapsed = finish - start;
//        std::cout << "Time to fill the array: " << elapsed.count() << " seconds" << std::endl;
//
//        std::ostringstream stream;
//        stream << "v7_img_display_g" << g << "_wp" << std::fixed << std::setprecision(1) << wp << "_pd" << std::fixed << std::setprecision(3) << pd << "_D" << D << ".png"; // 小数点以下2桁で切り捨て
//        cv::String filename = stream.str();
//
//        imwrite(filename, img_display);
//
//        free(elem_img);
//        free(uo);
//        free(vo);
//        free(raw_pointclouddata);
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

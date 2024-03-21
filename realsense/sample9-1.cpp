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
//#include <chrono>
//#include <thread>
//#include <cmath>
//#include <time.h>
//
//using namespace std;
//
//void insert_pixels(int WIDTH, int HEIGHT, int Wpin, int N, int D, double g, double wp, double inv_wp, double intv, int starty, int endy, double* uo, double* vo, double* pointclouddata, rs2::points points, cv::Mat img_display, double* val_z) {
//
//    double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//    double newx, newy;
//    int nx, ny;
//    int startv, startu;
//    int pcd_size = WIDTH * HEIGHT;
//    int inv_WIDTH = 1.0 / WIDTH;
//    auto verts = points.get_vertices();
//    for (int k = 0; k < pcd_size; k++) {
//
//        tmp_pcd0 = verts[k].x;
//        tmp_pcd1 = verts[k].y;
//        tmp_pcd2 = verts[k].z;
//        tmp_pcd3 = pointclouddata[k * 3];
//        tmp_pcd4 = pointclouddata[k * 3 + 1];
//        tmp_pcd5 = pointclouddata[k * 3 + 2];
//
//        //cout << "pcd0:" << tmp_pcd0 << ", pcd1:" << tmp_pcd1 << ", pcd2:" << tmp_pcd2 << ", pcd3:" << tmp_pcd3 << ", pcd4:" << tmp_pcd4 << ", pcd5:" << tmp_pcd5 << endl;
//        if (k % 1000 == 0) {
//            cout << "k:" << k << "\n" << endl;
//        }
//
//        //vector<vector<double>> newx(endy - starty, vector<double>(Wpin));
//        //vector<vector<double>> newy(endy - starty, vector<double>(Wpin));
//
//        //vector<vector<int>> nx(endy - starty, vector<int>(Wpin));
//        //vector<vector<int>> ny(endy - starty, vector<int>(Wpin));
//
//        inv_d = 1.0 / (tmp_pcd2 * 1000 + D);
//
//        for (int i = starty; i < endy; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                //cout << "i:" << i << ", j:" << j << endl;
//
//                newx = -(g * inv_d) * (uo[i * Wpin + j] - tmp_pcd0 * 1000);
//                newy = -(g * inv_d) * (vo[i * Wpin + j] - tmp_pcd1 * 1000);
//
//                nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//                ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//                startv = static_cast<int>(i * intv);
//                startu = static_cast<int>(j * intv);
//
//                //cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << endl;
//                //cout << "nx:" << nx << ", ny:" << ny << endl;
//
//                if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                    if (val_z[((i * Wpin + j) * N + ny) * N + nx] > tmp_pcd2) {
//
//                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[0] = static_cast<int>(tmp_pcd3);
//                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[1] = static_cast<int>(tmp_pcd4);
//                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[2] = static_cast<int>(tmp_pcd5);
//                        val_z[((i * Wpin + j) * N + ny) * N + nx] = tmp_pcd2;
//                    }
//                }
//            }
//        }
//    }
//}
//
//// パーティション関数
//int partition(double* A, int low, int high, int m, int l) {
//    //double pivot = A[high * m + l];    // 基準点を配列の最後に設定
//    int i = low - 1;
//    double tmp;
//    //cout << "pivot:" << pivot << endl;
//    for (int j = low; j <= high - 1; j++) {
//        //cout << "j:" << j << endl;
//        // もし現在の要素がピボットより小さい場合、iをインクリメントし、その要素をiの位置に交換
//        if (A[j * m + l] < A[high * m + l]) {
//            i++;
//            for (int k = 0; k < m; k++) {
//                tmp = A[i * m + k];
//                A[i * m + k] = A[j * m + k];
//                A[j * m + k] = tmp;
//                //cout << "tmp:" << tmp << endl;
//            }
//        }
//    }
//    // ピボットを正しい位置に置く
//    for (int k = 0; k < m; k++) {
//        tmp = A[(i + 1) * m + k];
//        A[high * m + k] = A[(i + 1) * m + k];
//        A[(i + 1) * m + k] = tmp;
//    }
//    return (i + 1);
//}
//
//// クイックソート関数
//void quickSort(double* A, int low, int high, int m, int l) {
//    //t++;
//    //cout << "t:" << t << endl;
//    if (low < high) {
//        // パーティションインデックスを取得する。A[p] は現在正しい場所にあります
//        int pi = partition(A, low, high, m, l);
//
//        // 別々に前半部と後半部をソートする
//        quickSort(A, low, pi - 1, m, l);
//        quickSort(A, pi + 1, high, m, l);
//    }
//}
//
//int main(int argc, char* argv[]) try
//{
//
//    double start, finish;
//    clock_t cpu_time;
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
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//
//    // 要素画像群
//    //vector<vector<cv::Mat>> elem_img(Hpin, vector<cv::Mat>(Wpin));
//    int* elem_img;
//    elem_img = (int*)calloc(Hpin * Wpin * N * N * 3, sizeof(int));
//
//    // 視点パラメータ(mm)
//    //vector<vector<double>> uo(Hpin, vector<double>(Wpin));
//    //vector<vector<double>> vo(Hpin, vector<double>(Wpin));
//    double* uo;
//    double* vo;
//    uo = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    vo = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    for (int i = 0; i < Hpin; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            uo[i * Wpin + j] = (j - static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp;
//            vo[i * Wpin + j] = (i - static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp;
//
//            //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//        }
//    }
//
//    double* val_z;
//    val_z = (double*)malloc(sizeof(double) * Hpin * Wpin * N * N);
//
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
//
//    int* color_img_rgb;
//    color_img_rgb = (int*)malloc(sizeof(int) * HEIGHT * WIDTH * 3);
//
//
//    int D = 100;                     // 表示系とカメラとの距離(mm)
//    double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//    double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//    //cout << "inv_WIDTH" << inv_WIDTH << endl;
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
//    cout << colorSensors.get_info(RS2_CAMERA_INFO_NAME) << endl;
//    cout << irSensor.get_info(RS2_CAMERA_INFO_NAME) << endl;
//    colorSensors.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
//    colorSensors.set_option(RS2_OPTION_EXPOSURE, 300);
//    colorSensors.set_option(RS2_OPTION_GAIN, 64);
//    irSensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
//    irSensor.set_option(RS2_OPTION_LASER_POWER, 360);
//
//    //rs2::colorizer color_map;
//    rs2::align align(RS2_STREAM_COLOR);
//
//    rs2::hole_filling_filter hole_filling_filter;
//    hole_filling_filter.set_option(RS2_OPTION_HOLES_FILL, 1);
//
//    for (int i = 0; i < 3; i++)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        cv::waitKey(10);
//    }
//
//    for (int tt = 0; tt < 1; tt++) {
//
//        // 計測開始時間
//        cpu_time = clock();
//        start = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//        //cout << tt << endl;
//
//        // Wait for the next set of frames from the camera
//        auto frames = pipe.wait_for_frames();
//        auto aligned_frames = align.process(frames);
//
//        //rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
//        rs2::video_frame color_frame = aligned_frames.get_color_frame();
//        rs2::video_frame depth_frame = aligned_frames.get_depth_frame();
//        depth_frame = hole_filling_filter.process(depth_frame);
//
//        cv::Mat color_image_rgb(cv::Size(WIDTH, HEIGHT), CV_8UC3);
//        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        const uint8_t* color_data = reinterpret_cast<const uint8_t*>(color_frame.get_data());
//
//        for (int i = 0; i < HEIGHT; i++) {
//            for (int j = 0; j < WIDTH; j++) {
//                color_image_rgb.at<cv::Vec3b>(i, j)[0] = color_data[(i * WIDTH + j) * 3 + 2];
//                color_image_rgb.at<cv::Vec3b>(i, j)[1] = color_data[(i * WIDTH + j) * 3 + 1];
//                color_image_rgb.at<cv::Vec3b>(i, j)[2] = color_data[(i * WIDTH + j) * 3];
//            }
//        }
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
//        // 計測終了時間
//        cpu_time = clock();
//        finish = (double)cpu_time / (double)CLOCKS_PER_SEC;
//
//        // 経過時間を出力
//        cout << "Time to fill the array: " << finish - start << " seconds" << endl;
//
//    }
//
//    return EXIT_SUCCESS;
//}
//catch (const rs2::error& e)
//{
//    cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << endl;
//    return EXIT_FAILURE;
//}
//catch (const exception& e)
//{
//    cerr << e.what() << endl;
//    return EXIT_FAILURE;
//}

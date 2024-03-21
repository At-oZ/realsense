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
//void insert_pixels(int WIDTH, int HEIGHT, int Hpin, int Wpin, int N, int Nz, int D, double max_z, double min_z, double u_min, double v_min, double g, double wp, double pd, double inv_wp, double intv, int starty, int endy, double* uo, double* vo, double**** pcd_space, rs2::points points, cv::Mat img_display) {
//
//    double newx, newy, x_z, y_z, z;
//    int nx, ny, u, v;
//    int startv, startu;
//    double block_size;
//    double inv_pc_intv = 1.0 / 0.00165;
//    double pd_g = pd / g;
//    for (int i = 0; i < Hpin; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            startv = static_cast<int>(i * intv);
//            startu = static_cast<int>(j * intv);
//
//            for (double y = -0.5 * (wp - pd); y <= 0.5 * (wp - pd); y += pd) {
//                for (double x = -0.5 * (wp - pd); x <= 0.5 * (wp - pd); x += pd) {
//
//                    nx = static_cast<int>(floor((x + 0.5 * wp) * inv_wp * N));
//                    ny = static_cast<int>(floor((y + 0.5 * wp) * inv_wp * N));
//
//                    for (int nz = 0; nz < Nz; nz++) {
//
//                        z = (nz / Nz) * (max_z - min_z) + min_z;
//                        x_z = (z + D) / (g * z) * x + uo[i * Wpin + j] / z;
//                        y_z = (z + D) / (g * z) * y + vo[i * Wpin + j] / z;
//                        u = int(round((x_z - u_min) * inv_pc_intv));
//                        v = int(round((y_z - v_min) * inv_pc_intv));
//                        block_size = int(round(pd_g * z * inv_pc_intv * 0.5));
//                        //block_size = pd_g * z * inv_pc_intv;
//                        cout << "block size:" << block_size << endl;
//                        //cout << "u:" << u << ", v:" << v << ", z:" << z << endl;
//                        for (int k = -block_size; k <= block_size; k++) {
//                            for (int l = -block_size; l <= block_size; l++) {
//                                if (u + k >= 0 && u + k <= WIDTH && v + l >= 0 && v + l <= HEIGHT) {
//                                    cout << "wow" << endl;
//                                    if (pcd_space[u + k][v + l][nz][0] + pcd_space[u + k][v + l][nz][1] + pcd_space[u + k][v + l][nz][2] > 0) {
//
//                                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[0] = pcd_space[u + k][v + l][nz][0];
//                                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[1] = pcd_space[u + k][v + l][nz][1];
//                                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[2] = pcd_space[u + k][v + l][nz][2];
//                                        break;
//
//                                    }
//                                }
//                                else {
//                                    break;
//                                }
//                            }
//                        }
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
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
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
//    int Nz = 100;
//    double**** pcdspace;
//    pcdspace = (double****)malloc(sizeof(double***) * WIDTH);
//    for (int i = 0; i < WIDTH; i++) {
//        pcdspace[i] = (double***)malloc(sizeof(double**) * HEIGHT);
//        for (int j = 0; j < HEIGHT; j++) {
//            pcdspace[i][j] = (double**)malloc(sizeof(double*) * Nz);
//            for (int k = 0; k < Nz; k++) {
//                pcdspace[i][j][k] = (double*)malloc(sizeof(double) * 3);
//            }
//        }
//    }
//
//    for (int i = 0; i < 3; i++)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        cv::waitKey(10);
//    }
//
//    for (int tt = 0; tt < 1; tt++) {
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
//        // 計測開始時間
//        //auto start = chrono::high_resolution_clock::now();
//        cpu_time = clock();
//        start = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//        cout << "before pc calc" << endl;
//
//        // Generate the pointcloud and texture mappings
//        points = pc.calculate(depth_frame);
//
//        // Tell pointcloud object to map to this color frame
//        pc.map_to(color_frame);
//
//        auto verts = points.get_vertices();
//        //cout << "num of points:" << sizeof(verts) << endl;
//
//        cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//
//        cout << "before raw_pointclouddata" << endl;
//
//        //vector<vector<double>> pointclouddata(WIDTH * HEIGHT, vector<double>(6));
//        double* raw_pointclouddata;
//        raw_pointclouddata = (double*)malloc(sizeof(double) * WIDTH * HEIGHT * 3);
//
//        double max_z = 0, min_z = 100;
//        for (int i = 0; i < WIDTH * HEIGHT; i++) {
//
//            //cout << "i:" << i << endl;
//            if (verts[i].z > max_z) {
//                max_z = verts[i].z;
//            }
//            if (verts[i].z > 0 && verts[i].z < min_z) {
//                min_z = verts[i].z;
//            }
//
//            //raw_pointclouddata[i * 3] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[0];
//            //raw_pointclouddata[i * 3 + 1] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[1];
//            //raw_pointclouddata[i * 3 + 2] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[2];
//
//        }
//        double u_min = verts[0].x / verts[0].z, v_min = verts[0].y / verts[0].z;
//
//        int u, v, z;
//        double inv_pc_intv = 1.0 / 0.00165;
//        for (int i = 0; i < HEIGHT; i++) {
//            for (int j = 0; j < WIDTH; j++) {
//                if (verts[i * WIDTH + j].z > 0) {
//
//                    u = int(round(((verts[i * WIDTH + j].x / verts[i * WIDTH + j].z) - u_min) * inv_pc_intv));
//                    v = int(round(((verts[i * WIDTH + j].y / verts[i * WIDTH + j].z) - v_min) * inv_pc_intv));
//                    z = int(round((verts[i * WIDTH + j].z - min_z) / (max_z - min_z) * (Nz - 1)));
//                    //cout << "u:" << u << ", v:" << v << ", z:" << z << endl;
//                    pcdspace[u][v][z][0] = color_image.at<cv::Vec3b>(i, j)[0];
//                    pcdspace[u][v][z][1] = color_image.at<cv::Vec3b>(i, j)[1];
//                    pcdspace[u][v][z][2] = color_image.at<cv::Vec3b>(i, j)[2];
//                }
//            }
//        }
//
//
//        const int numThreads = 1;
//        vector<thread> threads;
//        int rowsPerThread = Hpin / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, WIDTH, HEIGHT, Hpin, Wpin, N, Nz, D, max_z, min_z, u_min, v_min, g, wp, pd, inv_wp, intv, startRow, endRow, uo, vo, pcdspace, points, img_display);
//            //(int WIDTH, int HEIGHT, int Wpin, int N, int D, double g, double wp, double inv_wp, double intv, int starty, int endy, double* uo, double* vo, rs2::vertex * verts, cv::Mat color_image, cv::Mat img_display, double* val_z);
//        }
//
//        for (auto& t : threads) {
//            t.join();
//        }
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
//        //            //cout << "img_display 0:" << img_display.at<cv::Vec3b>(i, j)[0] << ", 1:" << img_display.at<cv::Vec3b>(i, j)[1] << ", 2:" << img_display.at<cv::Vec3b>(i, j)[2] << endl;
//        //        }
//        //    }
//        //}
//
//        // 計測終了時間
//        //auto finish = chrono::high_resolution_clock::now();
//        cpu_time = clock();
//        finish = (double)cpu_time / (double)CLOCKS_PER_SEC;
//
//        // 経過時間を出力
//        //chrono::duration<double> elapsed = finish - start;
//        //cout << "Time to fill the array: " << elapsed.count() << " seconds" << endl;
//
//        cout << "Time to fill the array: " << finish - start << " seconds" << endl;
//
//        ostringstream stream;
//        stream << "v11_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png"; // 小数点以下2桁で切り捨て
//        cv::String filename = stream.str();
//
//        imwrite(filename, img_display);
//
//        //// 画像を表示
//        //cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        //cv::imshow("Display window", img_display);
//
//        //// キーボードの入力を待つ（1ms）
//        //// 何かしらのキーが押されたらループを抜ける
//        //if (cv::waitKey(1) >= 0) break;
//
//    }
//
//    free(elem_img);
//    free(uo);
//    free(vo);
//    //free(raw_pointclouddata);
//    //free(pointclouddata);
//
//    for (int i = 0; i < WIDTH; i++) {
//        for (int j = 0; j < HEIGHT; j++) {
//            for (int k = 0; k < Nz; k++) {
//                free(pcdspace[i][j][k]);
//            }
//            free(pcdspace[i][j]);
//        }
//        free(pcdspace[i]);
//    }
//    free(pcdspace);
//
//    // ウィンドウを閉じる
//    cv::destroyAllWindows();
//
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

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
////#include <thread>
//#include <cmath>
//#include <time.h>
//#include <omp.h>
//#include <immintrin.h>
//
//using namespace std;
//
//void insert_pixels(int WIDTH, int HEIGHT, int Wpin, int N, int D, double g, double wp, double inv_wp, double intv, int starty, int endy, double* uo, double* vo, const uint8_t* color_data, rs2::points points, cv::Mat img_display, double* val_z) {
//
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
//        //auto start = chrono::high_resolution_clock::now();
//        cpu_time = clock();
//        start = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//        for (int i = 0; i < Hpin * Wpin * N * N; i++) {
//            val_z[i] = 10;
//        }
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
//        //cv::Mat color_image_rgb(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
//        //cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        //cv::Mat color_image;
//        //cv::cvtColor(color_image_rgb, color_image, cv::COLOR_RGB2BGR);
//
//        //// 画像を表示
//        //cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        //cv::imshow("Display window", color_image);
//
//        //// 任意のキーが押されるまで待つ
//        //cv::waitKey(0);
//
//        cout << "before pc calc" << endl;
//
//        // Generate the pointcloud and texture mappings
//        points = pc.calculate(depth_frame);
//
//        // Tell pointcloud object to map to this color frame
//        pc.map_to(color_frame);
//
//        //auto verts = points.get_vertices();
//        //cout << "num of points:" << sizeof(verts) << endl;
//
//        cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//
//        const uint8_t* color_data = reinterpret_cast<const uint8_t*>(color_frame.get_data());
//
//        cout << "before raw_pointclouddata" << endl;
//
//        //vector<vector<double>> pointclouddata(WIDTH * HEIGHT, vector<double>(6));
//        //double* raw_pointclouddata;
//        //raw_pointclouddata = (double*)malloc(sizeof(double) * WIDTH * HEIGHT * 3);
//
//        //double vtx_x, vtx_y, vtx_z;
//        //for (int i = 0; i < WIDTH * HEIGHT; i++) {
//
//        //    //cout << "i:" << i << endl;
//
//        //    raw_pointclouddata[i * 3] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[0];
//        //    raw_pointclouddata[i * 3 + 1] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[1];
//        //    raw_pointclouddata[i * 3 + 2] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[2];
//
//        //}
//
//        //cout << "before sort raw_pointclouddata" << endl;
//
//        //// ソートする列のインデックス（0から始まる）
//        //int sort_index = 2;
//
//        //quickSort(raw_pointclouddata, 0, WIDTH * HEIGHT - 1, 6, sort_index);
//
//        //cout << "before create pointclouddata" << endl;
//
//        //double* pointclouddata;
//        //pointclouddata = (double*)malloc(sizeof(double) * WIDTH * HEIGHT * 6);
//
//        //// raw...のブロックのうち、z > 0であるものをpointclouddataにコピー
//        //int pcd_size = 0;
//        //for (int i = 0; i < WIDTH * HEIGHT; i++) {
//        //    if (raw_pointclouddata[i * 6 + 2] > 0) { // z > 0である場合
//        //        for (int j = 0; j < 6; j++) {
//        //            pointclouddata[pcd_size++] = raw_pointclouddata[i * 6 + j];
//        //        }
//        //    }
//        //}
//
//        //cout << "CSVファイルへの出力を開始しました。" << endl;
//
//        //// 出力ファイルを開く
//        //ofstream outputFile("output_v3.csv");
//
//        //if (!outputFile.is_open()) {
//        //    cerr << "ファイルを開けませんでした。" << endl;
//        //    return 1;
//        //}
//
//        //// 二次元配列をループで走査し、CSV形式でファイルに出力
//        //for (int i = 0; i < int(pcd_size / 6); i++) {
//        //    for (int j = 0; j < 6; j++) {
//        //        outputFile << pointclouddata[i * 6 + j];
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
//        //cout << "CSVファイルへの出力が完了しました。" << endl;
//
//        double inv_d, vtx_z, newx, newy;
//        int nx, ny, startv, startu;
//        int inv_WIDTH = 1.0 / WIDTH;
//        auto verts = points.get_vertices();
//
//#pragma omp parallel for private(newx, newy, nx, ny, startv, startu, inv_d, vtx_z)
//        for (int k = 0; k < WIDTH * HEIGHT; k++) {
//
//            vtx_z = verts[k].z;
//
//            //cout << "pcd0:" << tmp_pcd0 << ", pcd1:" << tmp_pcd1 << ", pcd2:" << vtx_z << ", pcd3:" << tmp_pcd3 << ", pcd4:" << tmp_pcd4 << ", pcd5:" << tmp_pcd5 << endl;
//            //if (k % 1000 == 0) {
//            //    cout << "k:" << k << "\n" << endl;
//            //}
//
//            //vector<vector<double>> newx(endy - starty, vector<double>(Wpin));
//            //vector<vector<double>> newy(endy - starty, vector<double>(Wpin));
//
//            //vector<vector<int>> nx(endy - starty, vector<int>(Wpin));
//            //vector<vector<int>> ny(endy - starty, vector<int>(Wpin));
//
//            inv_d = 1.0 / (vtx_z * 1000 + D);
//
//            for (int i = 0; i < Hpin; i++) {
//                for (int j = 0; j < Wpin; j++) {
//
//                    //cout << "i:" << i << ", j:" << j << endl;
//
//                    newx = -(g * inv_d) * (uo[i * Wpin + j] - verts[k].x * 1000);
//                    newy = -(g * inv_d) * (vo[i * Wpin + j] - verts[k].y * 1000);
//
//                    __m256d newx_vec = _mm256_set1_pd(newx);
//                    __m256d newy_vec = _mm256_set1_pd(newy);
//
//                    nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//                    ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//                    startv = static_cast<int>(i * intv);
//                    startu = static_cast<int>(j * intv);
//
//                    //cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << endl;
//                    //cout << "nx:" << nx << ", ny:" << ny << endl;
//
//                    if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                        if (val_z[((i * Wpin + j) * N + ny) * N + nx] > vtx_z) {
//
//                            img_display.at<cv::Vec3b>(startv + ny, startu + nx)[0] = color_data[k * 3 + 2];
//                            img_display.at<cv::Vec3b>(startv + ny, startu + nx)[1] = color_data[k * 3 + 1];
//                            img_display.at<cv::Vec3b>(startv + ny, startu + nx)[2] = color_data[k * 3];
//                            val_z[((i * Wpin + j) * N + ny) * N + nx] = vtx_z;
//                        }
//                    }
//                }
//            }
//        }
//
//        //int startv, startu;
//        //for (int i = 0; i < Hpin; i++) {
//        //    for (int j = 0; j < Wpin; j++) {
//
//        //        //cout << "i:" << i << ", j:" << j << endl;
//
//        //        startv = static_cast<int>(i * intv);
//        //        startu = static_cast<int>(j * intv);
//
//        //        for (int v = 0; v < N; v++) {
//        //            for (int u = 0; u < N; u++) {
//
//        //                //cout << "u:" << u << ", v:" << v << endl;
//
//        //                img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[(((i * Wpin + j) * N + v) * N + u) * 3];
//        //                img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[(((i * Wpin + j) * N + v) * N + u) * 3 + 1];
//        //                img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[(((i * Wpin + j) * N + v) * N + u) * 3 + 2];
//        //            }
//        //        }
//        //    }
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
//        stream << "v12_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png"; // 小数点以下2桁で切り捨て
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

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
//void insert_pixels(int Wpin, int N, int Nz, double D, double g, double wp, double pd, int starty, int endy, std::vector<std::vector<double>>& uo, std::vector<std::vector<double>>& vo, std::vector<std::vector<cv::Mat>>& elem_img, std::vector<std::vector<double>>& pointclouddata) {
//
//    g *= 0.001;
//    wp *= 0.001;
//    pd *= 0.001;
//
//    //std::cout << "g:" << g << ", wp:" << wp << ", pd:" << pd << std::endl;
//
//    double s, t, z, u, v, delta;
//    double inv_g = 1.0 / g, inv_wp = 1.0 / wp, inv_pd = 1.0 / pd, inv_Nz = 1.0 / Nz;
//    double x, y;
//    //int nx, ny;
//    int flag = 0, startk = 0, tmp_startk = 0;
//    double min_z = pointclouddata[0][2], max_z = pointclouddata[pointclouddata.size() - 1][2];
//    double range_z = (max_z - min_z) * inv_Nz;
//    //std::cout << min_z << ", " << max_z << std::endl;
//    //std::cout << "range_z:" << range_z << std::endl;
//
//    for (int i = starty; i < endy; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            std::cout << "i:" << i << ", j:" << j << std::endl;
//
//            s = uo[i][j] * 0.001;
//            t = vo[i][j] * 0.001;
//
//            //std::cout << "s:" << s << ", t:" << t << std::endl;
//
//            //// double型でインクリメント
//            //for (double y = -0.5 * (wp - pd); y <= 0.5 * (wp - pd); y += pd) {
//            //    for (double x = -0.5 * (wp - pd); x <= 0.5 * (wp - pd); x += pd) {
//
//            //        nx = static_cast<int>(floor((x + 0.5 * wp) * inv_wp * N));
//            //        ny = static_cast<int>(floor((y + 0.5 * wp) * inv_wp * N));
//
//            //        //std::cout << "nx:" << nx << ", ny:" << ny << std::endl;
//
//            //        for (int k = 0; k < pointclouddata.size(); k++) {
//
//            //            z = pointclouddata[k][2];
//            //            u = (z + D) * inv_g * x + s;
//            //            v = (z + D) * inv_g * y + t;
//            //            delta = (z + D) * inv_g * pd * 0.5;
//
//            //            //std::cout << "pcd2:" << pointclouddata[k][2] << std::endl;
//
//            //            //std::cout << u - delta << "<=" << pointclouddata[k][0] << "<=" << u + delta << "?" << std::endl;
//            //            //std::cout << v - delta << "<=" << pointclouddata[k][1] << "<=" << v + delta << "?" << std::endl;
//
//            //            if (-delta + u <= pointclouddata[k][0] && pointclouddata[k][0] <= delta + u) {
//            //                if (-delta + v <= pointclouddata[k][1] && pointclouddata[k][1] <= delta + v) {
//
//            //                    elem_img[i][j].at<cv::Vec3b>(ny, nx)[0] = pointclouddata[k][3];
//            //                    elem_img[i][j].at<cv::Vec3b>(ny, nx)[1] = pointclouddata[k][4];
//            //                    elem_img[i][j].at<cv::Vec3b>(ny, nx)[2] = pointclouddata[k][5];
//            //                    //std::cout << "elem_img[" + i << "][" << j << "][" << ny << "][" << nx << "]:" << elem_img[i][j].at<cv::Vec3b>(ny, nx) << std::endl;
//            //                    flag = 1;
//            //                    //tmp_startk = 0;
//            //                    break;
//            //                }
//            //            }
//            //        }
//            //    }
//            //}
//
//            // int型でインクリメント
//            for (int ny = 0; ny < N; ny++) {
//                for (int nx = 0; nx < N; nx++) {
//
//                    x = (nx + 0.5) * pd - wp * 0.5;
//                    y = (ny + 0.5) * pd - wp * 0.5;
//
//                    //std::cout << "x:" << x << ", y:" << y << std::endl;
//
//                    for (int n = 0; n < Nz; n++) {
//
//                        //std::cout << "n:" << n << std::endl;
//
//                        z = range_z * n + min_z;
//                        u = (z + D) * inv_g * x + s;
//                        v = (z + D) * inv_g * y + t;
//                        delta = (z + D) * inv_g * pd * 0.5;
//
//                        for (int k = startk; k < pointclouddata.size(); k++) {
//                            //std::cout << "z:" << pointclouddata[k][2] << std::endl;
//                            //std::cout << "z - range_z:" << z - range_z << std::endl;
//                            //std::cout << "z + range_z:" << z + range_z << std::endl;
//
//                            //std::cout << "u:" << pointclouddata[k][0] << std::endl;
//                            //std::cout << "u - delta:" << u - delta << std::endl;
//                            //std::cout << "u + delta:" << u + delta << std::endl;
//
//                            //std::cout << "v:" << pointclouddata[k][1] << std::endl;
//                            //std::cout << "v - delta:" << v - delta << std::endl;
//                            //std::cout << "v + delta:" << v + delta << std::endl;
//
//                            if ((z - range_z * 0.5) <= pointclouddata[k][2] && pointclouddata[k][2] < (z + range_z * 0.5)) {
//                                //std::cout << "wow" << std::endl;
//                                if ((u - delta) <= pointclouddata[k][0] && pointclouddata[k][0] <= (u + delta) && (v - delta) <= pointclouddata[k][1] && pointclouddata[k][1] <= (v + delta)) {
//
//                                    elem_img[i][j].at<cv::Vec3b>(ny, nx)[0] = pointclouddata[k][3];
//                                    elem_img[i][j].at<cv::Vec3b>(ny, nx)[1] = pointclouddata[k][4];
//                                    elem_img[i][j].at<cv::Vec3b>(ny, nx)[2] = pointclouddata[k][5];
//                                    //std::cout << "elem_img[" << i << "][" << j << "][" << ny << "][" << nx << "]:" << elem_img[i][j].at<cv::Vec3b>(ny, nx) << std::endl;
//
//                                    //cliped_pointclouddata.push_back(pointclouddata[k]);
//                                    flag = 1;
//                                    tmp_startk = 0;
//                                    startk = 0;
//                                    break;
//                                }
//                                tmp_startk++;
//                            }
//                            else {
//                                startk += tmp_startk;
//                                tmp_startk = 0;
//                                break;
//                            }
//                        }
//
//                        if (flag == 1) {
//                            flag = 0;
//                            break;
//                        }
//
//                        if (n == Nz - 1) {
//                            startk = 0;
//                        }
//
//                        /* ↓遅すぎw */
//                        //// 条件に一致する範囲を抽出する新たな二次元配列を作成
//                        //std::vector<std::vector<double>> cliped_pointclouddata;
//
//                        //// 領域内のみコピー
//                        //std::copy_if(pointclouddata.begin(), pointclouddata.end(), std::back_inserter(cliped_pointclouddata), [u, v, delta, z, range_z](const std::vector<double>& row) {
//                        //    return (u - delta) <= row[0] && row[0] <= (u + delta) && (v - delta) <= row[1] && row[1] <= (v + delta) && (z - range_z * 0.5) <= row[2] && row[2] <= (z + range_z * 0.5);
//                        //});
//
//                        //if (!cliped_pointclouddata.empty()) {
//                        //    elem_img[i][j].at<cv::Vec3b>(ny, nx)[0] = cliped_pointclouddata[0][3];
//                        //    elem_img[i][j].at<cv::Vec3b>(ny, nx)[1] = cliped_pointclouddata[0][4];
//                        //    elem_img[i][j].at<cv::Vec3b>(ny, nx)[2] = cliped_pointclouddata[0][5];
//                        //    std::cout << "elem_img[" << i << "][" << j << "][" << ny << "][" << nx << "]:" << elem_img[i][j].at<cv::Vec3b>(ny, nx) << std::endl;
//                        //    break;
//                        //}
//                        //else {
//                        //    //std::cout << "empty" << std::endl;
//                        //}
//                        /* 以上 */
//
//                    }
//
//                    //for (int k = 0; k < pointclouddata.size(); k++) {
//
//                    //    z = pointclouddata[k][2];
//                    //    u = (z + D) * inv_g * x + s;
//                    //    v = (z + D) * inv_g * y + t;
//                    //    delta = (z + D) * inv_g * pd * 0.5;
//
//                    //    //std::cout << "pcd2:" << pointclouddata[k][2] << std::endl;
//
//                    //    //std::cout << u - delta << "<=" << pointclouddata[k][0] << "<=" << u + delta << "?" << std::endl;
//                    //    //std::cout << v - delta << "<=" << pointclouddata[k][1] << "<=" << v + delta << "?" << std::endl;
//
//                    //    if (-delta + u <= pointclouddata[k][0] && pointclouddata[k][0] <= delta + u) {
//                    //        if (-delta + v <= pointclouddata[k][1] && pointclouddata[k][1] <= delta + v) {
//
//                    //            elem_img[i][j].at<cv::Vec3b>(ny, nx)[0] = pointclouddata[k][3];
//                    //            elem_img[i][j].at<cv::Vec3b>(ny, nx)[1] = pointclouddata[k][4];
//                    //            elem_img[i][j].at<cv::Vec3b>(ny, nx)[2] = pointclouddata[k][5];
//                    //            //std::cout << "elem_img[" + i << "][" << j << "][" << ny << "][" << nx << "]:" << elem_img[i][j].at<cv::Vec3b>(ny, nx) << std::endl;
//                    //            flag = 1;
//                    //            //tmp_startk = 0;
//                    //            break;
//                    //        }
//                    //    }
//                    //}
//                }
//            }
//
//        }
//    }
//
//    //double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//    //for (int k = 0; k < pointclouddata.size(); k++) {
//
//    //    tmp_pcd0 = pointclouddata[k][0];
//    //    tmp_pcd1 = pointclouddata[k][1];
//    //    tmp_pcd2 = pointclouddata[k][2];
//    //    tmp_pcd3 = pointclouddata[k][3];
//    //    tmp_pcd4 = pointclouddata[k][4];
//    //    tmp_pcd5 = pointclouddata[k][5];
//
//    //    if (k % 1000 == 0) {
//    //        std::cout << "k:" << k << "\n" << std::endl;
//    //    }
//
//    //    //std::vector<std::vector<double>> newx(endy - starty, std::vector<double>(Wpin));
//    //    //std::vector<std::vector<double>> newy(endy - starty, std::vector<double>(Wpin));
//
//    //    //std::vector<std::vector<int>> nx(endy - starty, std::vector<int>(Wpin));
//    //    //std::vector<std::vector<int>> ny(endy - starty, std::vector<int>(Wpin));
//
//    //    inv_d = 1.0 / (tmp_pcd2 * 1000 + D);
//
//    //    double newx, newy;
//    //    int nx, ny;
//    //    for (int i = 0; i < endy - starty; i++) {
//    //        for (int j = 0; j < Wpin; j++) {
//
//    //            //std::cout << "i:" << i << ", j:" << j << std::endl;
//
//    //            newx = -(g * inv_d) * (uo[i + starty][j] - tmp_pcd0 * 1000);
//    //            newy = -(g * inv_d) * (vo[i + starty][j] - tmp_pcd1 * 1000);
//
//    //            nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//    //            ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//    //            //std::cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << std::endl;
//    //            //std::cout << "nx:" << nx << ", ny:" << ny << std::endl;
//
//    //            if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//    //                if (elem_img[i + starty][j].at<cv::Vec3b>(ny, nx)[0] == 0 && elem_img[i + starty][j].at<cv::Vec3b>(ny, nx)[1] == 0 && elem_img[i + starty][j].at<cv::Vec3b>(ny, nx)[2] == 0) {
//
//    //                    elem_img[i + starty][j].at<cv::Vec3b>(ny, nx)[0] = static_cast<int>(tmp_pcd3);
//    //                    elem_img[i + starty][j].at<cv::Vec3b>(ny, nx)[1] = static_cast<int>(tmp_pcd4);
//    //                    elem_img[i + starty][j].at<cv::Vec3b>(ny, nx)[2] = static_cast<int>(tmp_pcd5);
//
//    //                }
//    //            }
//    //        }
//    //    }
//    //}
//}
//
//void round_column(std::vector<std::vector<double>>& matrix, int columnIndex, int order) {
//    for (auto& row : matrix) {
//        if (columnIndex < row.size()) {
//
//            //std::cout << "row[" << columnIndex << "]:" << row[columnIndex] << std::endl;
//            row[columnIndex] *= pow(10, order);
//            row[columnIndex] = std::round(row[columnIndex]);
//            row[columnIndex] /= pow(10, order);
//            //std::cout << "row[" << columnIndex << "]:" << row[columnIndex] << std::endl;
//        }
//    }
//}
//int main(int argc, char* argv[]) try
//{
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
//    std::vector<std::vector<cv::Mat>> elem_img(Hpin, std::vector<cv::Mat>(Wpin));
//    for (int i = 0; i < Hpin; ++i) {
//        for (int j = 0; j < Wpin; ++j) {
//            elem_img[i][j] = cv::Mat(cv::Size(N, N), CV_8UC3, cv::Scalar(0, 0, 0));
//        }
//    }
//
//    int Nz = 100;                       // 奥行き方向zの段階数
//    double inv_Nz = 1.0 / Nz;           // Nzの逆数
//    std::vector<double> z_level(Nz);    // zのレベル
//
//    for (int i = 0; i < Nz; i++) {
//        z_level[i] = inv_Nz * (i + 1);
//        //std::cout << "z_level:" << z_level[i] << std::endl;
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
//    double D = 0.1;                     // 表示系とカメラとの距離(m)
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
//    for (int i = 0; i < 3; i++)
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
//        auto color_frame = aligned_frames.get_color_frame();
//        auto depth_frame = aligned_frames.get_depth_frame();
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
//
//        std::cout << "before raw_pointclouddata" << std::endl;
//
//        std::vector<std::vector<double>> raw_pointclouddata(WIDTH * HEIGHT, std::vector<double>(6));
//        double vtx_x, vtx_y, vtx_z;
//        for (int i = 0; i < WIDTH * HEIGHT; i++) {
//
//            vtx_x = verts[i].x;
//            vtx_y = verts[i].y;
//            vtx_z = verts[i].z;
//
//            for (int j = 0; j < 6; j++) {
//
//                //std::cout << "x:" << i % WIDTH << ", y:" << static_cast<int>(i * inv_WIDTH) << std::endl;
//
//                if (j == 0) {
//                    raw_pointclouddata[i][j] = vtx_x;
//                }
//                else if (j == 1) {
//                    raw_pointclouddata[i][j] = vtx_y;
//                }
//                else if (j == 2) {
//                    //raw_pointclouddata[i][j] = static_cast<double>(depth_frame.get_distance(i % WIDTH, i * inv_WIDTH));
//                    raw_pointclouddata[i][j] = vtx_z;
//                }
//                else if (j == 3) {
//                    raw_pointclouddata[i][j] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[0];
//                }
//                else if (j == 4) {
//                    raw_pointclouddata[i][j] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[1];
//                }
//                else {
//                    raw_pointclouddata[i][j] = color_image.at<cv::Vec3b>(static_cast<int>(i * inv_WIDTH), (i % WIDTH))[2];
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
//        // zが0より大きい行をBにコピー
//        std::copy_if(raw_pointclouddata.begin(), raw_pointclouddata.end(), std::back_inserter(pointclouddata), [](const std::vector<double>& row) {
//            return row[2] > 0; // z>0の行を選択
//            });
//
//        //// 小数点以下第3位を四捨五入して奥行き方向zを1000段階にする
//        //round_column(pointclouddata, 2, static_cast<int>(log10(Nz)) - 1);
//
//        //// 出力ファイルを開く
//        //std::ofstream outputFile("output_v2.csv");
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
//        // 要素画像内の画素を埋める
//        insert_pixels(Wpin, N, Nz, D, g, wp, pd, 0, Hpin, std::ref(uo), std::ref(vo), std::ref(elem_img), std::ref(pointclouddata));
//
//        // //要素画像内の画素を埋める(並列処理)
//        //const int numThreads = 17;
//        //std::vector<std::thread> threads;
//        //int rowsPerThread = Hpin / numThreads;
//
//        //int startRow, endRow;
//        //for (int i = 0; i < numThreads; i++) {
//        //    startRow = i * rowsPerThread;
//        //    endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//        //    threads.emplace_back(insert_pixels, Wpin, N, D, g, wp, pd, startRow, endRow, std::ref(uo), std::ref(vo), std::ref(elem_img), std::ref(pointclouddata));
//        //}
//
//        //for (auto& t : threads) {
//        //    t.join();
//        //}
//
//        // 要素画像群を表示画像の形にする
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
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elem_img[i][j].at<cv::Vec3b>(v, u)[0];
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elem_img[i][j].at<cv::Vec3b>(v, u)[1];
//                        img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elem_img[i][j].at<cv::Vec3b>(v, u)[2];
//                    }
//                }
//            }
//        }
//
//        //// 黒画素を周辺8近傍の画素の平均値で埋める
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

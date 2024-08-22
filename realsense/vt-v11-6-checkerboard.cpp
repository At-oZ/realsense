///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* v4-3との差分：rgbを一つの値にまとめ、img_displayを2次元配列に変更 */
//
//// #include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
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
//// 表示系のパラメータ(mm)
//int W = 170, H = 170;                                                                       // 表示の縦横幅
//double g = 4.0, wp = 1.25, a = 0.125;                                                       // ギャップ、ピッチ、ピンホール幅
//double ld = 13.4 * 25.4;                                                                    // ディスプレイサイズ
//int pw = 3840, ph = 2400;                                                                   // ディスプレイの縦横の解像度
//double pd = ld / sqrtf(pw * pw + ph * ph);                                                  // 画素ピッチ
//int N = static_cast<int>(floor(wp / pd));                                                   // 要素画像の解像度
//int pw_disp = static_cast<int>(floor(W / pd)), ph_disp = static_cast<int>(floor(H / pd));   // 表示画像の縦横の解像度
//int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp));         // 縦横のピンホール数
//double intv = wp / pd;                                                                      // 要素画像の間隔
//
//double D = 100;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//// パラメータ
//int num_z_level = 65; // イメージプレーンの層数
//double ptimes = 1; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//int box_size = 1; // 探索範囲
//
//int Ddash = 250;
//double coef = (double)num_z_level * (double)Ddash;
//
//double img_pitch = ptimes * pd;
//int num_z = 1751;
//
//int px_height_img = static_cast<int>(round(55 / ptimes));
//int px_width_img = static_cast<int>(round(55 / ptimes));
//
//
//void insert_pixels(int start, int end, int** img_display, int*** pcd_box, double* sp, double* tp) {
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
//                                img_display[startv + m][startu + n] = tmp_pcd_color - alpha;
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
//int main(int argc, char* argv[])
//{
//
//    std::cout << "vt-v11-6-checkerboard" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    std::cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    std::cout << "wp:" << wp << endl;
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
//            //std::cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//        }
//    }
//
//    cv::Mat img_save = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//
//    //int** num_px;
//    //num_px = new int* [ph_disp];
//    //for (int i = 0; i < ph_disp; i++) {
//    //    num_px[i] = new int[pw_disp];
//    //}
//
//    int** img_display;
//    img_display = (int**)malloc(sizeof(int*) * ph_disp);
//    for (int i = 0; i < ph_disp; i++) {
//        img_display[i] = (int*)malloc(sizeof(int) * pw_disp);
//    }
//
//    cv::Mat board = cv::imread("C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/checkerboard.bmp");
//    double cpd = wp / (double)board.rows;
//    std::cout << "cpd:" << cpd << ", pd:" << pd << endl;
//
//    int*** pcd_box;
//    pcd_box = (int***)malloc(sizeof(int**) * num_z_level);
//    for (int i = 0; i < num_z_level; i++) {
//        pcd_box[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        for (int j = 0; j < px_height_img; j++) {
//            pcd_box[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//        }
//    }
//
//    //for (int i = 0; i < board.rows; i++) {
//    //    for (int j = 0; j < board.cols; j++) {
//    //        std::cout << "i:" << i << ", j:" << ", board:" << board.at<cv::Vec3b>(i, j) << endl;
//    //    }
//    //}
//
//    double z, xt, yt, zt, cv, cu;
//    int nx, ny, nz;
//
//    // フレーム処理
//    for (int tt = 0; tt < num_z; tt += 50) {
//
//        z = 250 + tt;
//        std::cout << "z:" << z << endl;
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        //img_save = cv::Scalar::all(0);
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                //img_save.at<cv::Vec3b>(i, j)[0] = 121;
//                //img_save.at<cv::Vec3b>(i, j)[1] = 77;
//                //img_save.at<cv::Vec3b>(i, j)[2] = 49;
//                img_display[i][j] = 121 + 77 * 256 + 49 * 256 * 256;
//            }
//        }
//
//        // 箱とzバッファをオフセット
//        for (int i = 0; i < num_z_level; i++) {
//            for (int j = 0; j < px_height_img; j++) {
//                for (int k = 0; k < px_width_img; k++) {
//                    //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
//                    pcd_box[i][j][k] = 0;
//                }
//            }
//        }
//
//        for (int i = 0; i < board.rows; i++) {
//            for (int j = 0; j < board.cols; j++) {
//                //std::cout << "i:" << i << ", j:" << ", board:" << board.at<cv::Vec3b>(i, j) << endl;
//
//                cu = ((double)j - (board.cols - 1) * 0.5) * cpd;
//                cv = ((double)i - (board.rows - 1) * 0.5) * cpd;
//                //std::cout << "cu:" << cu << ", cv:" << cv << endl;
//
//                xt = 1.0 / g * cu;
//                yt = 1.0 / g * cv;
//                zt = 1.0 / z;
//                //std::cout << "xt:" << xt << ", yt:" << yt << endl;
//
//                nx = static_cast<int>(floor((g / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                ny = static_cast<int>(floor((g / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//                nz = static_cast<int>(floor(coef * zt + 0.5));
//                //std::cout << "nx:" << nx << ", ny:" << ny << " nz:" << nz << endl;
//
//                if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img && 0 <= nz && nz < num_z_level) {
//                    //if (pcd_box[nz][ny][nx] > 0) {
//                    //    pcd_count++;
//                    //}
//                    pcd_box[nz][ny][nx] = static_cast<unsigned char>(board.at<cv::Vec3b>(i, j)[0]) + static_cast<unsigned char>(board.at<cv::Vec3b>(i, j)[1]) * 256 + static_cast<unsigned char>(board.at<cv::Vec3b>(i, j)[2]) * 256 * 256 + 256 * 256 * 256;
//                    //std::cout << "pcd_box:" << pcd_box[nz][ny][nx] << endl;
//                    //pcd_box[nz][ny][nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256 + 256 * 256 * 256;
//
//                }
//
//            }
//        }
//
//        double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//        double newx, newy;
//        int nx, ny;
//
//        const int numThreads = 16;
//        vector<thread> threads;
//        int rowsPerThread = N / numThreads;
//
//        //insert_pixels(0, N, std::ref(img_display), std::ref(pcd_box), std::ref(sp), std::ref(tp));
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
//        std::cout << "calc finished" << endl;
//        finished_threads = 0;
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        std::cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                img_save.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i][j] / (256 * 256)));
//                img_save.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i][j] / 256 - img_save.at<cv::Vec3b>(i, j)[2] * 256));
//                img_save.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i][j] - (img_save.at<cv::Vec3b>(i, j)[2] * 256 - img_save.at<cv::Vec3b>(i, j)[1]) * 256));
//            }
//        }
//
//        ostringstream stream;
//        stream << "./checkerboard/vt-v11-6/vt-v11-6-checkerboard_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_val-z" << fixed << setprecision(0) << z << ".png";
//
//        cv::String filename = stream.str();
//
//        cv::imwrite(filename, img_save);
//
//        // // 画像を表示
//        // cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        // cv::imshow("Display window", img_display);
//
//        // // キーボードの入力を待つ（1ms）
//        // // 何かしらのキーが押されたらループを抜ける
//        // cv::waitKey(0);
//        // cv::destroyWindow("Display window");
//
//    }
//
//    for (int i = 0; i < num_z_level; i++) {
//        for (int j = 0; j < px_height_img; j++) {
//            std::free(pcd_box[i][j]);
//        }
//        std::free(pcd_box[i]);
//    }
//    std::free(pcd_box);
//
//    for (int i = 0; i < ph_disp; i++) {
//        std::free(img_display[i]);
//    }
//    std::free(img_display);
//    std::free(sp);
//    std::free(tp);
//
//    return EXIT_SUCCESS;
//}

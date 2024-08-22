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
//// #include </opt/homebrew/Cellar/gcc/13.2.0/lib/gcc/current/gcc/aarch64-apple-darwin22/13/include/omp.h>
//
//using namespace std;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//int interp_intv = 9;
//int range = 3;
//
//// 表示系のパラメータ(mm)
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
//int num_z = 1751;
//
//int WIDTH = 640;
//int HEIGHT = 480;
//int FPS = 30;
//
//double D = 100;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//
//void insert_pixels(int starty, int endy, double cpd, double* sp, double* tp, cv::Mat board, cv::Mat img_save, int** num_px, double val_z) {
//
//    int nx, ny;
//    int startv, startu;
//    double dx, dy, cu, cv, u, v;
//
//
//    for (int i = starty; i < endy; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            startv = static_cast<int>(i * intv);
//            startu = static_cast<int>(j * intv);
//
//            dx = g / val_z * sp[i * Wpin + j];
//            dy = g / val_z * tp[i * Wpin + j];
//
//            //cout << "i:" << i << ", j:" << j << endl;
//
//            for (int k = 0; k < board.rows; k++) {
//
//                cv = ((double)k - (board.rows - 1) * 0.5) * cpd;
//                v = cv - dy;
//                ny = static_cast<int>(floor(v / pd + N * 0.5));
//
//                for (int l = 0; l < board.cols; l++) {
//
//                    cu = ((double)l - (board.cols - 1) * 0.5) * cpd;
//                    u = cu - dx;
//                    nx = static_cast<int>(floor(u / pd + N * 0.5));
//
//                    //cout << "dx:" << dx << ", dy:" << dy << endl;
//                    if (ny >= 0 && ny < N && nx >= 0 && nx < N) {
//
//                        if (num_px[startv + ny][startu + nx] > 0) {
//                            //cout << "num px:" << num_px[startv + ny][startu + nx] << endl;
//                            img_save.at<cv::Vec3b>(startv + ny, startu + nx)[0] = (img_save.at<cv::Vec3b>(startv + ny, startu + nx)[0] * num_px[startv + ny][startu + nx] + board.at<cv::Vec3b>(k, l)[0]) / (num_px[startv + ny][startu + nx] + 1);
//                            img_save.at<cv::Vec3b>(startv + ny, startu + nx)[1] = (img_save.at<cv::Vec3b>(startv + ny, startu + nx)[1] * num_px[startv + ny][startu + nx] + board.at<cv::Vec3b>(k, l)[1]) / (num_px[startv + ny][startu + nx] + 1);
//                            img_save.at<cv::Vec3b>(startv + ny, startu + nx)[2] = (img_save.at<cv::Vec3b>(startv + ny, startu + nx)[2] * num_px[startv + ny][startu + nx] + board.at<cv::Vec3b>(k, l)[2]) / (num_px[startv + ny][startu + nx] + 1);
//                        }
//                        else {
//                            img_save.at<cv::Vec3b>(startv + ny, startu + nx)[0] = board.at<cv::Vec3b>(k, l)[0];
//                            img_save.at<cv::Vec3b>(startv + ny, startu + nx)[1] = board.at<cv::Vec3b>(k, l)[1];
//                            img_save.at<cv::Vec3b>(startv + ny, startu + nx)[2] = board.at<cv::Vec3b>(k, l)[2];
//                        }
//                        num_px[startv + ny][startu + nx]++;
//
//                        //img_save.at<cv::Vec3b>(startv + ny, startu + nx)[0] = board.at<cv::Vec3b>(k, l)[0];
//                        //img_save.at<cv::Vec3b>(startv + ny, startu + nx)[1] = board.at<cv::Vec3b>(k, l)[1];
//                        //img_save.at<cv::Vec3b>(startv + ny, startu + nx)[2] = board.at<cv::Vec3b>(k, l)[2];
//
//                    }
//                }
//            }
//        }
//    }
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
//    cout << "real-scene" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    cout << "wp:" << wp << endl;
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
//    cv::Mat img_save = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    int** num_px;
//    num_px = new int*[ph_disp];
//    for (int i = 0; i < ph_disp; i++) {
//        num_px[i] = new int[pw_disp];
//    }
//
//    int** img_display;
//    img_display = (int**)malloc(sizeof(int*) * ph_disp);
//    for (int i = 0; i < ph_disp; i++) {
//        img_display[i] = (int*)malloc(sizeof(int) * pw_disp);
//    }
//
//    double val_z;
//
//    cv::Mat board = cv::imread("C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/checkerboard.bmp");
//    double cpd = wp / (double)board.rows;
//    //cv::Mat board = cv::Mat::zeros(cv::Size(251, 251), CV_8UC3);
//
//    //for (int i = 0; i < board.rows; i++) {
//    //    for (int j = 0; j < board.cols; j++) {
//    //        cout << "i:" << i << ", j:" << ", board:" << board.at<cv::Vec3b>(i, j) << endl;
//    //    }
//    //}
//
//    // フレーム処理
//    for (int tt = 0; tt < num_z; tt += 50) {
//
//        //img_save = cv::Scalar::all(0);
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                img_save.at<cv::Vec3b>(i, j)[0] = 121;
//                img_save.at<cv::Vec3b>(i, j)[1] = 77;
//                img_save.at<cv::Vec3b>(i, j)[2] = 49;
//                num_px[i][j] = 0;
//            }
//        }
//
//        val_z = 250 + tt;
//        cout << "val_z:" << val_z << endl;
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//        double newx, newy;
//        int nx, ny;
//
//        const int numThreads = 16;
//        vector<thread> threads;
//        int rowsPerThread = Hpin / numThreads;
//
//        //insert_pixels(0, Hpin, cpd, sp, tp, board, img_save, val_z);
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, cpd, sp, tp, board, img_save, num_px, val_z);
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
//        cout << "calc finished" << endl;
//        finished_threads = 0;
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        //for (int i = 0; i < ph_disp; i++) {
//        //    for (int j = 0; j < pw_disp; j++) {
//        //        img_save.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i][j] / (256 * 256)));
//        //        img_save.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i][j] / 256 - img_save.at<cv::Vec3b>(i, j)[2] * 256));
//        //        img_save.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i][j] - (img_save.at<cv::Vec3b>(i, j)[2] * 256 - img_save.at<cv::Vec3b>(i, j)[1]) * 256));
//        //    }
//        //}
//
//        ostringstream stream;
//        stream << "./checkerboard/real-scene/real-scene_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_val-z" << fixed << setprecision(0) << val_z << ".png";
//
//        cv::String filename = stream.str();
//
//        imwrite(filename, img_save);
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
//
//    for (int i = 0; i < ph_disp; i++) {
//        free(img_display[i]);
//        delete num_px[i];
//    }
//    delete num_px;
//    free(img_display);
//
//    return EXIT_SUCCESS;
//}
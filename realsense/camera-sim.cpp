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
//// 表示系のパラメータ(mm)
//int W = 170, H = 170;           // 表示の縦横幅
//double f = 1450, Wfr = 2.0 * (double)W;    // 焦点距離、撮像面幅
//int N = 578 * 2;                // カメラの解像度
//int Wpin = 3, Hpin = 3;         // 縦横の視点数
//double g = 4, wp = 1.25;
//
//int num_z = 1751;
//
//int WIDTH = 640;
//int HEIGHT = 480;
//int FPS = 30;
//
//double D = 100;                     // 表示系とカメラとの距離(mm)
//
//void insert_pixels(int x, int y, double* sp, double* tp, cv::Mat board, int px_board, double val_z) {
//
//    int ndx, ndy;
//    int startv, startu;
//    double dx, dy, cu, cv, u, v;
//    cv::Mat img_wide = cv::Mat::zeros(cv::Size(N, N), CV_8UC3);
//    cv::Mat img_save = cv::Mat::zeros(cv::Size(int(N / 2), int(N / 2)), CV_8UC3);
//
//    for (int i = 0; i < N; i++) {
//        for (int j = 0; j < N; j++) {
//            img_wide.at<cv::Vec3b>(i, j)[0] = 121;
//            img_wide.at<cv::Vec3b>(i, j)[1] = 77;
//            img_wide.at<cv::Vec3b>(i, j)[2] = 49;
//        }
//    }
//
//    ndx = f / (f + val_z) * sp[y * Wpin + x] / Wfr * N;
//    ndy = f / (f + val_z) * tp[y * Wpin + x] / Wfr * N;
//    //cout << "ndx:" << ndx << ", ndy:" << ndy << endl;
//
//    if (N > px_board) {
//
//        startu = int((N - px_board) / 2);
//        startv = int((N - px_board) / 2);
//
//        for (int i = 0; i < px_board; i++) {
//            for (int j = 0; j < px_board; j++) {
//                if (i + startv + ndy >= 0 && i + startv + ndy < N && j + startu + ndx >= 0 && j + startu + ndx < N){
//                    img_wide.at<cv::Vec3b>(i + startv + ndy, j + startu + ndx)[0] = board.at<cv::Vec3b>(i, j)[0];
//                    img_wide.at<cv::Vec3b>(i + startv + ndy, j + startu + ndx)[1] = board.at<cv::Vec3b>(i, j)[1];
//                    img_wide.at<cv::Vec3b>(i + startv + ndy, j + startu + ndx)[2] = board.at<cv::Vec3b>(i, j)[2];
//                }
//            }
//        }
//    }
//    else {
//
//        startu = int((px_board - N) / 2);
//        startv = int((px_board - N) / 2);
//
//        for (int i = 0; i < N; i++) {
//            for (int j = 0; j < N; j++) {
//                if (i + startv + ndy >= 0 && i + startv + ndy < px_board && j + startu + ndx >= 0 && j + startu + ndx < px_board) {
//                    img_wide.at<cv::Vec3b>(i, j)[0] = board.at<cv::Vec3b>(i + startv + ndy, j + startu + ndx)[0];
//                    img_wide.at<cv::Vec3b>(i, j)[1] = board.at<cv::Vec3b>(i + startv + ndy, j + startu + ndx)[1];
//                    img_wide.at<cv::Vec3b>(i, j)[2] = board.at<cv::Vec3b>(i + startv + ndy, j + startu + ndx)[2];
//                }
//            }
//        }
//
//    }
//
//    if (ndx > 0) {
//        startu = 578;
//    }
//    else if (ndx == 0) {
//        startu = 289;
//    }
//    else {
//        startu = 0;
//    }
//
//    if (ndy > 0) {
//        startv = 578;
//    }
//    else if (ndy == 0) {
//        startv = 289;
//    }
//    else {
//        startv = 0;
//    }
//
//    //cout << "startu:" << startu << ", startv:" << startv << endl;
//    for (int i = 0; i < int(N / 2); i++) {
//        for (int j = 0; j < int(N / 2); j++) {
//            img_save.at<cv::Vec3b>(i, j)[0] = img_wide.at<cv::Vec3b>(i + startv, j + startu)[0];
//            img_save.at<cv::Vec3b>(i, j)[1] = img_wide.at<cv::Vec3b>(i + startv, j + startu)[1];
//            img_save.at<cv::Vec3b>(i, j)[2] = img_wide.at<cv::Vec3b>(i + startv, j + startu)[2];
//        }
//    }
//
//    int idx = y * Wpin + x;
//    ostringstream stream;
//    stream << "./checkerboard/direct-scene/img_view" << idx << "/direct-scene_img_view" << idx << "_val-z"  << fixed << setprecision(0) << val_z << ".png";
//
//    cv::String filename = stream.str();
//    imwrite(filename, img_save);
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
//    double pitch = (double)W / (double)(Wpin - 1);
//    cout << "pitch:" << pitch << endl;
//
//    // 視点パラメータ(mm)
//    double* sp;
//    double* tp;
//    sp = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    tp = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    for (int i = 0; i < Hpin; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            sp[i * Wpin + j] = (j - (Wpin - 1) * 0.5) * pitch;
//            tp[i * Wpin + j] = (i - (Hpin - 1) * 0.5) * pitch;
//
//            //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//        }
//    }
//
//    double val_z;
//    cv::Mat row_board = cv::imread("C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/checkerboard.bmp");
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
//
//        val_z = 250 + tt;
//        cout << "val_z:" << val_z << endl;
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        double Wimg = f / (val_z + f) * val_z / g * wp;
//        int px_board = Wimg / Wfr * N;
//        cv::Mat board = cv::Mat::zeros(cv::Size(px_board, px_board), CV_8UC3);
//        cv::resize(row_board, board, cv::Size(), (double)px_board / row_board.cols, (double)px_board / row_board.rows, cv::INTER_NEAREST);
//
//        //insert_pixels(0, Hpin, cpd, sp, tp, board, img_save, val_z);
//
//        const int numThreads = 9;
//        vector<thread> threads;
//
//        int idx_x, idx_y;
//        for (int i = 0; i < numThreads; i++) {
//            idx_x = i % Wpin;
//            idx_y = int(i / Wpin);
//            //cout << "idx_x:" << idx_x << "idx_y:" << idx_y << endl;
//            threads.emplace_back(insert_pixels, idx_x, idx_y, sp, tp, board, px_board, val_z);
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
//    return EXIT_SUCCESS;
//}
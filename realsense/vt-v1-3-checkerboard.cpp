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
//int num_z = 1751;
//
//void insert_pixels(int start, int end, int num_row, double* sp, double* tp, double** data, int** img_display, double** val_z) {
//
//    double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//    double newx, newy;
//    int nx, ny;
//    int startv, startu;
//    for (int k = 0; k < num_row; k++) {
//
//        tmp_pcd_x = data[k][0];
//        tmp_pcd_y = data[k][1];
//        tmp_pcd_z = data[k][2];
//        tmp_pcd_b = data[k][3];
//        tmp_pcd_g = data[k][4];
//        tmp_pcd_r = data[k][5];
//
//        for (int i = start; i < end; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                //cout << "i:" << i << ", j:" << j << endl;
//
//                newx = (g / tmp_pcd_z) * (tmp_pcd_x - sp[i * Wpin + j]);
//                newy = (g / tmp_pcd_z) * (tmp_pcd_y - tp[i * Wpin + j]);
//
//                nx = static_cast<int>(floor(newx / pd) + N * 0.5);
//                ny = static_cast<int>(floor(newy / pd) + N * 0.5);
//
//                startv = static_cast<int>(i * intv);
//                startu = static_cast<int>(j * intv);
//
//                //cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << endl;
//                //cout << "nx:" << nx << ", ny:" << ny << endl;
//
//                if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                    if (val_z[startv + ny][startu + nx] > tmp_pcd_z) {
//
//                        img_display[startv + ny][startu + nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256;
//                        val_z[startv + ny][startu + nx] = tmp_pcd_z;
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
//
//}
//
//
//int main(int argc, char* argv[])
//{
//
//    std::cout << "vt-v1-3-checkerboard" << endl;
//    std::cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    std::cout << "wp:" << wp << endl;
//
//    // 視点パラメータ(mm)
//    double* sp;
//    double* tp;
//    sp = new double[Hpin * Wpin];
//    tp = new double[Hpin * Wpin];
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
//    int** img_display;
//    double** val_z;
//    img_display = new int*[ph_disp];
//    val_z = new double*[ph_disp];
//    for (int i = 0; i < ph_disp; i++) {
//        img_display[i] = new int[pw_disp];
//        val_z[i] = new double[pw_disp];
//    }
//
//    cv::Mat board = cv::imread("C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/checkerboard.bmp");
//    double cpd = wp / (double)board.rows;
//    std::cout << "cpd:" << cpd << ", pd:" << pd << endl;
//
//    double** data;
//    data = new double*[board.rows * board.cols];
//    for (int i = 0; i < board.rows * board.cols; i++) {
//        data[i] = new double[6];
//    }
//
//    //for (int i = 0; i < board.rows; i++) {
//    //    for (int j = 0; j < board.cols; j++) {
//    //        std::cout << "i:" << i << ", j:" << ", board:" << board.at<cv::Vec3b>(i, j) << endl;
//    //    }
//    //}
//
//    double x, y, z, cv, cu;
//    int num_row = board.rows * board.cols;
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
//                val_z[i][j] = DBL_MAX;
//            }
//        }
//
//        // dataをオフセット
//        for (int i = 0; i < num_row; i++) {
//            data[i][0] = 0;
//            data[i][1] = 0;
//            data[i][2] = 0;
//            data[i][3] = 0;
//            data[i][4] = 0;
//            data[i][5] = 0;
//        }
//
//        for (int i = 0; i < board.rows; i++) {
//            for (int j = 0; j < board.cols; j++) {
//
//
//                //std::cout << "i:" << i << ", j:" << ", board:" << board.at<cv::Vec3b>(i, j) << endl;
//
//                cu = ((double)j - (board.cols - 1) * 0.5) * cpd;
//                cv = ((double)i - (board.rows - 1) * 0.5) * cpd;
//                //std::cout << "cu:" << cu << ", cv:" << cv << endl;
//
//                data[i * board.cols + j][0] = z / g * cu;
//                data[i * board.cols + j][1] = z / g * cv;
//                data[i * board.cols + j][2] = z;
//                data[i * board.cols + j][3] = board.at<cv::Vec3b>(i, j)[0];
//                data[i * board.cols + j][4] = board.at<cv::Vec3b>(i, j)[1];
//                data[i * board.cols + j][5] = board.at<cv::Vec3b>(i, j)[2];
//
//            }
//        }
//
//        //insert_pixels(0, Hpin, uo, vo, std::ref(data), std::ref(img_display), val_z);
//
//        const int numThreads = 16;
//        vector<thread> threads;
//        int rowsPerThread = Hpin / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, num_row, sp, tp, std::ref(data), std::ref(img_display), val_z);
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
//        stream << "./checkerboard/vt-v1-3/vt-v1-3-pdBase-checkerboard_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_val-z" << fixed << setprecision(0) << z << ".png";
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
//    for (int i = 0; i < board.rows * board.cols; i++) {
//        delete data[i];
//    }
//    delete data;
//
//    for (int i = 0; i < ph_disp; i++) {
//        delete img_display[i];
//        delete val_z[i];
//    }
//    delete img_display;
//    delete val_z;
//    delete sp;
//    delete tp;
//
//    return EXIT_SUCCESS;
//}

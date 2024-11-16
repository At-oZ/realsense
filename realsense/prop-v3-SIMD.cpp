///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* x, y軸方向にそれぞれテーブルを作る */
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
//#include <xmmintrin.h>
////#include <X11/Xlib.h>
////#include <X11/Xutil.h>
////
//// #include </opt/homebrew/Cellar/gcc/13.2.0/lib/gcc/current/gcc/aarch64-apple-darwin22/13/include/omp.h>
//
//using namespace std;
//using namespace cv;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//// 表示系のパラメータ(mm)
//int display_px = 2400;  // 表示領域の画素数
//int Npin = 40; // ピンホール数
//int N = display_px / Npin;  // 要素画像の解像度
//double ld = 13.4 * 25.4;    // ディスプレイサイズ
//int pw = 3840, ph = 2400;   // ディスプレイの縦横の解像度
//double pd = ld / sqrtf(pw * pw + ph * ph);  // 画素ピッチ
//double W = display_px * pd, H = display_px * pd;    // 表示の縦横幅
//double g = 16.0; // ギャップ
//double wp = W / (double)Npin;   // ピッチ
//int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp)); // 縦横のピンホール数
//
//int WIDTH = 640;
//int HEIGHT = 480;
//int FPS = 30;
//
//// 点群データ配列の行数と列数
//int rows = WIDTH * HEIGHT;
//int cols = 6;
//
//double D = 100;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//int TIMES = 50;
//
//// ファイル読み込み
//// 引数で配列のポインタを受け取りそこに値を書き込み
//int fileread(double* test, string fname) {
//    ifstream fin(fname);
//    if (!fin) {
//        std::cout << "ファイルをオープンできませんでした。\n";
//        getchar();
//        exit(0);		// プログラムの終了
//    }
//    else std::cout << "ファイルをオープンしました。\n";
//
//    // eofが検出されるまで配列に書き込む
//    int i = 1;
//    while (1) {
//        fin >> test[i];
//        if (fin.eof())	break;
//        i++;
//    }
//    fin.close();
//    std::cout << "ファイルをクローズしました。\n";
//
//    return i;   // 配列の長さを返す
//}
//
//void insert_pixels(int start, int end, double** data, double** red, double** green, double** blue, bool** alpha, double** sum_coef, double* sp, double* tp) {
//
//    double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r, tmp_coef;
//
//    //対応点パラメータ(mm)
//    int* up;
//    int* vp;
//    up = (int*)calloc(Wpin, sizeof(int));
//    vp = (int*)calloc(Hpin, sizeof(int));
//
//    //点群を箱に格納
//    for (int k = 0; k < rows; k++) {
//
//        tmp_pcd_x = data[k][0] * 1000.0;
//        tmp_pcd_y = data[k][1] * 1000.0;
//        tmp_pcd_z = data[k][2] * 1000.0 + D;
//        tmp_pcd_b = data[k][3];
//        tmp_pcd_g = data[k][4];
//        tmp_pcd_r = data[k][5];
//        tmp_coef = 1000000.0 / (tmp_pcd_z * tmp_pcd_z);
//
//        for (int i = 0; i < Wpin; i++) {
//            up[i] = static_cast<int>(round((g / tmp_pcd_z) * (tmp_pcd_x - sp[i]) / pd) + N * 0.5);
//        }
//        for (int i = start; i < end; i++) {
//            vp[i] = static_cast<int>(round((g / tmp_pcd_z) * (tmp_pcd_y - tp[i]) / pd) + N * 0.5);
//        }
//
//        for (int i = start; i < end; i++) {
//            if (vp[i] >= 0 && vp[i] < N) {
//                for (int j = 0; j < Wpin; j++) {
//                    if (up[j] >= 0 && up[j] < N) {
//                        //std::cout << "vp:" << vp[i] << ", up:" << up[j] << std::endl;
//
//                        red[N * i + vp[i]][N * j + up[j]] += tmp_pcd_r * tmp_coef;
//                        green[N * i + vp[i]][N * j + up[j]] += tmp_pcd_g * tmp_coef;
//                        blue[N * i + vp[i]][N * j + up[j]] += tmp_pcd_b * tmp_coef;
//                        sum_coef[N * i + vp[i]][N * j + up[j]] += tmp_coef;
//                        alpha[N * i + vp[i]][N * j + up[j]] = true;
//                    }
//                }
//            }
//        }
//    }
//
//    std::free(up);
//    std::free(vp);
//
//    // スレッドの終了をカウントし、条件変数を通知する
//    {
//        std::lock_guard<std::mutex> lock(mtx);
//        finished_threads++;
//        conv.notify_one(); // 1つの待機スレッドに通知
//    }
//}
//
//int main(int argc, char* argv[])
//{
//
//    std::cout << "prop-v3" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    std::cout << "TIMES:" << TIMES << endl;
//
//    //視点パラメータ(mm)
//    double* sp;
//    double* tp;
//    sp = (double*)malloc(sizeof(double) * Wpin);
//    tp = (double*)malloc(sizeof(double) * Hpin);
//    for (int i = 0; i < Wpin; i++) {
//        sp[i] = (i - (Wpin - 1) * 0.5) * wp;
//    }
//    for (int i = 0; i < Wpin; i++) {
//        tp[i] = (i - (Hpin - 1) * 0.5) * wp;
//    }
//
//    // CSVファイルのパス
//    std::string filePath = "output.csv";
//
//    // double型の二次元配列を動的に確保
//    double** data = new double* [rows];
//    for (int i = 0; i < rows; ++i) {
//        data[i] = new double[cols];
//    }
//
//    // ファイルを開く
//    std::ifstream file(filePath);
//
//    // ファイルが正常に開かれたか確認
//    if (file.is_open()) {
//        std::string line;
//        int row = 0;
//        // ファイルから一行ずつ読み込む
//        while (getline(file, line) && row < rows) {
//            std::stringstream ss(line);
//            std::string value;
//            int col = 0;
//            while (getline(ss, value, ',') && col < cols) {
//                // 文字列をdouble型に変換して配列に格納
//                data[row][col++] = std::stod(value);
//            }
//            ++row;
//        }
//        file.close(); // ファイルを閉じる
//    }
//    else {
//        std::cerr << "ファイルを開けませんでした。" << std::endl;
//        // メモリリークを防ぐためにメモリを解放
//        for (int i = 0; i < rows; ++i) {
//            delete[] data[i];
//        }
//        delete[] data;
//        return 1; // エラーコード1で終了
//    }
//
//    cv::Mat img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
//
//    double** red;
//    double** green;
//    double** blue;
//    bool** alpha;
//    double** sum_coef;
//
//    red = (double**)calloc(display_px, sizeof(double*));
//    green = (double**)calloc(display_px, sizeof(double*));
//    blue = (double**)calloc(display_px, sizeof(double*));
//    alpha = (bool**)calloc(display_px, sizeof(bool*));
//    sum_coef = (double**)calloc(display_px, sizeof(double*));
//
//    for (int i = 0; i < display_px; i++) {
//        red[i] = (double*)calloc(display_px, sizeof(double));
//        green[i] = (double*)calloc(display_px, sizeof(double));
//        blue[i] = (double*)calloc(display_px, sizeof(double));
//        alpha[i] = (bool*)calloc(display_px, sizeof(bool));
//        sum_coef[i] = (double*)calloc(display_px, sizeof(double));
//    }
//
//    double sum_time = 0;
//    // フレーム処理
//    for (int tt = 0; tt < TIMES; tt++) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        for (int i = 0; i < display_px; i++) {
//            for (int j = 0; j < display_px; j++) {
//
//                img_display.at<cv::Vec3b>(i, j)[0] = 0;
//                img_display.at<cv::Vec3b>(i, j)[1] = 0;
//                img_display.at<cv::Vec3b>(i, j)[2] = 0;
//
//                red[i][j] = 0;
//                green[i][j] = 0;
//                blue[i][j] = 0;
//                sum_coef[i][j] = 0;
//                alpha[i][j] = false;
//
//            }
//        }
//
//        double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double tmp_xt, tmp_yt, tmp_zt, tmp_coef;
//        int tmp_nx, tmp_ny, tmp_nz;
//        int pcd_count = 0;
//
//        //insert_pixels(0, Hpin, data, red, green, blue, alpha, sum_coef, sp, tp);
//
//        const int numThreads = 20;
//        vector<thread> threads;
//        int rowsPerThread = Hpin / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, data, red, green, blue, alpha, sum_coef, sp, tp);
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
//        for (int i = 0; i < display_px; i++) {
//            for (int j = 0; j < display_px; j++) {
//                if (alpha[i][j]) {
//                    img_display.at<cv::Vec3b>(i, j)[0] = blue[i][j] / sum_coef[i][j];
//                    img_display.at<cv::Vec3b>(i, j)[1] = green[i][j] / sum_coef[i][j];
//                    img_display.at<cv::Vec3b>(i, j)[2] = red[i][j] / sum_coef[i][j];
//                }
//            }
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
//        //std::cout << "重なり回数:" << pcd_count << endl;
//        sum_time += duration.count();
//
//        // 表示画像の保存
//        ostringstream stream;
//        stream << "./images/prop-v3_ImgDisplay.png";
//        cv::String filename = stream.str();
//        imwrite(filename, img_display);
//
//        stream.str("");
//        stream.clear(ostringstream::goodbit);
//
//        //// 各イメージプレーンの画像を保存（テスト用）
//        //// ostringstream stream;
//        //// cv::String filename;
//        //for (int zi = 0; zi < num_z_level; zi++) {
//
//        //    stream << "./images/imagePlane/prop-v1-10/ptimes" << fixed << setprecision(2) << ptimes <<  "/prop - v1 - 10_imagePlane_NumZLevel" << num_z_level << "_zi" << zi << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << ".png";
//        //    std::cout << "zi:" << zi << endl;
//
//        //    for (int i = 0; i < px_height_img; i++) {
//        //        for (int j = 0; j < px_width_img; j++) {
//        //            if (alpha[zi][i][j]) {
//        //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
//        //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
//        //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
//        //            }
//        //        }
//        //    }
//        //    filename = stream.str();
//        //    imwrite(filename, img);
//        //    stream.str("");
//        //    stream.clear(ostringstream::goodbit);
//        //    img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//        //}
//
//    }
//
//    std::cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//
//    // 使用したメモリを解放
//    for (int i = 0; i < rows; ++i) {
//        delete[] data[i];
//    }
//    delete[] data;
//
//    for (int i = 0; i < display_px; i++) {
//        std::free(red[i]);
//        std::free(green[i]);
//        std::free(blue[i]);
//        std::free(alpha[i]);
//        std::free(sum_coef[i]);
//    }
//    std::free(red);
//    std::free(green);
//    std::free(blue);
//    std::free(alpha);
//    std::free(sum_coef);
//
//    std::free(sp);
//    std::free(tp);
//
//    //for (int i = 0; i < N; i++) {
//    //    std::free(xt[i]);
//    //    std::free(yt[i]);
//    //}
//    //std::free(xt);
//    //std::free(yt);
//
//    return EXIT_SUCCESS;
//}
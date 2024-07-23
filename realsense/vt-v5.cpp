///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* v4-2との差分：基準カメラを縦横2x2の4箇所に配置し、それをもとに視差を用いて要素画像群を埋める */
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
//// #include </opt/homebrew/Cellar/gcc/13.2.0/lib/gcc/current/gcc/aarch64-apple-darwin22/13/include/omp.h>
//
//using namespace std;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
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
//int interp_intv = 9;
//int range = 3;
//int num_ref_view = 2;
//double inv_num_ref_view = 1.0 / num_ref_view;
//
//// ファイル読み込み
//// 引数で配列のポインタを受け取りそこに値を書き込み
//int fileread(double* test, string fname) {
//	ifstream fin(fname);
//	if (!fin) {							
//		cout << "ファイルをオープンできませんでした。\n";
//		getchar();		
//		exit(0);		// プログラムの終了
//	}
//	else cout << "ファイルをオープンしました。\n";
//
//	// eofが検出されるまで配列に書き込む
//	int i = 1;
//	while (1) {
//		fin >> test[i];
//		if (fin.eof())	break;
//		i++;
//	}
//	fin.close();
//	cout << "ファイルをクローズしました。\n";
//
//	return i;	                // 配列の長さを返す
//}
//
//void insert_pixels(int WIDTH, int HEIGHT, int N, double g, double intv, int starty, int endy, int* ref_pos_x, int* ref_pos_y, int* img, cv::Mat img_display, double* val_z) {
//
//    int nx, ny;
//    int startv, startu;
//    int pcd_size = WIDTH * HEIGHT;
//    int inv_WIDTH = 1.0 / WIDTH;
//    int ref_idx_x, ref_idx_y;
//    // cv::Mat img_elem;
//
//    for (int i = starty; i < endy; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            // img_elem = cv::Mat::zeros(cv::Size(N * range, N * range), CV_8UC3);
//            startv = static_cast<int>(i * intv);
//            startu = static_cast<int>(j * intv);
//
//            ref_idx_x = static_cast<int>(floor(j / (Wpin * inv_num_ref_view)));
//            ref_idx_y = static_cast<int>(floor(i / (Hpin * inv_num_ref_view)));
//
//            // cout << "ref_idx x:" << ref_idx_x << ", y:" << ref_idx_y << endl;
//
//            for (int k = 0; k < N * range; k++) {
//                for (int l = 0; l < N * range; l++) {
//
//                    // cout << "horizontal disparity:" << static_cast<int>(floor((j + 0.5 - Wpin / 2) * (wp * g / (pd * val_z[k][l] * 1000)))) << endl;
//                    // cout << "vartical disparity:" << static_cast<int>(floor((i + 0.5 - Wpin / 2) * (wp * g / (pd * val_z[k][l] * 1000)))) << endl;
//
//                    nx = l - static_cast<int>(floor((j - ref_pos_x[ref_idx_y * num_ref_view + ref_idx_x]) * (wp * g / (pd * val_z[((ref_idx_y * num_ref_view + ref_idx_x) * (N * range) + k) * (N * range) + l] * 1000))));
//                    ny = k - static_cast<int>(floor((i - ref_pos_y[ref_idx_y * num_ref_view + ref_idx_x]) * (wp * g / (pd * val_z[((ref_idx_y * num_ref_view + ref_idx_x) * (N * range) + k) * (N * range) + l] * 1000))));
//
//                    // if (0 <= nx && nx < N * range && 0 <= ny && ny < N * range) {
//                    //     img_elem.at<cv::Vec3b>(ny, nx)[0] = img.at<cv::Vec3b>(l, k)[0];
//                    //     img_elem.at<cv::Vec3b>(ny, nx)[1] = img.at<cv::Vec3b>(l, k)[1];
//                    //     img_elem.at<cv::Vec3b>(ny, nx)[2] = img.at<cv::Vec3b>(l, k)[2];
//                    // }
//
//                    if (N * (range - 1) * 0.5 <= nx && nx < N * (range + 1) * 0.5 && N * (range - 1) * 0.5 <= ny && ny < N * (range + 1) * 0.5) {
//
//                            nx -= N * (range - 1) * 0.5;
//                            ny -= N * (range - 1) * 0.5;
//
//                            // img_display.at<cv::Vec3b>(startv + ny, startu + nx)[0] = img.at<cv::Vec3b>(l, k)[0];
//                            // img_display.at<cv::Vec3b>(startv + ny, startu + nx)[1] = img.at<cv::Vec3b>(l, k)[1];
//                            // img_display.at<cv::Vec3b>(startv + ny, startu + nx)[2] = img.at<cv::Vec3b>(l, k)[2];
//
//                            img_display.at<cv::Vec3b>(startv + ny, startu + nx)[0] = img[(((ref_idx_y * num_ref_view + ref_idx_x) * (N * range) + k) * (N * range) + l) * 3];
//                            img_display.at<cv::Vec3b>(startv + ny, startu + nx)[1] = img[(((ref_idx_y * num_ref_view + ref_idx_x) * (N * range) + k) * (N * range) + l) * 3 + 1];
//                            img_display.at<cv::Vec3b>(startv + ny, startu + nx)[2] = img[(((ref_idx_y * num_ref_view + ref_idx_x) * (N * range) + k) * (N * range) + l) * 3 + 2];
//                    }
//                }
//            }
//
//            // for (int k = 0; k < N; k++) {
//            //     for (int l = 0; l < N; l++) {
//            //                 img_display.at<cv::Vec3b>(startv + l, startu + k)[0] = img_elem.at<cv::Vec3b>(N * (range - 1) * 0.5 + l, N * (range - 1) * 0.5 + k)[0];
//            //                 img_display.at<cv::Vec3b>(startv + l, startu + k)[1] = img_elem.at<cv::Vec3b>(N * (range - 1) * 0.5 + l, N * (range - 1) * 0.5 + k)[1];
//            //                 img_display.at<cv::Vec3b>(startv + l, startu + k)[2] = img_elem.at<cv::Vec3b>(N * (range - 1) * 0.5 + l, N * (range - 1) * 0.5 + k)[2];
//
//            //     }
//            // }
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
//    cout << "vt-v5" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    cout << "wp:" << wp << endl;
//    cout << "N:" << N << endl;
//    cout << "Wpin:" << Wpin << ", Hpin:" << Hpin << endl;
//
//    // 要素画像群
//    //vector<vector<cv::Mat>> elem_img(Hpin, vector<cv::Mat>(Wpin));
//    // int* elem_img;
//    // elem_img = (int*)calloc(Hpin * Wpin * N * N * 3, sizeof(int));
//
//    // 視点パラメータ(mm)
//    // vector<vector<double>> uo(Hpin, vector<double>(Wpin));
//    // vector<vector<double>> vo(Hpin, vector<double>(Wpin));
//
//    double* uo;
//    double* vo;
//    uo = (double*)malloc(sizeof(double) * (num_ref_view) * (num_ref_view));
//    vo = (double*)malloc(sizeof(double) * (num_ref_view) * (num_ref_view));
//
//    int* ref_pos_x;
//    int* ref_pos_y;
//    ref_pos_x = (int*)malloc(sizeof(int) * num_ref_view * num_ref_view);
//    ref_pos_y = (int*)malloc(sizeof(int) * num_ref_view * num_ref_view);
//
//    double intv_ref_pos_x = Wpin * inv_num_ref_view * wp;
//    double intv_ref_pos_y = Hpin * inv_num_ref_view * wp;
//    double std_ref_pos_x = -Wpin * 0.5 * wp * (1 + inv_num_ref_view);
//    double std_ref_pos_y = -Hpin * 0.5 * wp * (1 + inv_num_ref_view);
//
//    for (int i = 0; i < num_ref_view; i++) {
//        for (int j = 0; j < num_ref_view; j++) {
//
//            uo[i * num_ref_view + j] = std_ref_pos_x + intv_ref_pos_x * j;
//            vo[i * num_ref_view + j] = std_ref_pos_y + intv_ref_pos_y * i;
//
//            ref_pos_x[i * num_ref_view + j] = static_cast<int>((j + 1) * (inv_num_ref_view * Wpin - 1) * 0.5);
//            ref_pos_y[i * num_ref_view + j] = static_cast<int>((i + 1) * (inv_num_ref_view * Hpin - 1) * 0.5);
//
//            cout << "ref_pos x:" << ref_pos_x[i * num_ref_view + j] << ", y:" << ref_pos_y[i * num_ref_view + j] << endl;
//        }
//    }
//
//    cout << "ok1" << endl;
//
//    double* val_z;
//    val_z = (double*)malloc(sizeof(double) * num_ref_view * num_ref_view * (N * range) * (N * range));
//    // for (int i = 0; i < num_ref_view; i++) {
//
//    //     val_z[i] = (double***)malloc(sizeof(double**) * num_ref_view);
//
//    //     for (int j = 0; j < num_ref_view; j++) {
//
//    //         val_z[i][j] = (double**)malloc(sizeof(double*) * N * range);
//
//    //         for (int k = 0; k < N * range; k++) {
//
//    //             val_z[i][j][k] = (double*)malloc(sizeof(double) * N * range);
//    //         }
//    //     }
//    // }
//
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//
//    int D = 100;                      // 表示系とカメラとの距離(mm)
//    double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//    double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//    double inv_pd = 1.0 / pd;         // pdの逆数
//
//    // double* nu;
//    // double* nv;
//    // nu = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
//    // nv = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
//    
//    // CSVファイルのパス
//    std::string filePath = "output_v3.csv";
//
//    // 行数と列数
//    int rows = WIDTH * HEIGHT; // 例として10行
//    int cols = 6; // 例として5列
//
//    // double型の二次元配列を動的に確保
//    double** data = new double*[rows];
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
//    } else {
//        std::cerr << "ファイルを開けませんでした。" << std::endl;
//        // メモリリークを防ぐためにメモリを解放
//        for (int i = 0; i < rows; ++i) {
//            delete[] data[i];
//        }
//        delete[] data;
//        return 1; // エラーコード1で終了
//    }
//
//    cout << "ok2" << endl;
//
//    cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    cv::Mat img_ref = cv::Mat::zeros(cv::Size(N * range * num_ref_view, N * range * num_ref_view), CV_8UC3);
//    int* img;
//    img = (int*)malloc(sizeof(int) * (num_ref_view) * (num_ref_view) * (N * range) * (N * range) * 3);
//
//    // フレーム処理
//    for (int tt = 0; tt < 1; tt++) {
//
//        for (int i = 0; i < num_ref_view; i++) {
//            for (int j = 0; j < num_ref_view; j++) {
//                for (int k = 0; k < N * range; k++) {
//                    for (int l = 0; l < N * range; l++) {
//                        val_z[((i * num_ref_view + j) * (N * range) + k) * (N * range) + l] = 10;
//                    }
//                }
//            }
//        }
//
//        // // 計測開始時間
//        // //auto start = chrono::high_resolution_clock::now();
//        // cpu_time = clock();
//        // start = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//        double newx, newy;
//        int nx, ny;
//
//        for (int k = 0; k < WIDTH * HEIGHT; k++) {
//
//            tmp_pcd0 = data[k][0];
//            tmp_pcd1 = data[k][1];
//            tmp_pcd2 = data[k][2];
//            tmp_pcd3 = data[k][3];
//            tmp_pcd4 = data[k][4];
//            tmp_pcd5 = data[k][5];
//
//            inv_d = 1.0 / (tmp_pcd2 * 1000 + D);
//
//            for (int i = 0; i < num_ref_view; i++) {
//                for (int j = 0; j < num_ref_view; j++) {
//
//                    newx = -(g * inv_d) * (uo[i * num_ref_view + j] - tmp_pcd0 * 1000);
//                    newy = -(g * inv_d) * (vo[i * num_ref_view + j] - tmp_pcd1 * 1000);
//
//                    nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//                    ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//                    // cout << "i:" << i << ", j:" << j << endl;
//                    
//                    if (0 <= nx && nx < N * range && 0 <= ny && ny < N * range) {
//                        if (val_z[((i * num_ref_view + j) * (N * range) + ny) * (N * range) + nx] > tmp_pcd2) {
//
//                            // img.at<cv::Vec3b>(ny, nx)[0] = static_cast<int>(tmp_pcd3);
//                            // img.at<cv::Vec3b>(ny, nx)[1] = static_cast<int>(tmp_pcd4);
//                            // img.at<cv::Vec3b>(ny, nx)[2] = static_cast<int>(tmp_pcd5);
//
//                            img[(((i * (num_ref_view) + j) * (N * range) + ny) * (N * range) + nx) * 3] = static_cast<int>(tmp_pcd3);
//                            img[(((i * (num_ref_view) + j) * (N * range) + ny) * (N * range) + nx) * 3 + 1] = static_cast<int>(tmp_pcd4);
//                            img[(((i * (num_ref_view) + j) * (N * range) + ny) * (N * range) + nx) * 3 + 2] = static_cast<int>(tmp_pcd5);
//
//                            val_z[((i * num_ref_view + j) * (N * range) + ny) * (N * range) + nx] = tmp_pcd2;
//                        }
//                    }
//                }
//            }
//        }
//
//        cout << "ok3" << endl;
//
//        for (int i = 0; i < num_ref_view; i++) {
//            for (int j = 0; j < num_ref_view; j++) {
//                for (int k = 0; k < (N * range); k++) {
//                    for (int l = 0; l < (N * range); l++) {
//
//                        img_ref.at<cv::Vec3b>(i * (N * range) + k, j * (N * range) + l)[0] = img[(((i * (num_ref_view) + j) * (N * range) + k) * (N * range) + l) * 3];
//                        img_ref.at<cv::Vec3b>(i * (N * range) + k, j * (N * range) + l)[1] = img[(((i * (num_ref_view) + j) * (N * range) + k) * (N * range) + l) * 3 + 1];
//                        img_ref.at<cv::Vec3b>(i * (N * range) + k, j * (N * range) + l)[2] = img[(((i * (num_ref_view) + j) * (N * range) + k) * (N * range) + l) * 3 + 2];
//
//                    }
//                }
//            }
//        }
//
//        cout << "ok4" << endl;
//
//        const int numThreads = 8;
//        vector<thread> threads;
//        int rowsPerThread = Hpin / numThreads;
//
//        insert_pixels(WIDTH, HEIGHT, N, g, intv, 0, Hpin, std::ref(ref_pos_x), std::ref(ref_pos_y), std::ref(img), std::ref(img_display), std::ref(val_z));
//
//        // int startRow, endRow;
//        // for (int i = 0; i < numThreads; i++) {
//        //     startRow = i * rowsPerThread;
//        //     endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//        //     threads.emplace_back(insert_pixels, WIDTH, HEIGHT, N, g, intv, startRow, endRow, std::ref(img), std::ref(img_display), val_z);
//        // }
//
//        // for (auto& t : threads) {
//        //     if(t.joinable()){t.join();}
//        // }
//
//        // // 全てのスレッドが終了するのを待つ
//        // {
//        //     std::unique_lock<std::mutex> lock(mtx);
//        //     conv.wait(lock, []{return finished_threads == numThreads;});
//        // }
//
//        cout << "calc finished" << endl;
//        finished_threads = 0;
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
//        // // 計測修了時間
//        // //auto finish = chrono::high_resolution_clock::now();
//        // cpu_time = clock();
//        // finish = (double)cpu_time / (double)CLOCKS_PER_SEC;
//
//        // // 経過時間を出力
//        // //chrono::duration<double> elapsed = finish - start;
//        // //cout << "Time to fill the array: " << elapsed.count() << " seconds" << endl;
//
//        // cout << "Time to fill the array: " << finish - start << " seconds" << endl;
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        ostringstream stream;
//        stream << "mac_v5_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png"; // �����_�ȉ�2���Ő؂�̂�
//        cv::String filename = stream.str();
//
//        imwrite(filename, img_ref);
//
//        // 画像を表示
//        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        cv::imshow("Display window", img_ref);
//
//        // キーボードの入力を待つ（1ms）
//        // 何かしらのキーが押されたらループを抜ける
//        cv::waitKey(0);
//        cv::destroyWindow("Display window");
//
//    }
//
//    // free(elem_img);
//    // free(uo);
//    // free(vo);
//    // for (int i = 0; i < num_ref_view; i++) {
//    //     for (int j = 0; j < num_ref_view; j++) {
//    //         for (int k = 0; k < N * range; k++) {
//    //             free(val_z[i][j][k]);
//    //         }
//    //         free(val_z[i][j]);
//    //     }
//    //     free(val_z[i]);
//    // }
//    free(val_z);
//    //free(raw_pointclouddata);
//    //free(pointclouddata);
//    // 使用したメモリを解放
//    for (int i = 0; i < rows; ++i) {
//        delete[] data[i];
//    }
//    delete[] data;
//
//    free(img);
//    free(ref_pos_x);
//    free(ref_pos_y);
//
//    // ウィンドウを閉じる
//    cv::destroyAllWindows();
//
//
//    return EXIT_SUCCESS;
//}
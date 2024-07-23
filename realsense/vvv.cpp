///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* 差分：注目画素の周辺の探索およびイメージプレーンの解像度の変更を可能に */
//
//#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
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
////#include <X11/Xlib.h>
////#include <X11/Xutil.h>
//
//// #include </opt/homebrew/Cellar/gcc/13.2.0/lib/gcc/current/gcc/aarch64-apple-darwin22/13/include/omp.h>
//
//// const double PI = 3.14159;
//const double Htan = 1.02117; // tangent of Horizontal Field Of View (HFOV = 91.2')
//const double Vtan = 0.64322; // tangent of Vertical Field Of View (VFOV = 65.5')
//
//using namespace std;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//// 表示系のパラメータ(mm)
//const int W = 170, H = 170;                                                                       // 表示の縦横幅
//const double g = 4.0, wp = 1.25, a = 0.125;                                                          // ギャップ、ピッチ、ピンホール幅
//const double ld = 13.4 * 25.4;                                                                    // ディスプレイサイズ
//const int pw = 3840, ph = 2400;                                                                   // ディスプレイの縦横の解像度
//const double pd = ld / sqrtf(pw * pw + ph * ph);                                                  // 画素ピッチ
//const int N = static_cast<int>(floor(wp / pd));                                                   // 要素画像の解像度
//const int pw_disp = static_cast<int>(floor(W / pd)), ph_disp = static_cast<int>(floor(H / pd));   // 表示画像の縦横の解像度
//const int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp));         // 縦横のピンホール数
//const double intv = wp / pd;                                                                      // 要素画像の間隔
//
//const int WIDTH = 640;
//const int HEIGHT = 480;
//const int FPS = 30;
//
//const double D = 100.0;                     // 表示系とカメラとの距離(mm)
//const double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//const double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//const double inv_HEIGHT = 1.0 / HEIGHT;
//const double inv_pd = 1.0 / pd;         // pdの逆数
//
//const double max_z = 0.5;
//const int num_z_level = 100;
//const double base = pow(max_z * 1000.0, 1.0 / num_z_level);
//const int bsize = 2; // 注目画素を中心に点を探索する範囲(e.g. AxA画素なら bsize = (A - 1) / 2)
//
//// イメージプレーンの解像度
//const int px_width_img = WIDTH / 4;
//const int px_height_img = HEIGHT / 4;
//
//// ファイル読み込み
//// 引数で配列のポインタを受け取りそこに値を書き込み
//int fileread(double* test, string fname) {
//    ifstream fin(fname);
//    if (!fin) {
//        cout << "ファイルをオープンできませんでした。\n";
//        getchar();
//        exit(0);		// プログラムの終了
//    }
//    else cout << "ファイルをオープンしました。\n";
//
//    // eofが検出されるまで配列に書き込む
//    int i = 1;
//    while (1) {
//        fin >> test[i];
//        if (fin.eof())	break;
//        i++;
//    }
//    fin.close();
//    cout << "ファイルをクローズしました。\n";
//
//    return i;   // 配列の長さを返す
//}
//
//void insert_pixels(int start, int end, int** img_display, int*** pcd_box, int*** x_img, int*** y_img, int* startu, int* startv) {
//
//    int ximg, yimg, tmp_pcd_color, sum_zi;
//    int alpha = 256 * 256 * 256;
//    bool flag1 = false;
//    // bool flag2 = false;
//
//    // 各要素画像の各画素ごとに
//    sum_zi = 0;
//    int sum_pcd_r = 0;
//    int sum_pcd_g = 0;
//    int sum_pcd_b = 0;
//    int num_pcd = 0;
//    for (int m = start; m < end; m++) {
//        for (int n = 0; n < N; n++) {
//
//            // 各要素カメラごとに
//            for (int i = 0; i < Hpin; i++) {
//                for (int j = 0; j < Wpin; j++) {
//
//                    // // 表示画像中における要素画像の左上画素の画素番号
//                    // startu = static_cast<int>(j * intv);
//                    // startv = static_cast<int>(i * intv);
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int zi = 0; zi < num_z_level; zi++) {
//
//                        ximg = x_img[n][j][zi];
//                        yimg = y_img[m][i][zi];
//
//                        // ximg = x_img[(n * Wpin + j) * num_z_level + zi];
//                        // yimg = y_img[(m * Hpin + i) * num_z_level + zi];
//
//                        // 注目画素の位置がイメージプレーン内にあれば
//                        if (ximg != -1 && yimg != -1) {
//
//                            // tmp_pcd_color = pcd_box[(yimg * WIDTH + ximg) * num_z_level + zi];
//                            tmp_pcd_color = pcd_box[yimg][ximg][zi];
//
//                            if (tmp_pcd_color > 0) {
//                                img_display[startv[i] + m][startu[j] + n] = tmp_pcd_color - alpha;
//                                // sum_zi += zi;
//                                break;
//                            }
//                            // else {
//                            //     for (int k = -bsize; k <= bsize; k++) {
//                            //         for (int l = -bsize; l <= bsize; l++) {
//                            //             if (0 <= ximg + l && ximg + l < px_width_img && 0 <= yimg + k && yimg + k < px_height_img) {
//
//                            //                 tmp_pcd_color = pcd_box[yimg + k][ximg + l][zi];
//
//                            //                 if (tmp_pcd_color / alpha == 1) {
//                            //                     // sum_zi += zi;
//                            //                     img_display[startv[i] + m][startu[j] + n] = tmp_pcd_color - alpha;
//                            //                     flag1 = true;
//                            //                     break;
//
//                            //                     // tmp_pcd_color = tmp_pcd_color - alpha;
//                            //                     // sum_pcd_b += static_cast<int>(floor(tmp_pcd_color / (256 * 256)));
//                            //                     // sum_pcd_g += static_cast<int>(floor(tmp_pcd_color / 256 - sum_pcd_b * 256));
//                            //                     // sum_pcd_r += static_cast<int>(floor(tmp_pcd_color - (sum_pcd_b * 256 - sum_pcd_g) * 256));
//                            //                     // num_pcd++;
//                            //                     // flag1 = true;
//                            //                 }
//
//                            //             }
//                            //         }
//                            //         if (flag1) {
//                            //             break;
//                            //         }
//                            //     }
//
//                            //     // img_display[startv[i] + m][startu[j] + n] = static_cast<unsigned char>(round(sum_pcd_b / num_pcd)) + static_cast<unsigned char>(round(sum_pcd_g / num_pcd)) * 256 + static_cast<unsigned char>(round(sum_pcd_r / num_pcd)) * 256 * 256;
//
//                            //     // if (img_display[startv[i] + m][startu[j] + n] > 0) {
//                            //     //     cout << "img_display:" << img_display[startv[i] + m][startu[j] + n] << endl;
//                            //     // }
//                            //     // sum_pcd_b = 0;
//                            //     // sum_pcd_g = 0;
//                            //     // sum_pcd_r = 0;
//                            //     // num_pcd = 0;
//
//                            //     // if (flag1) {
//                            //     //     flag1 = false;
//                            //     //     break;
//                            //     // }
//                            // }
//                            // if (flag1) {
//                            //     flag1 = false;
//                            //     break;
//                            // }
//                        }
//                        //注目画素の位置がイメージプレーン外にあれば
//                        // else {
//                        //     cout << "wow" << endl;
//                        // }
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
//    cout << "vt-v10-8" << endl;
//
//    // double start, finish;
//    // clock_t cpu_time;
//
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    cout << "wp:" << wp << endl;
//
//    //Display *display;
//    //Window window;
//    int screen;
//
//    // unsigned char *img_data = new unsigned char[pw_disp * ph_disp];
//    unsigned char* img_data;
//    img_data = (unsigned char*)malloc(sizeof(unsigned char) * pw_disp * ph_disp);
//
//    int*** pcd_box;
//    pcd_box = (int***)malloc(sizeof(int**) * px_height_img);
//    for (int i = 0; i < px_height_img; i++) {
//        pcd_box[i] = (int**)malloc(sizeof(int*) * px_width_img);
//        for (int j = 0; j < px_width_img; j++) {
//            pcd_box[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//    }
//
//    // int* pcd_box;
//    // pcd_box = (int*)malloc(sizeof(int) * px_height_img * px_width_img * num_z_level);
//
//    // // 視点パラメータ(mm)
//    // double* uo;
//    // double* vo;
//    // uo = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    // vo = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    // for (int i = 0; i < Hpin; i++) {
//    //     for (int j = 0; j < Wpin; j++) {
//
//    //         uo[i * Wpin + j] = (j - static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp;
//    //         vo[i * Wpin + j] = (i - static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp;
//
//    //         //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//    //     }
//    // }
//
//    // CSVファイルのパス
//    std::string filePath = "output_v3.csv";
//
//    // 行数と列数
//    int rows = WIDTH * HEIGHT;
//    int cols = 6;
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
//
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
//    // cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    cv::Mat img = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    // unsigned char* img_display = new unsigned char[pw_disp * ph_disp * 4];
//    // unsigned char* img = new unsigned char[(N * range) * (N * range) * 3];
//    int** img_display;
//    img_display = (int**)malloc(sizeof(int*) * ph_disp);
//    // double** val_z;
//    // val_z = (double**)malloc(sizeof(double*) * ph_disp);
//    for (int i = 0; i < ph_disp; i++) {
//        img_display[i] = (int*)malloc(sizeof(int) * pw_disp);
//        // val_z[i] = (double*)malloc(sizeof(double) * pw_disp);
//    }
//    // double*** new_data;
//    // new_data = (double***)malloc(sizeof(double**) * num_z_level);
//    // for (int i = 0; i < num_z_level; i++) {
//    //     new_data[i] = (double**)malloc(sizeof(double*) * WIDTH * HEIGHT);
//    //     for (int j = 0; j <  WIDTH * HEIGHT; j++){
//    //         new_data[i][j] = (double*)malloc(sizeof(double) * 6);
//    //     }
//    // }
//
//    // int* img;
//    // img = (int*)malloc(sizeof(int) * (N * range) * (N * range));
//
//    // for (int i = 0; i < px_height_img; i++) {
//    //     for (int j = 0; j < px_width_img; j++) {
//    //         for (int k = 0; k < num_z_level; k++) {
//    //             pcd_box[i][j][k] = 255 + 255 * 256 + 255 * 256 * 256 + 256 * 256 * 256;
//    //         }
//    //     }
//    // }
//
//    int* startu;
//    int* startv;
//    startu = (int*)malloc(sizeof(int) * Wpin);
//    startv = (int*)malloc(sizeof(int) * Hpin);
//
//    for (int i = 0; i < Wpin; i++) {
//        startu[i] = static_cast<int>(floor(i * intv));
//    }
//
//    for (int i = 0; i < Hpin; i++) {
//        startv[i] = static_cast<int>(floor(i * intv));
//    }
//
//    int*** x_img;
//    int*** y_img;
//    x_img = (int***)malloc(sizeof(int**) * N);
//    y_img = (int***)malloc(sizeof(int**) * N);
//    for (int i = 0; i < N; i++) {
//        x_img[i] = (int**)malloc(sizeof(int*) * Wpin);
//        y_img[i] = (int**)malloc(sizeof(int*) * Hpin);
//
//        for (int j = 0; j < Wpin; j++) {
//            x_img[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//
//        for (int j = 0; j < Hpin; j++) {
//            y_img[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//    }
//
//    // int* x_img;
//    // int* y_img;
//    // x_img = (int*)malloc(sizeof(int) * N * Wpin * num_z_level);
//    // y_img = (int*)malloc(sizeof(int) * N * Hpin * num_z_level);
//
//    // int* px_range;
//    // int* box_size;
//    // px_range = (int*)malloc(sizeof(int) * num_z_level);
//    // box_size = (int*)malloc(sizeof(int) * num_z_level);
//
//    double u, v, z, u_img, v_img, px_range, W_img, H_img, coef, uo, vo;
//    int alpha = 256 * 256 * 256;
//
//    for (int m = 0; m < N; m++) {
//        for (int n = 0; n < N; n++) {
//
//            // cout << "m:" << m << ", n:" << n << endl;
//
//            // 左上端の要素カメラの視点位置
//            uo = (-static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp;
//            vo = (-static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp;
//
//            // 注目画素の画素位置
//            u = (n + 0.5) * pd + uo - N * 0.5 * pd;
//            v = (m + 0.5) * pd + vo - N * 0.5 * pd;
//
//            for (int i = 0; i < Hpin; i++) {
//                for (int j = 0; j < Wpin; j++) {
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int zi = 0; zi < num_z_level; zi++) {
//
//                        // cout << "zi:" << zi << endl;
//                        // 奥行き
//                        z = pow(base, zi + 1);
//
//                        // 変換係数
//                        coef = (z + D) / g;
//
//                        // cout << "z:" << z << endl;
//
//                        // 奥行きレベルにおける注目画素の位置
//                        u_img = coef * (u - uo) + uo;
//                        v_img = coef * (v - vo) + vo;
//
//                        // cout << "u_img:" << u_img << ", v_img:" << v_img << endl;
//
//                        // イメージプレーンの縦横の長さ
//                        W_img = 2 * z * Htan;
//                        H_img = 2 * z * Vtan;
//
//                        // cout << "W_img:" << W_img << ", H_img:" << H_img << endl;
//
//                        // イメージプレーン上の注目画素の画素番号
//                        x_img[n][j][zi] = static_cast<int>(floor((u_img + W_img * 0.5) / (W_img / px_width_img)));
//                        y_img[m][i][zi] = static_cast<int>(floor((v_img + H_img * 0.5) / (H_img / px_height_img)));
//
//                        // x_img[(n * Wpin + j) * num_z_level + zi] = static_cast<int>(floor((u_img + W_img * 0.5) / (W_img / px_width_img)));
//                        // y_img[(m * Hpin + i) * num_z_level + zi] = static_cast<int>(floor((v_img + H_img * 0.5) / (H_img / px_height_img)));
//
//                        //注目画素がイメージプレーン外にあれば-1を代入
//                        if (x_img[n][j][zi] < 0 || px_width_img <= x_img[n][j][zi] || y_img[m][i][zi] < 0 || px_height_img <= y_img[m][i][zi]) {
//                            x_img[n][j][zi] = -1;
//                            y_img[m][i][zi] = -1;
//                        }
//
//                        // 注目範囲の大きさと画素の個数
//                        // px_range = coef * pd;
//                        // box_size[zi] = static_cast<int>(round(px_range / (W_img / px_width_img)));
//
//                        // cout << "pixel range:" << px_range <<  ", box size:" << box_size[zi] << endl;
//                    }
//
//                    uo += wp;
//                    u += wp;
//                }
//                uo = (-static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp;
//                u = (n + 0.5) * pd + uo - N * 0.5 * pd;
//
//                vo += wp;
//                v += wp;
//            }
//            // cout << "u:" << u << ", v:" << v << endl;
//
//        }
//    }
//
//    cout << "calculating x_img and y_img finished." << endl;
//
//    // フレーム処理
//    for (int tt = 0; tt < 1; tt++) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z;
//        double W_img, H_img, z;
//        int z_level, ximg, yimg;
//        // int nx, ny;
//
//        // 箱をゼロオフセット
//        for (int i = 0; i < px_height_img; i++) {
//            for (int j = 0; j < px_width_img; j++) {
//                for (int k = 0; k < num_z_level; k++) {
//                    pcd_box[i][j][k] = 0;
//                }
//            }
//        }
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                img_display[i][j] = 0;
//            }
//        }
//
//        for (int k = 0; k < WIDTH * HEIGHT; k++) {
//            if (data[k][2] > 0 && data[k][2] <= max_z) {
//
//                tmp_pcd_x = data[k][0];
//                tmp_pcd_y = data[k][1];
//                tmp_pcd_z = data[k][2];
//
//                z_level = static_cast<int>(round(log10(tmp_pcd_z * 1000) / log10(base)));
//                // cout << "z level:" << z_level << endl;
//
//                // 奥行き
//                z = pow(base, z_level + 1);
//
//                // イメージプレーンの縦横の長さ
//                W_img = 2 * z * Htan;
//                H_img = 2 * z * Vtan;
//
//                ximg = floor((tmp_pcd_x * 1000 + W_img * 0.5) / (W_img / px_width_img));
//                yimg = floor((tmp_pcd_y * 1000 + H_img * 0.5) / (H_img / px_height_img));
//
//                if (0 <= ximg && ximg < px_width_img && 0 <= yimg && yimg < px_height_img) {
//
//                    // cout << "in!" << endl;
//                    // pcd_box[(yimg * px_width_img + ximg) * num_z_level + z_level] = static_cast<unsigned char>(data[k][3]) + static_cast<unsigned char>(data[k][4]) * 256 + static_cast<unsigned char>(data[k][5]) * 256 * 256 + 256 * 256 * 256;
//                    pcd_box[yimg][ximg][z_level] = static_cast<unsigned char>(data[k][3]) + static_cast<unsigned char>(data[k][4]) * 256 + static_cast<unsigned char>(data[k][5]) * 256 * 256 + 256 * 256 * 256;
//
//                }
//            }
//        }
//
//        // cout << "ok" << endl;
//
//        // insert_pixels(0, N, std::ref(img_display), std::ref(pcd_box));
//
//        const int numThreads = 8;
//        vector<thread> threads;
//        int rowsPerThread = N / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(pcd_box), std::ref(x_img), std::ref(y_img), std::ref(startu), std::ref(startv));
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
//        // cout << "calc finished" << endl;
//        // finished_threads = 0;
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        //display = XOpenDisplay(nullptr);
//        //if (display == nullptr) {
//        //    std::cerr << "Cannot open display" << std::endl;
//        //    return 1;
//        //}
//
//        //screen = DefaultScreen(display);
//        //window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 500, 500, 1, BlackPixel(display, screen), WhitePixel(display, screen));
//        //XSelectInput(display, window, ExposureMask | KeyPressMask);
//        //XMapWindow(display, window);
//        //
//        //// 画像データをXImageに変換
//        //XImage *image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, reinterpret_cast<char*>(img_data), pw_disp, ph_disp, 8, 0);
//
//        //for (int y = 0; y < ph_disp; y++) {
//        //    for (int x = 0; x < pw_disp; x++) {
//        //        XPutPixel(image, x, y, img_display[y][x]);
//        //    }
//        //}
//
//        //// メインループ
//        //XEvent e;
//        //while (true) {
//        //    XNextEvent(display, &e);
//        //    if (e.type == Expose) {
//        //        XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, pw_disp, ph_disp);
//        //    }
//        //    if (e.type == KeyPress)
//        //        break;
//        //}
//
//        //// クリーンアップ
//        //XFree(image);
//        //XDestroyWindow(display, window);
//        //XCloseDisplay(display);
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                img.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i][j] / (256 * 256)));
//                img.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i][j] / 256 - img.at<cv::Vec3b>(i, j)[2] * 256));
//                img.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i][j] - (img.at<cv::Vec3b>(i, j)[2] * 256 - img.at<cv::Vec3b>(i, j)[1]) * 256));
//            }
//        }
//
//        ostringstream stream;
//        stream << "./images/v10-8_img_display_NumZLevel" << num_z_level << ".png";
//
//        cv::String filename = stream.str();
//
//        imwrite(filename, img);
//
//        //// 画像を表示
//        //cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        //cv::imshow("Display window", img_display);
//
//        //// キーボードの入力を待つ（1ms）
//        //// 何かしらのキーが押されたらループを抜ける
//        //cv::waitKey(0);
//        //cv::destroyWindow("Display window");
//
//       //// 各イメージプレーンの画像を保存（テスト用）
//       //// ostringstream stream;
//       //// cv::String filename;
//       // for (int zi = 0; zi < num_z_level; zi++) {
//
//       //     stream << "./images/v10-7-2_zi" << zi << "_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png";
//       //     cout << "zi:" << zi << endl;
//
//       //     for (int i = 0; i < px_height_img; i++) {
//       //         for (int j = 0; j < px_width_img; j++) {
//       //             if (pcd_box[i][j][zi] > 0) {
//       //                 img.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor((pcd_box[i][j][zi] - 256 * 256 * 256) / (256 * 256)));
//       //                 img.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor((pcd_box[i][j][zi] - 256 * 256 * 256) / 256 - img.at<cv::Vec3b>(i, j)[2] * 256));
//       //                 img.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor((pcd_box[i][j][zi] - 256 * 256 * 256) - (img.at<cv::Vec3b>(i, j)[2] * 256 - img.at<cv::Vec3b>(i, j)[1]) * 256));
//       //             }
//       //         }
//       //     }
//       //     filename = stream.str();
//       //     imwrite(filename, img);
//       //     stream.str("");
//       //     stream.clear(ostringstream::goodbit);
//       //     img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//       // }
//
//    }
//
//    // free(uo);
//    // free(vo);
//
//    // cout << "a" << endl;
//
//    // 使用したメモリを解放
//    // for (int i = 0; i < rows; ++i) {
//    //     delete[] data[i];
//    // }
//    // delete[] data;    
//    free(img_data);
//    for (int i = 0; i < ph_disp; i++) {
//        free(img_display[i]);
//        // free(val_z[i]);
//    }
//    free(img_display);
//    // free(val_z);
//
//    // cout << "b" << endl;
//
//    for (int i = 0; i < N; i++) {
//        for (int j = 0; j < Wpin; j++) {
//            free(x_img[i][j]);
//        }
//        free(x_img[i]);
//
//        for (int j = 0; j < Hpin; j++) {
//            free(y_img[i][j]);
//        }
//        free(y_img[i]);
//    }
//    free(x_img);
//    free(y_img);
//
//    for (int i = 0; i < px_height_img; i++) {
//        for (int j = 0; j < px_width_img; j++) {
//            free(pcd_box[i][j]);
//        }
//        free(pcd_box[i]);
//    }
//    free(pcd_box);
//
//    free(startu);
//    free(startv);
//
//    // free(px_range);
//    // free(box_size);
//
//    // // ウィンドウを閉じる
//    // cv::destroyAllWindows();
//
//
//    return EXIT_SUCCESS;
//}

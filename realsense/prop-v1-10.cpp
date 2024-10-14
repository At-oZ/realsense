///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* 差分:再構成品質の改善の検討(v1-7の変更点 & 3次元配列の詳細化と点の拡大) → PSNR上がらず... */
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
//int WIDTH = 640;
//int HEIGHT = 480;
//int FPS = 30;
//
//// 点群データ配列の行数と列数
//int rows = WIDTH * HEIGHT;
//int cols = 6;
//
//
//int D = 100;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//// パラメータ
//int num_z_level = 65; // イメージプレーンの層数
//double ptimes = 0.50; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//int box_size = static_cast<int>(floor(1 / ptimes - 1)); // 探索範囲
////int box_size = 0; // 探索範囲
//
//int Ddash = 250;
//double coef = (double)num_z_level * (double)Ddash;
//double inv_coef = 1.0 / coef;
//
//double img_pitch = ptimes * pd;
//
//int px_height_img = static_cast<int>(round(55 / ptimes));
//int px_width_img = static_cast<int>(round(55 / ptimes));
//
//int TIMES = 1;
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
//void insert_pixels(int start, int end, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int* startu, int* startv) {
//
//    int tmp_pcd_color, sum_zi, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r, tmp_nx, tmp_ny;
//    bool flag1 = false;
//    // bool flag2 = false;
//    int tmp_startu, tmp_startv;
//    //double u, v, xt, yt, zt;
//    double tmp_sp, tmp_tp, tmp_xt, tmp_yt;
//
//    // 各要素画像の各画素ごとに
//    for (int m = start; m < end; m++) {
//
//        for (int n = 0; n < N; n++) {
//
//            // 各要素カメラごとに
//            for (int i = 0; i < Hpin; i++) {
//
//                tmp_startv = static_cast<int>(i * intv);
//
//                for (int j = 0; j < Wpin; j++) {
//
//                    tmp_startu = static_cast<int>(j * intv);
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int nz = num_z_level - 1; nz >= 0; nz--) {
//
//                        tmp_nx = nx[n][j][nz];
//                        tmp_ny = ny[m][i][nz];
//
//                        //cout << "nx:" << nx << ", ny:" << ny << endl;
//                        if (0 <= tmp_nx && tmp_nx < px_width_img && 0 <= tmp_ny && tmp_ny < px_height_img) {
//                            if (alpha[nz][tmp_ny][tmp_nx]) {
//
//                                img_display.at<cv::Vec3b>(tmp_startv + m, tmp_startu + n)[2] = static_cast<int>(red[nz][tmp_ny][tmp_nx]);
//                                img_display.at<cv::Vec3b>(tmp_startv + m, tmp_startu + n)[1] = static_cast<int>(green[nz][tmp_ny][tmp_nx]);
//                                img_display.at<cv::Vec3b>(tmp_startv + m, tmp_startu + n)[0] = static_cast<int>(blue[nz][tmp_ny][tmp_nx]);
//                                // cout << "v, u:" << startv + m << ", " << startu + n << endl;
//                                break;
//                            }
//                        }
//                    }
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
//double getPSNR(const Mat& I1, const Mat& I2) {
//    Mat s1;
//    absdiff(I1, I2, s1);       // |I1 - I2|
//    s1.convertTo(s1, CV_32F);  // Convert to float
//    s1 = s1.mul(s1);           // |I1 - I2|^2
//
//    Scalar s = sum(s1);        // Sum elements per channel
//
//    double sse = s.val[0] + s.val[1] + s.val[2]; // Sum channels
//
//    if (sse <= 1e-10) {        // For small values return zero
//        return 0;
//    }
//    else {
//        double mse = sse / (double)(I1.channels() * I1.total());
//        double psnr = 10.0 * log10((255 * 255) / mse);
//        return psnr;
//    }
//}
//
//Mat getDiff(const Mat& I1, const Mat& I2) {
//    Mat s1;
//    absdiff(I1, I2, s1);
//    s1.convertTo(s1, CV_8U);  // Convert to 8-bit
//    return s1;
//}
//
//Mat readImage(const std::string& filename) {
//    std::ifstream file(filename, std::ios::binary);
//    if (!file) {
//        return Mat();
//    }
//
//    std::vector<uchar> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//    return imdecode(buffer, IMREAD_COLOR);
//}
//
//
//int main(int argc, char* argv[])
//{
//
//    cout << "prop-v1-10" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    cout << "num z level:" << num_z_level << ", ptimes:" << ptimes << ", box size:" << box_size << ", TIMES:" << TIMES << endl;
//
//    // 視点パラメータ(mm)
//    //double* sp;
//    //double* tp;
//    //sp = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    //tp = (double*)malloc(sizeof(double) * Hpin * Wpin);
//    //for (int i = 0; i < Hpin; i++) {
//    //    for (int j = 0; j < Wpin; j++) {
//
//    //        sp[i * Wpin + j] = (j - (Wpin - 1) * 0.5) * wp;
//    //        tp[i * Wpin + j] = (i - (Hpin - 1) * 0.5) * wp;
//
//    //        //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//    //    }
//    //}
//
//    // 各要素画像の原点画素位置(左上)
//    int* startu = (int*)malloc(sizeof(int) * Wpin);
//    int* startv = (int*)malloc(sizeof(int) * Hpin);
//    for (int i = 0; i < Wpin; i++) {
//        startu[i] = static_cast<int>(i * intv);
//    }
//    for (int i = 0; i < Hpin; i++) {
//        startv[i] = static_cast<int>(i * intv);
//    }
//
//    //// 変換後の座標パラメータ
//    //double** xt = (double**)malloc(sizeof(double*) * N);
//    //double** yt = (double**)malloc(sizeof(double*) * N);
//    //for (int i = 0; i < N; i++) {
//    //    xt[i] = (double*)malloc(sizeof(double) * Wpin);
//    //    yt[i] = (double*)malloc(sizeof(double) * Hpin);
//    //}
//
//    //double u, zt;
//    //for (int i = 0; i < N; i++) {
//
//    //    u = ((double)i - (double)(N - 1) * 0.5) * pd;
//    //    zt = (double)(num_z_level - 1) * inv_coef;
//
//    //    for (int j = 0; j < Wpin; j++) {
//
//    //        xt[i][j] = sp[j] * zt + u / g;
//
//    //    }
//
//    //    for (int j = 0; j < Hpin; j++) {
//
//    //        yt[i][j] = tp[j * Wpin] * zt + u / g;
//
//    //    }
//
//    //}
//
//    // 3次元配列のインデックス
//    int*** nx = (int***)malloc(sizeof(int**) * N);
//    int*** ny = (int***)malloc(sizeof(int**) * N);
//    for (int i = 0; i < N; i++) {
//
//        nx[i] = (int**)malloc(sizeof(int*) * Wpin);
//        ny[i] = (int**)malloc(sizeof(int*) * Hpin);
//
//        for (int j = 0; j < Wpin; j++) {
//            nx[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//        for (int j = 0; j < Hpin; j++) {
//            ny[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//    }
//
//    double u, xt, yt, zt, nz;
//    for (int i = 0; i < N; i++) {
//
//        u = ((double)i - (double)(N - 1) * 0.5) * pd;
//
//        for (int j = 0; j < Wpin; j++) {
//
//            zt = (double)(num_z_level - 1) * inv_coef;
//            xt = (j - (Wpin - 1) * 0.5) * wp * zt + u / g;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                nx[i][j][nz] = static_cast<int>(floor((g / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                zt -= inv_coef;
//                xt -= (j - (Wpin - 1) * 0.5) * wp * inv_coef;
//            }
//
//        }
//
//        for (int j = 0; j < Hpin; j++) {
//
//            zt = (double)(num_z_level - 1) * inv_coef;
//            yt = (j - (Hpin - 1) * 0.5) * wp * zt + u / g;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                ny[i][j][nz] = static_cast<int>(floor((g / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//                zt -= inv_coef;
//                yt -= (j - (Wpin - 1) * 0.5) * wp * inv_coef;
//            }
//
//        }
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
//    cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//    int*** red;
//    int*** green;
//    int*** blue;
//    bool*** alpha;
//    int*** num_pcd;
//    //double*** val_z;
//
//    // red = new int**[num_z_level];
//    // green = new int**[num_z_level];
//    // blue = new int**[num_z_level];
//    // alpha = new bool**[num_z_level];
//    // val_z = new int**[num_z_level];
//
//    red = (int***)malloc(sizeof(int**) * num_z_level);
//    green = (int***)malloc(sizeof(int**) * num_z_level);
//    blue = (int***)malloc(sizeof(int**) * num_z_level);
//    alpha = (bool***)malloc(sizeof(bool**) * num_z_level);
//    num_pcd = (int***)malloc(sizeof(int**) * num_z_level);
//    //val_z = (double***)malloc(sizeof(double**) * num_z_level);
//
//    for (int i = 0; i < num_z_level; i++) {
//
//        // red[i] = new int*[px_height_img];
//        // green[i] = new int*[px_height_img];
//        // blue[i] = new int*[px_height_img];
//        // alpha[i] = new bool*[px_height_img];
//        // val_z[i] = new int*[px_height_img];
//
//        red[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        green[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        blue[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        alpha[i] = (bool**)malloc(sizeof(bool*) * px_height_img);
//        num_pcd[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        //val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
//
//        for (int j = 0; j < px_height_img; j++) {
//
//            // red[i][j] = new int[px_width_img];
//            // green[i][j] = new int[px_width_img];
//            // blue[i][j] = new int[px_width_img];
//            // alpha[i][j] = new bool[px_width_img];
//            // val_z[i][j] = new int[px_width_img];
//
//            red[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            green[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            blue[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            alpha[i][j] = (bool*)malloc(sizeof(bool) * px_width_img);
//            num_pcd[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            //val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
//
//        }
//    }
//
//    // // 箱の全引き出しに白を格納（テスト用）
//    // for (int i = 0; i < num_z_level; i++) {
//    //     for (int j = 0; j < px_height_img; j++) {
//    //         for (int k = 0; k < px_width_img; k++) {
//    //             pcd_box[i][j][k] = 255 + 255 * 256 + 255 * 256 * 256 + 256 * 256 * 256;
//    //         }
//    //     }
//    // }
//
//    double sum_time = 0;
//    double sum_psnr = 0;
//    // フレーム処理
//    for (int tt = 0; tt < TIMES; tt++) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        // 箱とzバッファをオフセット
//        for (int i = 0; i < num_z_level; i++) {
//            for (int j = 0; j < px_height_img; j++) {
//                for (int k = 0; k < px_width_img; k++) {
//                    //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
//                    red[i][j][k] = 0;
//                    green[i][j][k] = 0;
//                    blue[i][j][k] = 0;
//                    alpha[i][j][k] = false;
//                    num_pcd[i][j][k] = 0;
//                    //val_z[i][j][k] = 0;
//                }
//            }
//        }
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//
//                img_display.at<cv::Vec3b>(i, j)[0] = 0;
//                img_display.at<cv::Vec3b>(i, j)[1] = 0;
//                img_display.at<cv::Vec3b>(i, j)[2] = 0;
//
//            }
//        }
//
//        //int startu, startv;
//        //for (int i = 0; i < Hpin; i++) {
//        //    startv = static_cast<int>(i * intv);
//        //    for (int j = 0; j < pw_disp; j++) {
//        //        img_display[startv][j] = 64 + 64 * 256 + 64 * 256 * 256;
//        //    }
//        //}
//        //for (int i = 0; i < Wpin; i++) {
//        //    startu = static_cast<int>(i * intv);
//        //    for (int j = 0; j < ph_disp; j++) {
//        //        img_display[j][startu] = 64 + 64 * 256 + 64 * 256 * 256;
//        //    }
//        //}
//
//        double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double tmp_xt, tmp_yt, tmp_zt;
//        int tmp_nx, tmp_ny, tmp_nz;
//        int pcd_count = 0;
//
//        //点群を箱に格納
//        for (int k = 0; k < rows; k++) {
//
//            tmp_pcd_x = data[k][0] * 1000.0;
//            tmp_pcd_y = data[k][1] * 1000.0;
//            tmp_pcd_z = data[k][2] * 1000.0 + (double)D;
//            tmp_pcd_b = data[k][3];
//            tmp_pcd_g = data[k][4];
//            tmp_pcd_r = data[k][5];
//
//            tmp_zt = 1.0 / tmp_pcd_z;
//            tmp_xt = tmp_pcd_x * tmp_zt;
//            tmp_yt = tmp_pcd_y * tmp_zt;
//
//            tmp_nx = static_cast<int>(floor((g / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//            tmp_ny = static_cast<int>(floor((g / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//            tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//            //cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;
//
//            if (0 <= tmp_nz && tmp_nz < num_z_level) {
//                for (int m = -box_size; m <= box_size; m++) {
//                    for (int n = -box_size; n <= box_size; n++) {
//                        if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
//                            blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                            green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                            red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                            alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                            num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                        }
//                    }
//                }
//            }
//        }
//
//        for (int i = 0; i < num_z_level; i++) {
//            for (int j = 0; j < px_height_img; j++) {
//                for (int k = 0; k < px_width_img; k++) {
//                    //cout << i << ", " << j << ", " << k << "; " << "red:" << red[i][j][k] << ", green:" << green[i][j][k] << ", blue:" << blue[i][j][k] << endl;
//                    if (alpha[i][j][k]) {
//                        red[i][j][k] = red[i][j][k] / num_pcd[i][j][k];
//                        green[i][j][k] = green[i][j][k] / num_pcd[i][j][k];
//                        blue[i][j][k] = blue[i][j][k] / num_pcd[i][j][k];
//                    }
//                }
//            }
//        }
//
//        // insert_pixels(0, N, img_display, red, green, blue, alpha, sp, tp);
//
//        const int numThreads = 16;
//        vector<thread> threads;
//        int rowsPerThread = N / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(startu), std::ref(startv));
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
//        //cout << "重なり回数:" << pcd_count << endl;
//        sum_time += duration.count();
//
//        // 表示画像の保存
//        ostringstream stream;
//        stream << "./images/prop-v1-10_ImgDisplay_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << "_boxSize" << box_size << ".png";
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
//        //    cout << "zi:" << zi << endl;
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
//        //std::string filename_original, filename_compared;
//        //Mat img1;
//        //Mat img2;
//
//        //stream.str("");
//        //stream.clear(ostringstream::goodbit);
//
//        //stream << "C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/images/DirectProjection_pdBase_img_display_z0_g4_wp1.2_pd0.075_D100.png";
//
//        //filename_original = stream.str();
//
//        //stream.str("");
//        //stream.clear(ostringstream::goodbit);
//
//        //stream << "C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/images/prop-v1-10_ImgDisplay_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << "_boxSize" << box_size << ".png";
//
//        //filename_compared = stream.str();
//
//        //img1 = readImage(filename_original);
//        //img2 = readImage(filename_compared);
//
//        //if (img1.empty()) {
//        //    cout << "Could not open or find the image: " << filename_original << endl;
//        //    return -1;
//        //}
//        //if (img2.empty()) {
//        //    cout << "Could not open or find the image: " << filename_compared << endl;
//        //    return -1;
//        //}
//
//        //double psnrValue = getPSNR(img1, img2);
//        //cout << "PSNR value is: " << psnrValue << " dB" << endl;
//        //sum_psnr += psnrValue;
//
//    }
//
//    cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//    //cout << "平均PSNR: " << sum_psnr / TIMES << " dB" << std::endl;
//
//    //try {
//    //    std::string filename_original, filename_compared;
//
//    //    std::cout << "Please enter the name of original image file: ";
//    //    std::getline(std::cin, filename_original);
//    //    std::cout << "Please enter the name of compared image file: ";
//    //    std::getline(std::cin, filename_compared);
//
//    //    Mat img1 = readImage(filename_original);
//    //    Mat img2 = readImage(filename_compared);
//
//    //    if (img1.empty()) {
//    //        cout << "Could not open or find the image: " << filename_original << endl;
//    //        return -1;
//    //    }
//    //    if (img2.empty()) {
//    //        cout << "Could not open or find the image: " << filename_compared << endl;
//    //        return -1;
//    //    }
//
//    //    double psnrValue = getPSNR(img1, img2);
//    //    cout << "PSNR value is: " << psnrValue << " dB" << endl;
//
//    //    Mat img_diff = getDiff(img1, img2);
//
//    //    // img_diffをファイルとして保存して内容を確認する
//    //    imwrite("img_diff.png", img_diff);
//
//    //    // img_diffのコントラストを強調する
//    //    Mat img_diff_enhanced;
//    //    normalize(img_diff, img_diff_enhanced, 0, 255, NORM_MINMAX, CV_8U);
//
//    //    //cv::namedWindow("Difference image", cv::WINDOW_AUTOSIZE);
//    //    //cv::imshow("Difference image", img_diff_enhanced);
//
//    //    //cv::waitKey(0);
//    //    //cv::destroyWindow("Difference image");
//
//    //    ostringstream stream;
//    //    stream << "./images/v1-3v_v11-6v_diff_img.png";
//    //    cv::String filename = stream.str();
//
//    //    imwrite(filename, img_diff);
//
//    //}
//    //catch (cv::Exception& e) {
//    //    cout << "OpenCV exception: " << e.what() << endl;
//    //    return -1;
//    //}
//    //catch (std::exception& e) {
//    //    cout << "Standard exception: " << e.what() << endl;
//    //    return -1;
//    //}
//    //catch (...) {
//    //    cout << "Unknown exception occurred." << endl;
//    //    return -1;
//    //}
//
//    // 使用したメモリを解放
//    for (int i = 0; i < rows; ++i) {
//        delete[] data[i];
//    }
//    delete[] data;
//
//    for (int i = 0; i < num_z_level; i++) {
//        for (int j = 0; j < px_height_img; j++) {
//            free(red[i][j]);
//            free(green[i][j]);
//            free(blue[i][j]);
//            free(alpha[i][j]);
//            free(num_pcd[i][j]);
//            //free(val_z[i][j]);
//        }
//        free(red[i]);
//        free(green[i]);
//        free(blue[i]);
//        free(alpha[i]);
//        free(num_pcd[i]);
//        //free(val_z[i]);
//    }
//    free(red);
//    free(green);
//    free(blue);
//    free(alpha);
//    free(num_pcd);
//    //free(val_z);
//
//    //free(sp);
//    //free(tp);
//
//    free(startu);
//    free(startv);
//
//    //for (int i = 0; i < N; i++) {
//    //    free(xt[i]);
//    //    free(yt[i]);
//    //}
//    //free(xt);
//    //free(yt);
//
//    for (int i = 0; i < N; i++) {
//        for (int j = 0; j < Wpin; j++) {
//            free(nx[i][j]);
//        }
//        for (int j = 0; j < Hpin; j++) {
//            free(ny[i][j]);
//        }
//        free(nx[i]);
//        free(ny[i]);
//    }
//
//    return EXIT_SUCCESS;
//}
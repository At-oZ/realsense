///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* 差分:実際に再構成 */
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
//int D = 100;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//int num_z_level = 30;
//double coef = 1.0 * num_z_level * 250;
//
//int px_height_img = 50;
//int px_width_img = 50;
//
//double img_pitch = 1.0 * pd;
//
//int box_size = 1;
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
//void insert_pixels(int start, int end, int** img_display, int*** pcd_box, double* uo, double* vo) {
//
//    int nx, ny, tmp_pcd_color, sum_zi;
//    int alpha = 256 * 256 * 256;
//    bool flag1 = false;
//    // bool flag2 = false;
//    int startu, startv;
//    double half_m, half_n, full_nx, full_ny, tmp_uo, tmp_vo;
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
//        half_m = m - N * 0.5;
//
//        for (int n = 0; n < N; n++) {
//
//            half_n = n - N * 0.5;
//
//            // 各要素カメラごとに
//            for (int i = 0; i < Hpin; i++) {
//
//                // 表示画像中における要素画像の左上画素の画素番号
//                startv = static_cast<int>(i * intv);
//
//                for (int j = 0; j < Wpin; j++) {
//
//                    tmp_uo = uo[i * Wpin + j];
//                    tmp_vo = vo[i * Wpin + j];
//
//                    full_nx = half_n * pd;
//                    full_ny = half_m * pd;
//
//                    nx = static_cast<int>(floor((full_nx + px_width_img * 0.5 * img_pitch) / img_pitch));
//                    ny = static_cast<int>(floor((full_ny + px_height_img * 0.5 * img_pitch) / img_pitch));
//
//                    // 表示画像中における要素画像の左上画素の画素番号
//                    startu = static_cast<int>(j * intv);
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int nz = num_z_level - 1; nz >= 0; nz--) {
//
//                        if (box_size <= nx && nx < px_width_img - box_size && box_size <= ny && ny < px_height_img - box_size) {
//                            // cout << "nx, ny:" << nx << ", " << ny << endl;
//                            tmp_pcd_color = pcd_box[nz][ny][nx];
//
//                            if (tmp_pcd_color > 0) {
//                                img_display[startv + m][startu + n] = tmp_pcd_color - alpha;
//                                // cout << "v, u:" << startv + m << ", " << startu + n << endl;
//                                break;
//                            }
//
//                            // for (int k = -box_size; k <= box_size; k++){
//                            //     for (int l = -box_size; l <= box_size; l++){
//
//                            //         tmp_pcd_color = pcd_box[nz][ny + k][nx + l];
//
//                            //         if (tmp_pcd_color > 0) {
//                            //             img_display[startv + m][startu + n] = tmp_pcd_color - alpha;
//                            //             // cout << "v, u:" << startv + m << ", " << startu + n << endl;
//                            //             flag1 = true;
//                            //             break;
//                            //         }
//
//                            //     }
//
//                            //     if (flag1) {
//                            //         break;
//                            //     }
//                            // }
//                            // if (flag1) {
//                            //     flag1 = false;
//                            //     break;
//                            // }
//
//                        }
//
//                        full_nx += (g / coef) * tmp_uo;
//                        full_ny += (g / coef) * tmp_vo;
//
//                        nx = static_cast<int>(floor((full_nx + px_width_img * 0.5 * img_pitch) / img_pitch));
//                        ny = static_cast<int>(floor((full_ny + px_height_img * 0.5 * img_pitch) / img_pitch));
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
//    cout << "vt-v11-2" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    cout << "wp:" << wp << endl;
//
//    //Display *display;
//    //Window window;
//    //int screen;
//
//    unsigned char* img_data;
//    img_data = (unsigned char*)malloc(sizeof(unsigned char) * pw_disp * ph_disp);
//
//    cv::Mat img_save = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//
//    // 視点パラメータ(mm)
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
//    int** img_display;
//    img_display = (int**)malloc(sizeof(int*) * ph_disp);
//    for (int i = 0; i < ph_disp; i++) {
//        img_display[i] = (int*)malloc(sizeof(int) * pw_disp);
//    }
//
//    // CSVファイルのパス
//    std::string filePath = "output_v3.csv";
//
//    // 行数と列数
//    int rows = WIDTH * HEIGHT; // 例として10行
//    int cols = 6; // 例として5列
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
//    // cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//    // unsigned char* img_display = new unsigned char[pw_disp * ph_disp * 4];
//    // unsigned char* img = new unsigned char[(N * range) * (N * range) * 3];
//    // int* img;
//    // img = (int*)malloc(sizeof(int) * (N * range) * (N * range));
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
//    //// 箱の全引き出しに白を格納（テスト用）
//    //for (int i = 0; i < num_z_level; i++) {
//    //    for (int j = 0; j < px_height_img; j++) {
//    //        for (int k = 0; k < px_width_img; k++) {
//    //            pcd_box[i][j][k] = 255 + 255 * 256 + 255 * 256 * 256 + 256 * 256 * 256;
//    //        }
//    //    }
//    //}
//
//
//    // フレーム処理
//    for (int tt = 0; tt < 1; tt++) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//        double newx, newy;
//        int nx, ny, nz;
//
//         //点群を箱に格納
//         for (int k = 0; k < WIDTH * HEIGHT; k++) {
//
//             tmp_pcd0 = data[k][0];
//             tmp_pcd1 = data[k][1];
//             tmp_pcd2 = data[k][2];
//             tmp_pcd3 = data[k][3];
//             tmp_pcd4 = data[k][4];
//             tmp_pcd5 = data[k][5];
//
//             inv_d = 1.0 / (tmp_pcd2 * 1000 + D);
//
//             newx = (g * inv_d) * (tmp_pcd0 * 1000);
//             newy = (g * inv_d) * (tmp_pcd1 * 1000);
//
//             nx = static_cast<int>(floor((newx + px_width_img * 0.5 * img_pitch) / img_pitch));
//             ny = static_cast<int>(floor((newy + px_height_img * 0.5 * img_pitch) / img_pitch));
//             nz = static_cast<int>(round(coef * inv_d));
//
//             // cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;
//
//             if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img && 0 <= nz && nz < num_z_level) {
//                 pcd_box[nz][ny][nx] = static_cast<unsigned char>(tmp_pcd3) + static_cast<unsigned char>(tmp_pcd4) * 256 + static_cast<unsigned char>(tmp_pcd5) * 256 * 256 + 256 * 256 * 256;
//                 // cout << "pcd_box:" << pcd_box[nz][ny][nx] << endl;
//             }
//         }
//
//        // insert_pixels(0, N, std::ref(img_display), std::ref(pcd_box), std::ref(uo), std::ref(vo));
//
//        const int numThreads = 12;
//        vector<thread> threads;
//        int rowsPerThread = N / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(pcd_box), std::ref(uo), std::ref(vo));
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
//
//                img_save.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i][j] / (256 * 256)));
//                img_save.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i][j] / 256 - img_save.at<cv::Vec3b>(i, j)[2] * 256));
//                img_save.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i][j] - (img_save.at<cv::Vec3b>(i, j)[2] * 256 - img_save.at<cv::Vec3b>(i, j)[1]) * 256));
//            }
//        }
//
//        ostringstream stream;
//        stream << "mac_v11_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png";
//        cv::String filename = stream.str();
//        imwrite(filename, img_save);
//
//        // 各イメージプレーンの画像を保存（テスト用）
//        // ostringstream stream;
//        // cv::String filename;
//        // for (int zi = 0; zi < num_z_level; zi++) {
//
//        //     stream << "mac_v11_zi" << zi << "_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png";
//        //     cout << "zi:" << zi << endl;
//
//        //     for (int i = 0; i < px_height_img; i++) {
//        //         for (int j = 0; j < px_width_img; j++) {
//        //             if (pcd_box[zi][i][j] > 0) {
//        //                 img.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor((pcd_box[zi][i][j] - 256 * 256 * 256) / (256 * 256)));
//        //                 img.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor((pcd_box[zi][i][j] - 256 * 256 * 256) / 256 - img.at<cv::Vec3b>(i, j)[2] * 256));
//        //                 img.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor((pcd_box[zi][i][j] - 256 * 256 * 256) - (img.at<cv::Vec3b>(i, j)[2] * 256 - img.at<cv::Vec3b>(i, j)[1]) * 256));
//        //             }
//        //         }
//        //     }
//        //     filename = stream.str();
//        //     imwrite(filename, img);
//        //     stream.str("");
//        //     stream.clear(ostringstream::goodbit);
//        //     img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//        // }
//
//
//    }
//
//    free(uo);
//    free(vo);
//
//    // 使用したメモリを解放
//    // for (int i = 0; i < rows; ++i) {
//    //     cout << "calc finished" << endl;
//
//    //     delete[] data[i];
//    // }
//    // delete[] data;    
//
//    for (int i = 0; i < ph_disp; i++) {
//        free(img_display[i]);
//    }
//    free(img_display);
//
//    for (int i = 0; i < num_z_level; i++) {
//        for (int j = 0; j < px_height_img; j++) {
//            free(pcd_box[i][j]);
//        }
//        free(pcd_box[i]);
//    }
//    free(pcd_box);
//
//    return EXIT_SUCCESS;
//}
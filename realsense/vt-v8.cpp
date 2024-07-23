///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* z方向を離散化する */
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
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
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
//int WIDTH = 640;
//int HEIGHT = 480;
//int FPS = 30;
//
//int D = 100;                     // 表示系とカメラとの距離(mm)
//double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
//double inv_pd = 1.0 / pd;         // pdの逆数
//
//int num_z_level = 256;
//// int max_z_level = 0;
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
//	return i;   // 配列の長さを返す
//}
//
//void insert_pixels(int start, int end, int** img_display, double*** data, double* uo, double* vo, int* num_idx, double** val_z) {
//
//    int nx, ny;
//    int startv, startu, startv2, startu2;
//    double newx, newy, newrange, coef;
//    double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//    double inv_d;
//    bool flag = false;
//    double h_disparity, v_disparity, sum_h_disparity, sum_v_disparity;
//
//
//    // int delta = 10;
//
//    // int num_width = static_cast<int>(round((WIDTH - N - 1) / Wpin));
//    // int num_height = static_cast<int>(round((HEIGHT - N - 1) / Hpin));
//
//    // cout << "num_width: " << num_width << ", num_height: " << num_height << endl;
//
//    // int st_width = 0;
//    // int st_height = 0;
//    
//    for (int i = start; i < end; i++) {
//        for (int j = 0; j < Wpin; j++) {
//
//            // cout << "st_width: " << st_width << ", st_height" << st_height << endl;
//
//            startv = static_cast<int>(i * intv);
//            startu = static_cast<int>(j * intv);
//
//            for (int s = 0; s < N; s++) {
//                for (int t = 0; t < N; t++) {
//
//                    if (img_display[startv + s][startu + t] == 0) {
//                        for (int zi = 0; zi < num_z_level; zi++) {
//
//                            if (num_idx[zi] > 0) {
//
//                                coef = static_cast<double>(10 * zi / D);
//                                newx = coef * ((t + 0.5) * pd - 0.5 * wp);
//                                newy = coef * ((s + 0.5) * pd - 0.5 * wp);
//                                newrange = coef * pd * 0.5;
//
//                                for (int k = 0; k < num_idx[zi]; k++) {
//                                    if (newx - newrange <= data[zi][num_idx[zi]][0] && data[zi][num_idx[zi]][0] < newx + newrange && newy - newrange <= data[zi][num_idx[zi]][1] && data[zi][num_idx[zi]][1] < newy + newrange) {
//                                        if (data[zi][num_idx[zi]][2] < val_z[startv + s][startu + t]) {
//                                            img_display[startv + s][startu + t] = static_cast<unsigned char>(data[zi][num_idx[zi]][3]) + static_cast<unsigned char>(data[zi][num_idx[zi]][4]) * 256 + static_cast<unsigned char>(data[zi][num_idx[zi]][5]) * 256 * 256;
//                                            val_z[startv + s][startu + t] = data[zi][num_idx[zi]][2];
//                                            flag = true;
//                                        }
//                                    }
//                                }
//                            }
//
//                            if (flag) {
//
//                                h_disparity = N * g / val_z[startv + s][startu + t];
//                                v_disparity = h_disparity;
//
//                                sum_h_disparity = 0;
//                                sum_v_disparity = 0;
//
//                                newx = (g * inv_d) * ((static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp + tmp_pcd_x * 1000);
//                                newy = (g * inv_d) * ((static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp + tmp_pcd_y * 1000);
//
//                                tmp_pcd_x = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//                                tmp_pcd_y = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//                                for (int k = i; k < end; k++) {
//                                    for (int l = j; l < Wpin; l++) {
//
//                                        startv2 = static_cast<int>(k * intv);
//                                        startu2 = static_cast<int>(l * intv);
//
//                                        nx = static_cast<int>(floor(tmp_pcd_x - sum_h_disparity));
//                                        ny = static_cast<int>(floor(tmp_pcd_y - sum_v_disparity));
//
//                                        if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                                            img_display[startv2 + ny][startu2 + nx] = img_display[startv + s][startu + t];
//                                        }
//                                        sum_h_disparity += h_disparity;
//                                    }
//                                    sum_v_disparity += v_disparity;
//                                    sum_h_disparity = 0;
//                                }
//                                sum_v_disparity = 0;
//
//                                flag = false;
//                                break;
//                            }
//
//                        }
//                    }
//                }
//            }
//            
//            // for (int s = static_cast<int>(delta_height / 2); s < HEIGHT - static_cast<int>(delta_height / 2); s++) {
//            //     for (int t = static_cast<int>(delta_width / 2); t < WIDTH - static_cast<int>(delta_width / 2); t++) {
//
//            //         tmp_pcd_x = data[s * WIDTH + t][0];
//            //         tmp_pcd_y = data[s * WIDTH + t][1];
//            //         tmp_pcd_z = data[s * WIDTH + t][2];
//            //         tmp_pcd_b = data[s * WIDTH + t][3];
//            //         tmp_pcd_g = data[s * WIDTH + t][4];
//            //         tmp_pcd_r = data[s * WIDTH + t][5];
//
//            //         inv_d = 1.0 / (tmp_pcd_z * 1000 + D);
//
//            //         newx = -(g * inv_d) * (uo[i * Wpin + j] - tmp_pcd_x * 1000);
//            //         newy = -(g * inv_d) * (vo[i * Hpin + j] - tmp_pcd_y * 1000);
//
//            //         nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//            //         ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//            //         // cout << "nx: " << nx << ", ny: " << ny << endl;
//
//            //         if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//            //             if (tmp_pcd_z < val_z[startv + ny][startu + nx]) {
//
//            //                 // cout << "yes" << endl;
//            //                 img_display[startv + ny][startu + nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256;
//            //                 val_z[startv + ny][startu + nx] = tmp_pcd_z;
//            //             }
//            //         }
//
//            //     }
//            // }
//
//            // st_width += num_width;
//        }
//        // st_height += num_height;
//        // st_width = 0;
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
//    cout << "vt-v8" << endl;
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//    cout << "wp:" << wp << ", N:" << N << endl;
//
//    Display *display;
//    Window window;
//    int screen;
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
//    // unsigned char *img_data = new unsigned char[pw_disp * ph_disp];
//    unsigned char *img_data;
//    img_data = (unsigned char*)malloc(sizeof(unsigned char) * pw_disp * ph_disp);
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
//    // cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    cv::Mat img = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//    // unsigned char* img_display = new unsigned char[pw_disp * ph_disp * 4];
//    // unsigned char* img = new unsigned char[(N * range) * (N * range) * 3];
//    int** img_display;
//    img_display = (int**)malloc(sizeof(int*) * ph_disp);
//    double** val_z;
//    val_z = (double**)malloc(sizeof(double*) * ph_disp);
//    for (int i = 0; i < ph_disp; i++) {
//        img_display[i] = (int*)malloc(sizeof(int) * pw_disp);
//        val_z[i] = (double*)malloc(sizeof(double) * pw_disp);
//    }
//    double*** new_data;
//    new_data = (double***)malloc(sizeof(double**) * num_z_level);
//    for (int i = 0; i < num_z_level; i++) {
//        new_data[i] = (double**)malloc(sizeof(double*) * WIDTH * HEIGHT);
//        for (int j = 0; j <  WIDTH * HEIGHT; j++){
//            new_data[i][j] = (double*)malloc(sizeof(double) * 6);
//        }
//    }
//    int* num_idx;
//    num_idx = (int*)malloc(sizeof(int) * num_z_level);
//    for (int i = 0; i < num_z_level; i++) {
//        num_idx[i] = 0;
//    }
//
//    double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z;
//    double newx, newy;
//    int z_level;
//
//    // int* img;
//    // img = (int*)malloc(sizeof(int) * (N * range) * (N * range));
//
//    // フレーム処理
//    for (int tt = 0; tt < 1; tt++) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                val_z[i][j] = 10;
//            }
//        }
//
//        // int nx, ny;
//
//        for (int k = 0; k < WIDTH * HEIGHT; k++) {
//
//            if (data[k][2] > 0) {
//                z_level = round(data[k][2] * 100);
//                // cout << "z_level:" << z_level << endl;
//                // if(z_level > max_z_level) {
//                //     max_z_level = z_level;
//                // }
//                new_data[z_level][num_idx[z_level]][0] = data[k][0];
//                new_data[z_level][num_idx[z_level]][1] = data[k][1];
//                new_data[z_level][num_idx[z_level]][2] = data[k][2];
//                new_data[z_level][num_idx[z_level]][3] = data[k][3];
//                new_data[z_level][num_idx[z_level]][4] = data[k][4];
//                new_data[z_level][num_idx[z_level]][5] = data[k][5];
//                num_idx[z_level]++;
//            }
//            // inv_d = 1.0 / tmp_pcd_z;
//
//            // newx = (g * inv_d) * ((static_cast<int>(floor((Wpin - 1) * 0.5)) + 0.5) * wp + tmp_pcd_x * 1000);
//            // newy = (g * inv_d) * ((static_cast<int>(floor((Hpin - 1) * 0.5)) + 0.5) * wp + tmp_pcd_y * 1000);
//
//            // data[k][0] = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//            // data[k][1] = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//            // data[k][2] = tmp_pcd_z;
//
//        }
//
//        cout << "ok" << endl;
//        // cout << "max_z_level:" << max_z_level << endl;
//
//        // for (int i = 0; i < ph_disp; i++) {
//        //     for (int j = 0; j < pw_disp; j++) {
//        //         val_z[i][j] = 10;
//        //     }
//        // }
//
//        insert_pixels(0, Hpin, std::ref(img_display), std::ref(new_data), std::ref(uo), std::ref(vo), std::ref(num_idx), std::ref(val_z));
//
//        // const int numThreads = 8;
//        // vector<thread> threads;
//        // int rowsPerThread = Hpin / numThreads;
//
//        // int startRow, endRow;
//        // for (int i = 0; i < numThreads; i++) {
//        //     startRow = i * rowsPerThread;
//        //     endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//        //     threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(data), std::ref(val_z), std::ref(uo), std::ref(vo));
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
//        // display = XOpenDisplay(nullptr);
//        // if (display == nullptr) {
//        //     std::cerr << "Cannot open display" << std::endl;
//        //     return 1;
//        // }
//
//        // screen = DefaultScreen(display);
//        // window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 500, 500, 1, BlackPixel(display, screen), WhitePixel(display, screen));
//        // XSelectInput(display, window, ExposureMask | KeyPressMask);
//        // XMapWindow(display, window);
//        
//        // // 画像データをXImageに変換
//        // XImage *image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, reinterpret_cast<char*>(img_data), pw_disp, ph_disp, 8, 0);
//
//        // for (int y = 0; y < ph_disp; y++) {
//        //     for (int x = 0; x < pw_disp; x++) {
//        //         XPutPixel(image, x, y, img_display[y][x]);
//        //     }
//        // }
//
//        // // メインループ
//        // XEvent e;
//        // while (true) {
//        //     XNextEvent(display, &e);
//        //     if (e.type == Expose) {
//        //         XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, pw_disp, ph_disp);
//        //     }
//        //     if (e.type == KeyPress)
//        //         break;
//        // }
//
//        // // クリーンアップ
//        // XFree(image);
//        // XDestroyWindow(display, window);
//        // XCloseDisplay(display);
//
//        // for (int i = 0; i < ph_disp; i++) {
//        //     for (int j = 0; j < pw_disp; j++) {
//        //         img.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i][j] / (256 * 256)));
//        //         img.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i][j] / 256 - img.at<cv::Vec3b>(i, j)[2] * 256));
//        //         img.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i][j] - (img.at<cv::Vec3b>(i, j)[2] * 256 - img.at<cv::Vec3b>(i, j)[1]) * 256));
//        //     }
//        // }
//
//        // ostringstream stream;
//        // stream << "mac_v8_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << "_delta_width" << delta_width << "_delta_height" << delta_height << ".png";
//
//        // cv::String filename = stream.str();
//
//        // imwrite(filename, img);
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
//    // free(elem_img);
//    // free(uo);
//    // free(vo);
//
//    // 使用したメモリを解放
//    // for (int i = 0; i < rows; ++i) {
//    //     cout << "calc finished" << endl;
//
//    //     delete[] data[i];
//    // }
//    // delete[] data;    
//    // delete[] img;
//    // delete[] val_z;
//    // delete[] img_data;
//    // delete[] img_display;
//    // free(img);
//    free(img_data);
//    for (int i = 0; i < ph_disp; i++) {
//        free(img_display[i]);
//        // free(val_z[i]);
//    }
//    free(img_display);
//    // free(val_z);
//
//
//
//    // // ウィンドウを閉じる
//    // cv::destroyAllWindows();
//
//
//    return EXIT_SUCCESS;
//}
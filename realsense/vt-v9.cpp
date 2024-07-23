///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* 点群データをツリー状に探索する*/
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
//#include <cmath>
//#include <time.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
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
//
//int block_size = 5;
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
//// void insert_pixels(int start, int end, double* uo, double* vo, double** data, int** img_display, double** val_z) {
//
////     double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
////     double newx, newy;
////     int nx, ny;
////     int startv, startu;
////     int inv_WIDTH = 1.0 / WIDTH;
////     double* vec;
////     vec = (double*)malloc(sizeof(double) * 6);
////     for (int k = start; k < end; k++) {
//
////         tmp_pcd_x = data[k][0] * 1000;
////         tmp_pcd_y = data[k][1] * 1000;
////         tmp_pcd_z = data[k][2] * 1000;
////         tmp_pcd_b = data[k][3];
////         tmp_pcd_g = data[k][4];
////         tmp_pcd_r = data[k][5];
//
////         //cout << "pcd0:" << tmp_pcd_x << ", pcd1:" << tmp_pcd_y << ", pcd2:" << tmp_pcd_z << ", pcd3:" << tmp_pcd_b << ", pcd4:" << tmp_pcd_g << ", pcd5:" << tmp_pcd_r << endl;
////         //    if (k % 1000 == 0) {
////         //        cout << "k:" << k << "\n" << endl;
////         //    }
//
////         inv_d = 1.0 / (tmp_pcd_z * 1000 + D);
//
////         for (int i = 0; i < Hpin; i++) {
////             for (int j = 0; j < Wpin; j++) {
//
////                 // 視点位置から点群データへのベクトル
////                 vec[0] = g / (tmp_pcd_z + D) * (tmp_pcd_x - uo[i * Wpin + j]); // x方向
////                 vec[1] = g / (tmp_pcd_z + D) * (tmp_pcd_y - vo[i * Wpin + j]); // y方向
////                 vec[2] = tmp_pcd_z + D; // z方向
//                
//
////                 for (int m = 0; m < N; m++) {
//
////                     // 視点位置から画素中心へのベクトル(x方向):wp / N * m + wp / N / 2 - wp / 2
////                     vec[3] = wp / (N * 2) * (m * 2 + 1 - N);
//
////                 }
//
////                 for (int n = 0; n < N; n++) {
//
////                     // 視点位置から画素中心へのベクトル(y方向):wp / N * m + wp / N / 2 - wp / 2
////                     vec[3] = wp / (N * 2) * (n * 2 + 1 - N);
//
////                 }
//
////                 //cout << "i:" << i << ", j:" << j << endl;
//
////                 newx = -(g * inv_d) * (uo[i * Wpin + j] - tmp_pcd_x * 1000);
////                 newy = -(g * inv_d) * (vo[i * Hpin + j] - tmp_pcd_y * 1000);
//
////                 nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
////                 ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
////                 startv = static_cast<int>(i * intv);
////                 startu = static_cast<int>(j * intv);
//
////                 //cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << endl;
////                 //cout << "nx:" << nx << ", ny:" << ny << endl;
//
////                 if (0 <= nx && nx < N && 0 <= ny && ny < N) {
////                     if (val_z[startv + ny][startu + nx] > tmp_pcd_z) {
//
////                         img_display[startv + ny][startu + nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256;
////                         val_z[startv + ny][startu + nx] = tmp_pcd_z;
////                     }
////                 }
////             }
////         }
////     }
//
////     // スレッドの終了をカウントし、条件変数を通知する
////     {
////         std::lock_guard<std::mutex> lock(mtx);
////         finished_threads++;
////         conv.notify_one(); // 1つの待機スレッドに通知
////     }
//
////     free(vec);
//
//// }
//
//void insert_pixels(int start, int end, int* img_display, double** data, double** val_z) {
//
//    int nx, ny;
//    int startv, startu;
//    double h_disparity, v_disparity, sum_h_disparity, sum_v_disparity;
//    double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//
//    for (int k = start; k < end; k++) {
//
//        tmp_pcd_x = data[k][0];
//        tmp_pcd_y = data[k][1];
//        tmp_pcd_z = data[k][2];
//        tmp_pcd_b = data[k][3];
//        tmp_pcd_g = data[k][4];
//        tmp_pcd_r = data[k][5];
//
//        h_disparity = N * g / tmp_pcd_z;
//        v_disparity = N * g / tmp_pcd_z;
//
//        // cout << "h disparity: " << h_disparity << ", v disparity: " << v_disparity << endl;
//
//        sum_h_disparity = 0;
//        sum_v_disparity = 0;
//
//        // cout << "type of sum h disparity: " << typeid(sum_h_disparity).name() << endl;
//
//        for (int i = 0; i < Hpin; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                startv = static_cast<int>(i * intv);
//                startu = static_cast<int>(j * intv);
//
//                nx = tmp_pcd_x - int(floor(sum_h_disparity));
//                ny = tmp_pcd_y - int(floor(sum_v_disparity));
//
//                // cout << "nx: " << nx << ", ny: " << ny << endl;
//
//                if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                    if (tmp_pcd_z < val_z[startv + ny][startu + nx]) {
//
//                        // cout << "yes" << endl;
//                        img_display[((startv + ny) * pw_disp + (startu + nx))] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256;
//                        val_z[startv + ny][startu + nx] = tmp_pcd_z;
//                    }
//                }
//
//                sum_h_disparity += h_disparity;
//            }
//            sum_v_disparity += v_disparity;
//            sum_h_disparity = 0;
//        }
//        sum_v_disparity = 0;
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
//int main(int argc, char* argv[])
//{
//
//    cout << "vt-v9" << endl;
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//
//    Display *display;
//    Window window;
//    int screen;
//
//    // unsigned char *img_data = new unsigned char[pw_disp * ph_disp];
//    int* img_display;
//    img_display = (int*)malloc(sizeof(int) * pw_disp * ph_disp);
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
//    unsigned char *img_data;
//    img_data = (unsigned char*)malloc(sizeof(unsigned char) * pw_disp * ph_disp);
//
//    double** val_z;
//    val_z = (double**)malloc(sizeof(double*) * ph_disp);
//    for (int i = 0; i < ph_disp; i++) {
//        val_z[i] = (double*)malloc(sizeof(double) * pw_disp);
//    }
//
//    cv::Mat img = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
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
//    double** sparse_data;
//    sparse_data = (double**)malloc(sizeof(double*) * (WIDTH * HEIGHT) / (block_size * block_size));
//    for (int i = 0; i < (WIDTH * HEIGHT) / (block_size * block_size); i++) {
//        sparse_data[i] = (double*)malloc(sizeof(double) * 6);
//    }
//
//    double sum_x, sum_y, sum_z, z_min;
//    int z_min_m, z_min_n;
//
//    for (int tt = 0; tt < 1; tt++) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                val_z[i][j] = 10000;
//            }
//        }
//
//        for (int k = 0; k < HEIGHT; k += block_size) {
//            for (int l = 0; l < WIDTH; l += block_size) {
//
//                sum_x = 0;
//                sum_y = 0;
//                sum_z = 0;
//
//                z_min_m = 0;
//                z_min_n = 0;
//                z_min = 10000;
//
//                for (int m = 0; m < block_size; m++) {
//                    for (int n = 0; n < block_size; n++) {
//
//                        sum_x += data[(k + m) * WIDTH + (l + n)][0];
//                        sum_y += data[(k + m) * WIDTH + (l + n)][1];
//                        sum_z += data[(k + m) * WIDTH + (l + n)][2] * 1000 + D;
//
//                        if (data[(k + m) * WIDTH + (l + n)][2] > 0 && data[(k + m) * WIDTH + (l + n)][2] < z_min) {
//                            z_min_m = m;
//                            z_min_n = n;
//                            z_min = data[(k + m) * WIDTH + (l + n)][2];
//                        }
//                    }
//                }
//
//                sparse_data[(k / block_size) * (WIDTH / block_size) + (l / block_size)][0] = sum_x / (block_size * block_size);
//                sparse_data[(k / block_size) * (WIDTH / block_size) + (l / block_size)][1] = sum_y / (block_size * block_size);
//                sparse_data[(k / block_size) * (WIDTH / block_size) + (l / block_size)][2] = sum_z / (block_size * block_size);
//
//                sparse_data[(k / block_size) * (WIDTH / block_size) + (l / block_size)][3] = data[(k + z_min_m) * WIDTH + (l + z_min_n)][3];
//                sparse_data[(k / block_size) * (WIDTH / block_size) + (l / block_size)][4] = data[(k + z_min_m) * WIDTH + (l + z_min_n)][4];
//                sparse_data[(k / block_size) * (WIDTH / block_size) + (l / block_size)][5] = data[(k + z_min_m) * WIDTH + (l + z_min_n)][5];
//
//            }
//        }
//
//        insert_pixels(0, (WIDTH * HEIGHT) / (block_size * block_size), std::ref(img_display), std::ref(sparse_data), std::ref(val_z));
//
//        // const int numThreads = 8;
//        // vector<thread> threads;
//        // int rowsPerThread = ((WIDTH * HEIGHT) / (block_size * block_size)) / numThreads;
//
//        // int startRow, endRow;
//        // for (int i = 0; i < numThreads; i++) {
//        //     startRow = i * rowsPerThread;
//        //     endRow = (i == numThreads - 1) ? ((WIDTH * HEIGHT) / (block_size * block_size)) : (i + 1) * rowsPerThread;
//        //     threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(sparse_data), std::ref(val_z));
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
//        // for (int i = 1; i < ph_disp - 1; i++) {
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
//        // }
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // for (int k = 0; k < HEIGHT / block_size; k++) {
//        //     for (int l = 0; l < WIDTH / block_size; l++) {
//
//        //         cout << "x of sparse_data[" << k << "][" << l << "]:" << sparse_data[k * (WIDTH / block_size) + l][0] << endl;
//        //         cout << "y of sparse_data[" << k << "][" << l << "]:" << sparse_data[k * (WIDTH / block_size) + l][1] << endl;
//        //         cout << "z of sparse_data[" << k << "][" << l << "]:" << sparse_data[k * (WIDTH / block_size) + l][2] << endl;
//        //         cout << "b of sparse_data[" << k << "][" << l << "]:" << sparse_data[k * (WIDTH / block_size) + l][3] << endl;
//        //         cout << "g of sparse_data[" << k << "][" << l << "]:" << sparse_data[k * (WIDTH / block_size) + l][4] << endl;
//        //         cout << "r of sparse_data[" << k << "][" << l << "]:" << sparse_data[k * (WIDTH / block_size) + l][5] << endl;
//
//        //         // if (sparse_data[k * (WIDTH / block_size) + l][0] == 0 && sparse_data[k * (WIDTH / block_size) + l][1] == 0 && sparse_data[k * (WIDTH / block_size) + l][2] == 0) {
//        //         //     cout << "wow" << endl;
//        //         // }
//        //     }
//        // }
//
//        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        std::cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        display = XOpenDisplay(nullptr);
//        if (display == nullptr) {
//            std::cerr << "Cannot open display" << std::endl;
//            return 1;
//        }
//
//        screen = DefaultScreen(display);
//        window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 500, 500, 1, BlackPixel(display, screen), WhitePixel(display, screen));
//        XSelectInput(display, window, ExposureMask | KeyPressMask);
//        XMapWindow(display, window);
//        
//        // 画像データをXImageに変換
//        XImage *image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, reinterpret_cast<char*>(img_data), pw_disp, ph_disp, 8, 0);
//
//        for (int y = 0; y < ph_disp; y++) {
//            for (int x = 0; x < pw_disp; x++) {
//                XPutPixel(image, x, y, img_display[y * pw_disp + x]);
//            }
//        }
//        // メインループ
//        XEvent e;
//        while (true) {
//            XNextEvent(display, &e);
//            if (e.type == Expose) {
//                XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, pw_disp, ph_disp);
//            }
//            if (e.type == KeyPress)
//                break;
//        }
//
//        // クリーンアップ
//        XFree(image);
//        XDestroyWindow(display, window);
//        XCloseDisplay(display);
//
//        for (int i = 0; i < ph_disp; i++) {
//            for (int j = 0; j < pw_disp; j++) {
//                img.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i * pw_disp + j] / (256 * 256)));
//                img.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i * pw_disp + j] / 256 - img.at<cv::Vec3b>(i, j)[2] * 256));
//                img.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i * pw_disp + j] - (img.at<cv::Vec3b>(i, j)[2] * 256 - img.at<cv::Vec3b>(i, j)[1]) * 256));
//            }
//        }
//
//        ostringstream stream;
//        stream << "mac_v9_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png"; // �����_�ȉ�2���Ő؂�̂�
//        cv::String filename = stream.str();
//
//        imwrite(filename, img);
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
//    // free(uo);
//    // free(vo);
//    // 使用したメモリを解放
//    for (int i = 0; i < rows; ++i) {
//        delete[] data[i];
//    }
//    delete[] data;
//    
//    for (int k = 0; k < (WIDTH * HEIGHT) / (block_size * block_size); k++) {
//        free(sparse_data[k]);
//    }
//    free(sparse_data);
//
//    for (int i = 0; i < ph_disp; ++i) {
//        // free(img_display[i]);
//        free(val_z[i]);
//    }
//    free(img_display);
//    free(val_z);
//    free(img_data);
//
//    // // ウィンドウを閉じる
//    // cv::destroyAllWindows();
//
//
//    return EXIT_SUCCESS;
//}
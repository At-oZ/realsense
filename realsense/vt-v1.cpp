///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
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
//
//using namespace std;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
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
//    return i;	                // 配列の長さを返す
//}
//
//void insert_pixels(int WIDTH, int HEIGHT, int Wpin, int N, int D, double g, double wp, double inv_wp, double intv, int starty, int endy, double* uo, double* vo, double** data, cv::Mat img_display, double* val_z) {
//
//    double inv_d, tmp_pcd0, tmp_pcd1, tmp_pcd2, tmp_pcd3, tmp_pcd4, tmp_pcd5;
//    double newx, newy;
//    int nx, ny;
//    int startv, startu;
//    int pcd_size = WIDTH * HEIGHT;
//    int inv_WIDTH = 1.0 / WIDTH;
//    for (int k = 0; k < pcd_size; k++) {
//
//        tmp_pcd0 = data[k][0];
//        tmp_pcd1 = data[k][1];
//        tmp_pcd2 = data[k][2];
//        tmp_pcd3 = data[k][3];
//        tmp_pcd4 = data[k][4];
//        tmp_pcd5 = data[k][5];
//
//        //cout << "pcd0:" << tmp_pcd0 << ", pcd1:" << tmp_pcd1 << ", pcd2:" << tmp_pcd2 << ", pcd3:" << tmp_pcd3 << ", pcd4:" << tmp_pcd4 << ", pcd5:" << tmp_pcd5 << endl;
//        //    if (k % 1000 == 0) {
//        //        cout << "k:" << k << "\n" << endl;
//        //    }
//
//        //vector<vector<double>> newx(endy - starty, vector<double>(Wpin));
//        //vector<vector<double>> newy(endy - starty, vector<double>(Wpin));
//
//        //vector<vector<int>> nx(endy - starty, vector<int>(Wpin));
//        //vector<vector<int>> ny(endy - starty, vector<int>(Wpin));
//
//        inv_d = 1.0 / (tmp_pcd2 * 1000 + D);
//
//        for (int i = starty; i < endy; i++) {
//            for (int j = 0; j < Wpin; j++) {
//
//                //cout << "i:" << i << ", j:" << j << endl;
//
//                newx = -(g * inv_d) * (uo[i * Wpin + j] - tmp_pcd0 * 1000);
//                newy = -(g * inv_d) * (vo[i * Wpin + j] - tmp_pcd1 * 1000);
//
//                nx = static_cast<int>(floor((newx + 0.5 * wp) * inv_wp * N));
//                ny = static_cast<int>(floor((newy + 0.5 * wp) * inv_wp * N));
//
//                startv = static_cast<int>(i * intv);
//                startu = static_cast<int>(j * intv);
//
//                //cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << endl;
//                //cout << "nx:" << nx << ", ny:" << ny << endl;
//
//                if (0 <= nx && nx < N && 0 <= ny && ny < N) {
//                    if (val_z[((i * Wpin + j) * N + ny) * N + nx] > tmp_pcd2) {
//
//                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[0] = static_cast<int>(tmp_pcd3);
//                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[1] = static_cast<int>(tmp_pcd4);
//                        img_display.at<cv::Vec3b>(startv + ny, startu + nx)[2] = static_cast<int>(tmp_pcd5);
//                        val_z[((i * Wpin + j) * N + ny) * N + nx] = tmp_pcd2;
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
//int main(int argc, char* argv[])
//{
//
//    cout << "vt-v1" << endl;
//
//    double start, finish;
//    clock_t cpu_time;
//
//    // 表示系のパラメータ(mm)
//    int W = 170, H = 170;                                                                       // 表示の縦横幅
//    double g = 4, wp = 1.25, a = 0.125;                                                          // ギャップ、ピッチ、ピンホール幅
//    double ld = 13.4 * 25.4;                                                                    // ディスプレイサイズ
//    int pw = 3840, ph = 2400;                                                                   // ディスプレイの縦横の解像度
//    double pd = ld / sqrtf(pw * pw + ph * ph);                                                  // 画素ピッチ
//    int N = static_cast<int>(floor(wp / pd));                                                   // 要素画像の解像度
//    int pw_disp = static_cast<int>(floor(W / pd)), ph_disp = static_cast<int>(floor(H / pd));   // 表示画像の縦横の解像度
//    int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp));         // 縦横のピンホール数
//    double intv = wp / pd;                                                                      // 要素画像の間隔
//
//    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
//
//    // 要素画像群
//    //vector<vector<cv::Mat>> elem_img(Hpin, vector<cv::Mat>(Wpin));
//    int* elem_img;
//    elem_img = (int*)calloc(Hpin * Wpin * N * N * 3, sizeof(int));
//
//    // 視点パラメータ(mm)
//    //vector<vector<double>> uo(Hpin, vector<double>(Wpin));
//    //vector<vector<double>> vo(Hpin, vector<double>(Wpin));
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
//    double* val_z;
//    val_z = (double*)malloc(sizeof(double) * Hpin * Wpin * N * N);
//
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//
//    int D = 100;                     // 表示系とカメラとの距離(mm)
//    double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
//    double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
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
//    cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
//
//    for (int tt = 0; tt < 1; tt++) {
//
//        for (int i = 0; i < Hpin * Wpin * N * N; i++) {
//            val_z[i] = 10;
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
//        insert_pixels(WIDTH, HEIGHT, Wpin, N, D, g, wp, inv_wp, intv, 0, Hpin, uo, vo, std::ref(data), std::ref(img_display), val_z);
//
//        //const int numThreads = 16;
//        //vector<thread> threads;
//        //int rowsPerThread = Hpin / numThreads;
//
//        //int startRow, endRow;
//        //for (int i = 0; i < numThreads; i++) {
//        //    startRow = i * rowsPerThread;
//        //    endRow = (i == numThreads - 1) ? Hpin : (i + 1) * rowsPerThread;
//        //    threads.emplace_back(insert_pixels, WIDTH, HEIGHT, Wpin, N, D, g, wp, inv_wp, intv, startRow, endRow, uo, vo, std::ref(data), std::ref(img_display), val_z);
//        //}
//
//        //for (auto& t : threads) {
//        //    if (t.joinable()) { t.join(); }
//        //}
//
//        //// 全てのスレッドが終了するのを待つ
//        //{
//        //    std::unique_lock<std::mutex> lock(mtx);
//        //    conv.wait(lock, [] {return finished_threads == numThreads; });
//        //}
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
//        std::cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        ostringstream stream;
//        stream << "./images/v1_img_display_g" << g << "_wp" << fixed << setprecision(1) << wp << "_pd" << fixed << setprecision(3) << pd << "_D" << D << ".png"; // �����_�ȉ�2���Ő؂�̂�
//        cv::String filename = stream.str();
//
//        imwrite(filename, img_display);
//
//        // 画像を表示
//        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//        cv::imshow("Display window", img_display);
//
//        // キーボードの入力を待つ（1ms）
//        // 何かしらのキーが押されたらループを抜ける
//        cv::waitKey(0);
//        cv::destroyWindow("Display window");
//
//    }
//
//    free(elem_img);
//    free(uo);
//    free(vo);
//    //free(raw_pointclouddata);
//    //free(pointclouddata);
//    // 使用したメモリを解放
//    for (int i = 0; i < rows; ++i) {
//        delete[] data[i];
//    }
//    delete[] data;
//
//    // ウィンドウを閉じる
//    cv::destroyAllWindows();
//
//
//    return EXIT_SUCCESS;
//}
/* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
/* 差分:変換式を究極までシンプルに(変換式は正しいことを確認済み)*/

// #include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>
#include <algorithm>            // min, max
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <time.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//
// #include </opt/homebrew/Cellar/gcc/13.2.0/lib/gcc/current/gcc/aarch64-apple-darwin22/13/include/omp.h>

using namespace std;
using namespace cv;

std::mutex mtx;
std::condition_variable conv;
int finished_threads = 0; // 終了したスレッドの数

// 表示系のパラメータ(mm)
int W = 170, H = 170;                                                                       // 表示の縦横幅
double g = 4.0, wp = 1.25, a = 0.125;                                                       // ギャップ、ピッチ、ピンホール幅
double ld = 13.4 * 25.4;                                                                    // ディスプレイサイズ
int pw = 3840, ph = 2400;                                                                   // ディスプレイの縦横の解像度
double pd = ld / sqrtf(pw * pw + ph * ph);                                                  // 画素ピッチ
int N = static_cast<int>(floor(wp / pd));                                                   // 要素画像の解像度
int pw_disp = static_cast<int>(floor(W / pd)), ph_disp = static_cast<int>(floor(H / pd));   // 表示画像の縦横の解像度
int Wpin = static_cast<int>(floor(W / wp)), Hpin = static_cast<int>(floor(H / wp));         // 縦横のピンホール数
double intv = wp / pd;                                                                      // 要素画像の間隔

int WIDTH = 640;
int HEIGHT = 480;
int FPS = 30;

// 点群データ配列の行数と列数
int rows = WIDTH * HEIGHT;
int cols = 6;


int D = 100;                     // 表示系とカメラとの距離(mm)
double inv_wp = 1.0 / wp;         // ピンホールピッチの逆数
double inv_WIDTH = 1.0 / WIDTH;   // WIDTHの逆数
double inv_pd = 1.0 / pd;         // pdの逆数

// パラメータ
int num_z_level = 65; // イメージプレーンの層数
double ptimes = 1; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
int box_size = 1; // 探索範囲

int Ddash = 250;
double coef = (double)num_z_level * (double)Ddash;

double img_pitch = ptimes * pd;

int px_height_img = static_cast<int>(round(55 / ptimes));
int px_width_img = static_cast<int>(round(55 / ptimes));

// ファイル読み込み
// 引数で配列のポインタを受け取りそこに値を書き込み
int fileread(double* test, string fname) {
    ifstream fin(fname);
    if (!fin) {
        cout << "ファイルをオープンできませんでした。\n";
        getchar();
        exit(0);		// プログラムの終了
    }
    else cout << "ファイルをオープンしました。\n";

    // eofが検出されるまで配列に書き込む
    int i = 1;
    while (1) {
        fin >> test[i];
        if (fin.eof())	break;
        i++;
    }
    fin.close();
    cout << "ファイルをクローズしました。\n";

    return i;   // 配列の長さを返す
}

void insert_pixels(int start, int end, int** img_display, int*** pcd_box, double* sp, double* tp) {

    int nx, ny, tmp_pcd_color, sum_zi, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
    int alpha = 256 * 256 * 256;
    bool flag1 = false;
    // bool flag2 = false;
    int startu, startv;
    double u, v, xt, yt, zt;

    // 各要素画像の各画素ごとに
    sum_zi = 0;
    int sum_pcd_r = 0;
    int sum_pcd_g = 0;
    int sum_pcd_b = 0;
    int num_pcd = 0;

    for (int m = start; m < end; m++) {

        v = ((double)m - (double)(N - 1) * 0.5) * pd;

        for (int n = 0; n < N; n++) {

            u = ((double)n - (double)(N - 1) * 0.5) * pd;

            // 各要素カメラごとに
            for (int i = 0; i < Hpin; i++) {

                // 表示画像中における要素画像の左上画素の画素番号
                startv = static_cast<int>(i * intv);

                for (int j = 0; j < Wpin; j++) {

                    // 表示画像中における要素画像の左上画素の画素番号
                    startu = static_cast<int>(j * intv);

                    // 各奥行きレベルごとに(手前から)
                    for (int nz = num_z_level - 1; nz >= 0; nz--) {

                        zt = (double)nz / coef;
                        xt = sp[i * Wpin + j] * zt + u / g;
                        yt = tp[i * Wpin + j] * zt + v / g;

                        nx = static_cast<int>(floor((g / img_pitch) * xt + 0.5) + px_width_img * 0.5);
                        ny = static_cast<int>(floor((g / img_pitch) * yt + 0.5) + px_height_img * 0.5);

                         //cout << "nx:" << nx << ", ny:" << ny << endl;
                        if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img) {
                            tmp_pcd_color = pcd_box[nz][ny][nx];

                            if (tmp_pcd_color > 0) {
                                 img_display[startv + m][startu + n] = tmp_pcd_color - alpha;

                                // cout << "v, u:" << startv + m << ", " << startu + n << endl;
                                break;
                            }
                            //else {
                            //    for (int s = 1; s < static_cast<int>((box_size - 1) / 2); s++) {

                            //        // 上辺
                            //        if (0 <= ny - s && ny - s < px_height_img) {
                            //            for (int k = 0; k < box_size; k++) {
                            //                if (0 <= nx - s + k && nx - s + k < px_width_img) {
                            //                    if (pcd_box[nz][ny - s][nx - s + k] > 0) {
                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny - s][nx - s + k] - alpha;
                            //                        flag1 = true;
                            //                    }
                            //                }
                            //            }
                            //        }

                            //        if (flag1) break;

                            //        // 右辺
                            //        if (0 <= nx + s && nx + s < px_width_img) {
                            //            for (int k = 1; k < box_size; k++) {
                            //                if (0 <= ny - s + k && ny - s + k < px_height_img) {
                            //                    if (pcd_box[nz][ny - s + k][nx + s] > 0) {
                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny - s + k][nx + s] - alpha;
                            //                        flag1 = true;
                            //                    }
                            //                }
                            //            }
                            //        }

                            //        if (flag1) break;

                            //        //下辺
                            //        if (0 <= ny + s && ny + s < px_height_img) {
                            //            for (int k = 1; k < box_size; k++) {
                            //                if (0 <= nx + s - k && nx + s - k < px_width_img) {
                            //                    if (pcd_box[nz][ny + s][nx + s - k] > 0) {
                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny + s][nx + s - k] - alpha;
                            //                        flag1 = true;
                            //                    }
                            //                }
                            //            }
                            //        }

                            //        if (flag1) break;

                            //        // 左辺
                            //        if (0 <= nx - s && nx - s < px_width_img) {
                            //            for (int k = 1; k < box_size - 1; k++) {
                            //                if (0 <= ny + s - k && ny + s - k < px_height_img) {
                            //                    if (pcd_box[nz][ny + s - k][nx - s] > 0) {
                            //                        img_display[startv + m][startu + n] = pcd_box[nz][ny + s - k][nx - s] - alpha;
                            //                        flag1 = true;
                            //                    }
                            //                }
                            //            }
                            //        }

                            //        if (flag1) break;
                            //    }


                            //    if (flag1) {
                            //        flag1 = false;
                            //        break;
                            //    }

                            //}

                        }

                        //xt -= tmp_sp * coef;
                        //yt -= tmp_tp * coef;
                        //zt -= coef;

                    }

                }
            }
        }
    }

    // sum_zi = sum_zi / (2 * 16 * 136 * 136);
    // cout << "average of zi:" << sum_zi << endl;

    // スレッドの終了をカウントし、条件変数を通知する
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished_threads++;
        conv.notify_one(); // 1つの待機スレッドに通知
    }
}

double getPSNR(const Mat& I1, const Mat& I2) {
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // Convert to float
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);        // Sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // Sum channels

    if (sse <= 1e-10) {        // For small values return zero
        return 0;
    }
    else {
        double mse = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

Mat getDiff(const Mat& I1, const Mat& I2) {
    Mat s1;
    absdiff(I1, I2, s1);
    s1.convertTo(s1, CV_8U);  // Convert to 8-bit
    return s1;
}

Mat readImage(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return Mat();
    }

    std::vector<uchar> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return imdecode(buffer, IMREAD_COLOR);
}


int main(int argc, char* argv[])
{

    cout << "vt-v12-1" << endl;

    double start, finish;
    clock_t cpu_time;

    cout << "pw_disp:" << pw_disp << ", ph_disp:" << ph_disp << endl;
    cout << "wp:" << wp << endl;

    //Display* display;
    //Window window;
    //int screen;

    unsigned char* img_data;
    img_data = (unsigned char*)malloc(sizeof(unsigned char) * pw_disp * ph_disp);

    cv::Mat img_save = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);

    // 視点パラメータ(mm)
    double* sp;
    double* tp;
    sp = (double*)malloc(sizeof(double) * Hpin * Wpin);
    tp = (double*)malloc(sizeof(double) * Hpin * Wpin);
    for (int i = 0; i < Hpin; i++) {
        for (int j = 0; j < Wpin; j++) {

            sp[i * Wpin + j] = (j - (Wpin - 1) * 0.5) * wp;
            tp[i * Wpin + j] = (i - (Hpin - 1) * 0.5) * wp;

            //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;

        }
    }

    int** img_display;
    img_display = (int**)malloc(sizeof(int*) * ph_disp);
    for (int i = 0; i < ph_disp; i++) {
        img_display[i] = (int*)malloc(sizeof(int) * pw_disp);
    }

    // CSVファイルのパス
    std::string filePath = "output.csv";

    // double型の二次元配列を動的に確保
    double** data = new double* [rows];
    for (int i = 0; i < rows; ++i) {
        data[i] = new double[cols];
    }

    // ファイルを開く
    std::ifstream file(filePath);

    // ファイルが正常に開かれたか確認
    if (file.is_open()) {
        std::string line;
        int row = 0;
        // ファイルから一行ずつ読み込む
        while (getline(file, line) && row < rows) {
            std::stringstream ss(line);
            std::string value;
            int col = 0;
            while (getline(ss, value, ',') && col < cols) {
                // 文字列をdouble型に変換して配列に格納
                data[row][col++] = std::stod(value);
            }
            ++row;
        }
        file.close(); // ファイルを閉じる
    }
    else {
        std::cerr << "ファイルを開けませんでした。" << std::endl;
        // メモリリークを防ぐためにメモリを解放
        for (int i = 0; i < rows; ++i) {
            delete[] data[i];
        }
        delete[] data;
        return 1; // エラーコード1で終了
    }

    // cv::Mat img_display = cv::Mat::zeros(cv::Size(pw_disp, ph_disp), CV_8UC3);
    cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
    // unsigned char* img_display = new unsigned char[pw_disp * ph_disp * 4];
    // unsigned char* img = new unsigned char[(N * range) * (N * range) * 3];
    // int* img;
    // img = (int*)malloc(sizeof(int) * (N * range) * (N * range));


    int*** pcd_box;
    double*** val_z;
    pcd_box = (int***)malloc(sizeof(int**) * num_z_level);
    val_z = (double***)malloc(sizeof(double**) * num_z_level);
    for (int i = 0; i < num_z_level; i++) {
        pcd_box[i] = (int**)malloc(sizeof(int*) * px_height_img);
        val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
        for (int j = 0; j < px_height_img; j++) {
            pcd_box[i][j] = (int*)malloc(sizeof(int) * px_width_img);
            val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
        }
    }

    // // 箱の全引き出しに白を格納（テスト用）
    // for (int i = 0; i < num_z_level; i++) {
    //     for (int j = 0; j < px_height_img; j++) {
    //         for (int k = 0; k < px_width_img; k++) {
    //             pcd_box[i][j][k] = 255 + 255 * 256 + 255 * 256 * 256 + 256 * 256 * 256;
    //         }
    //     }
    // }

    // フレーム処理
    for (int tt = 0; tt < 1; tt++) {

        // 測定開始時刻を記録
        auto start = std::chrono::high_resolution_clock::now();

        // 箱とzバッファをオフセット
        for (int i = 0; i < num_z_level; i++) {
            for (int j = 0; j < px_height_img; j++) {
                for (int k = 0; k < px_width_img; k++) {
                    //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
                    pcd_box[i][j][k] = 0;
                    val_z[i][j][k] = 0;
                }
            }
        }

        for (int i = 0; i < ph_disp; i++) {
            for (int j = 0; j < pw_disp; j++) {
                img_display[i][j] = 0;
            }
        }

        //int startu, startv;
        //for (int i = 0; i < Hpin; i++) {
        //    startv = static_cast<int>(i * intv);
        //    for (int j = 0; j < pw_disp; j++) {
        //        img_display[startv][j] = 64 + 64 * 256 + 64 * 256 * 256;
        //    }
        //}
        //for (int i = 0; i < Wpin; i++) {
        //    startu = static_cast<int>(i * intv);
        //    for (int j = 0; j < ph_disp; j++) {
        //        img_display[j][startu] = 64 + 64 * 256 + 64 * 256 * 256;
        //    }
        //}

        double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
        double xt, yt, zt;
        int nx, ny, nz;
        int pcd_count = 0;

         //点群を箱に格納
        for (int k = 0; k < rows; k++) {

            tmp_pcd_x = data[k][0] * 1000.0;
            tmp_pcd_y = data[k][1] * 1000.0;
            tmp_pcd_z = data[k][2] * 1000.0 + (double)D;
            tmp_pcd_b = data[k][3];
            tmp_pcd_g = data[k][4];
            tmp_pcd_r = data[k][5];

            zt = 1.0 / tmp_pcd_z;
            xt = tmp_pcd_x * zt;
            yt = tmp_pcd_y * zt;

            nx = static_cast<int>(floor((g / img_pitch) * xt + 0.5) + px_width_img * 0.5);
            ny = static_cast<int>(floor((g / img_pitch) * yt + 0.5) + px_height_img * 0.5);
            nz = static_cast<int>(floor (coef * zt + 0.5));

             //cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;

            if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img && 0 <= nz && nz < num_z_level) {
                //if (pcd_box[nz][ny][nx] > 0) {
                //    pcd_count++;
                //}

                if (val_z[nz][ny][nx] < zt) {
                    pcd_box[nz][ny][nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256 + 256 * 256 * 256;
                    val_z[nz][ny][nx] = zt;
                }
                //pcd_box[nz][ny][nx] = static_cast<unsigned char>(tmp_pcd_b) + static_cast<unsigned char>(tmp_pcd_g) * 256 + static_cast<unsigned char>(tmp_pcd_r) * 256 * 256 + 256 * 256 * 256;

                // cout << "pcd_box:" << pcd_box[nz][ny][nx] << endl;
            }
        }

        //insert_pixels(0, N, std::ref(img_display), std::ref(pcd_box), std::ref(sp), std::ref(tp));

        const int numThreads = 16;
        vector<thread> threads;
        int rowsPerThread = N / numThreads;

        int startRow, endRow;
        for (int i = 0; i < numThreads; i++) {
            startRow = i * rowsPerThread;
            endRow = (i == numThreads - 1) ? N : (i + 1) * rowsPerThread;
            threads.emplace_back(insert_pixels, startRow, endRow, std::ref(img_display), std::ref(pcd_box), std::ref(sp), std::ref(tp));
        }

        for (auto& t : threads) {
            if (t.joinable()) { t.join(); }
        }

        // 全てのスレッドが終了するのを待つ
        {
            std::unique_lock<std::mutex> lock(mtx);
            conv.wait(lock, [] {return finished_threads == numThreads; });
        }

        cout << "calc finished" << endl;
        finished_threads = 0;

        // 測定終了時刻を記録
        auto end = std::chrono::high_resolution_clock::now();

        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        cout << "実行時間: " << duration.count() << " ms" << std::endl;
        //cout << "重なり回数:" << pcd_count << endl;

        //display = XOpenDisplay(nullptr);
        //if (display == nullptr) {
        //    std::cerr << "Cannot open display" << std::endl;
        //    return 1;
        //}

        //screen = DefaultScreen(display);
        //window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 500, 500, 1, BlackPixel(display, screen), WhitePixel(display, screen));
        //XSelectInput(display, window, ExposureMask | KeyPressMask);
        //XMapWindow(display, window);

        //// 画像データをXImageに変換
        //XImage* image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, reinterpret_cast<char*>(img_data), pw_disp, ph_disp, 8, 0);

        //for (int y = 0; y < ph_disp; y++) {
        //    for (int x = 0; x < pw_disp; x++) {
        //        XPutPixel(image, x, y, img_display[y][x]);
        //    }
        //}

        //// メインループ
        //XEvent e;
        //while (true) {
        //    XNextEvent(display, &e);
        //    if (e.type == Expose) {
        //        XPutImage(display, window, DefaultGC(display, screen), image, 0, 0, 0, 0, pw_disp, ph_disp);
        //    }
        //    if (e.type == KeyPress)
        //        break;
        //}

        //// クリーンアップ
        //XFree(image);
        //XDestroyWindow(display, window);
        //XCloseDisplay(display);

        //for (int i = 0; i < ph_disp; i++) {
        //    for (int j = 0; j < pw_disp; j++) {

        //        img_save.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor(img_display[i][j] / (256 * 256)));
        //        img_save.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor(img_display[i][j] / 256 - img_save.at<cv::Vec3b>(i, j)[2] * 256));
        //        img_save.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor(img_display[i][j] - (img_save.at<cv::Vec3b>(i, j)[2] * 256 - img_save.at<cv::Vec3b>(i, j)[1]) * 256));
        //    }
        //}

        //ostringstream stream;
        //stream << "./images/v11-6_1_ImgDisplay_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << "_boxSize" << box_size << ".png";
        //cv::String filename = stream.str();
        //imwrite(filename, img_save);

        //stream.str("");
        //stream.clear(ostringstream::goodbit);

        //// 各イメージプレーンの画像を保存（テスト用）
        //// ostringstream stream;
        //// cv::String filename;
        //for (int zi = 0; zi < num_z_level; zi++) {

        //    stream << "./images/imagePlane/v11-5_imagePlane_NumZLevel" << num_z_level << "_zi" << zi << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << ".png";
        //    cout << "zi:" << zi << endl;

        //    for (int i = 0; i < px_height_img; i++) {
        //        for (int j = 0; j < px_width_img; j++) {
        //            if (pcd_box[zi][i][j] > 0) {
        //                img.at<cv::Vec3b>(i, j)[2] = static_cast<int>(floor((pcd_box[zi][i][j] - 256 * 256 * 256) / (256 * 256)));
        //                img.at<cv::Vec3b>(i, j)[1] = static_cast<int>(floor((pcd_box[zi][i][j] - 256 * 256 * 256) / 256 - img.at<cv::Vec3b>(i, j)[2] * 256));
        //                img.at<cv::Vec3b>(i, j)[0] = static_cast<int>(floor((pcd_box[zi][i][j] - 256 * 256 * 256) - (img.at<cv::Vec3b>(i, j)[2] * 256 - img.at<cv::Vec3b>(i, j)[1]) * 256));
        //            }
        //        }
        //    }
        //    filename = stream.str();
        //    imwrite(filename, img);
        //    stream.str("");
        //    stream.clear(ostringstream::goodbit);
        //    img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);

        //}


    }

    //try {
    //    std::string filename_original, filename_compared;

    //    std::cout << "Please enter the name of original image file: ";
    //    std::getline(std::cin, filename_original);
    //    std::cout << "Please enter the name of compared image file: ";
    //    std::getline(std::cin, filename_compared);

    //    Mat img1 = readImage(filename_original);
    //    Mat img2 = readImage(filename_compared);

    //    if (img1.empty()) {
    //        cout << "Could not open or find the image: " << filename_original << endl;
    //        return -1;
    //    }
    //    if (img2.empty()) {
    //        cout << "Could not open or find the image: " << filename_compared << endl;
    //        return -1;
    //    }

    //    double psnrValue = getPSNR(img1, img2);
    //    cout << "PSNR value is: " << psnrValue << " dB" << endl;

    //    Mat img_diff = getDiff(img1, img2);

    //    // img_diffをファイルとして保存して内容を確認する
    //    imwrite("img_diff.png", img_diff);

    //    // img_diffのコントラストを強調する
    //    Mat img_diff_enhanced;
    //    normalize(img_diff, img_diff_enhanced, 0, 255, NORM_MINMAX, CV_8U);

    //    //cv::namedWindow("Difference image", cv::WINDOW_AUTOSIZE);
    //    //cv::imshow("Difference image", img_diff_enhanced);

    //    //cv::waitKey(0);
    //    //cv::destroyWindow("Difference image");

    //    ostringstream stream;
    //    stream << "./images/v1-3v_v11-6v_diff_img.png";
    //    cv::String filename = stream.str();

    //    imwrite(filename, img_diff);

    //}
    //catch (cv::Exception& e) {
    //    cout << "OpenCV exception: " << e.what() << endl;
    //    return -1;
    //}
    //catch (std::exception& e) {
    //    cout << "Standard exception: " << e.what() << endl;
    //    return -1;
    //}
    //catch (...) {
    //    cout << "Unknown exception occurred." << endl;
    //    return -1;
    //}

    free(sp);
    free(tp);

    // 使用したメモリを解放
    // for (int i = 0; i < rows; ++i) {
    //     cout << "calc finished" << endl;

    //     delete[] data[i];
    // }
    // delete[] data;    

    for (int i = 0; i < ph_disp; i++) {
        free(img_display[i]);
    }
    free(img_display);

    for (int i = 0; i < num_z_level; i++) {
        for (int j = 0; j < px_height_img; j++) {
            free(pcd_box[i][j]);
        }
        free(pcd_box[i]);
    }
    free(pcd_box);

    return EXIT_SUCCESS;
}
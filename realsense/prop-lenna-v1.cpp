///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* lenna画像を用いたプログラム */
///* v1-7と同じアルゴリズム */
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
//// ディスプレイのパラメータ(mm)
//double display_size = 13.4 * 25.4;                    // ディスプレイサイズ
//int display_width_px = 3840, display_height_px = 2400;                   // ディスプレイの縦横の解像度
//double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ
//
//// ピンホールアレイのパラメータ(mm)
//double pinhole_array_size = display_pixel_pitch * display_height_px;   // 各軸方向のピンホールアレイのサイズ
//int num_pinhole = 100;  // 各軸方向のピンホール数
//double pinhole_pitch = pinhole_array_size / num_pinhole;    // ピンホールピッチ
//
//// 表示系のパラメータ(mm)
//double gap = 4.0; // ギャップ
//int element_image_px = static_cast<int>(floor(pinhole_pitch / display_pixel_pitch)); // 要素画像の解像度
//int display_px = 2400; // 各軸方向の表示画像の解像度
//double intv = pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//
//// 被写体のパラメータ(mm)
//int subject_image_resolution = 554; // 被写体の解像度
//double subject_size = pinhole_array_size; // 被写体のサイズ
//double subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
//double subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
//double subject_z = 256.0; // 被写体の奥行き方向の位置
//
//// 点群データ配列の行数と列数
//int rows = subject_image_resolution * subject_image_resolution;
//int cols = 6;
//
//// パラメータ
//int num_z_level = 160; // イメージプレーンの層数
//double ptimes = 1; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//int box_size = 0; // 探索範囲
//
//double Ddash = 250.0;
//double coef = (double)num_z_level * Ddash;
//double inv_coef = 1.0 / coef;
//
//double img_pitch = ptimes * display_pixel_pitch;
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
//void insert_pixels(int start, int end, cv::Mat img_display, unsigned char*** red, unsigned char*** green, unsigned char*** blue, bool*** alpha, int*** nx, int*** ny, int* startu, int* startv) {
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
//        for (int n = 0; n < element_image_px; n++) {
//
//            // 各要素カメラごとに
//            for (int i = 0; i < num_pinhole; i++) {
//
//                //tmp_startv = startv[i];
//                tmp_startv = static_cast<int>(round(i * intv));
//
//                for (int j = 0; j < num_pinhole; j++) {
//
//                    //tmp_startu = startv[j];
//                    tmp_startu = static_cast<int>(round(j * intv));
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
//    cout << "prop-lenna-v1" << endl;
//    cout << "pinhole array size:" << subject_position_offset << endl;
//
//
//    double start, finish;
//    clock_t cpu_time;
//
//    // 各要素画像の原点画素位置(左上)
//    int* startu = (int*)malloc(sizeof(int) * num_pinhole);
//    int* startv = (int*)malloc(sizeof(int) * num_pinhole);
//    for (int i = 0; i < num_pinhole; i++) {
//        startu[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//        startv[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//    }
//
//    // 3次元配列のインデックス
//    int*** nx = (int***)malloc(sizeof(int**) * element_image_px);
//    int*** ny = (int***)malloc(sizeof(int**) * element_image_px);
//    for (int i = 0; i < element_image_px; i++) {
//
//        nx[i] = (int**)malloc(sizeof(int*) * num_pinhole);
//        ny[i] = (int**)malloc(sizeof(int*) * num_pinhole);
//
//        for (int j = 0; j < num_pinhole; j++) {
//            nx[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//        for (int j = 0; j < num_pinhole; j++) {
//            ny[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//        }
//    }
//
//    double u, xt, yt, zt, nz;
//    for (int i = 0; i < element_image_px; i++) {
//
//        u = ((double)i - (double)(element_image_px - 1) * 0.5) * display_pixel_pitch;
//
//        for (int j = 0; j < num_pinhole; j++) {
//
//            zt = (double)(num_z_level - 1) * inv_coef;
//            xt = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * zt + u / gap;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                nx[i][j][nz] = static_cast<int>(floor((gap / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                zt -= inv_coef;
//                xt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
//            }
//
//        }
//
//        for (int j = 0; j < num_pinhole; j++) {
//
//            zt = (double)(num_z_level - 1) * inv_coef;
//            yt = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * zt + u / gap;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                ny[i][j][nz] = static_cast<int>(floor((gap / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//                zt -= inv_coef;
//                yt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
//            }
//
//        }
//    }
//
//    double** data = (double**)malloc(sizeof(double*) * rows);
//    for (int i = 0; i < rows; i++) {
//        data[i] = (double*)malloc(sizeof(double) * cols);
//    }
//
//    std::string filenamein = "./images/lenna.bmp";
//    cv::Mat image_input = cv::imread(filenamein);
//
//    if (image_input.empty())
//    {
//        std::cout << "画像を開くことができませんでした。\n";
//        return -1;
//    }
//
//    cv::Mat resized_image;
//    cv::resize(image_input, resized_image, cv::Size(554, 554), 0, 0, cv::INTER_NEAREST);
//
//    for (int i = 0; i < subject_image_resolution; i++) {
//        for (int j = 0; j < subject_image_resolution; j++) {
//            data[i * subject_image_resolution + j][0] = subject_position_offset + j * subject_pixel_pitch;
//            data[i * subject_image_resolution + j][1] = subject_position_offset + i * subject_pixel_pitch;
//            data[i * subject_image_resolution + j][2] = subject_z;
//            data[i * subject_image_resolution + j][3] = resized_image.at<Vec3b>(i, j)[0];
//            data[i * subject_image_resolution + j][4] = resized_image.at<Vec3b>(i, j)[1];
//            data[i * subject_image_resolution + j][5] = resized_image.at<Vec3b>(i, j)[2];
//        }
//    }
//
//
//    unsigned char*** red;
//    unsigned char*** green;
//    unsigned char*** blue;
//    bool*** alpha;
//    double*** val_z;
//
//    // red = new unsigned char**[num_z_level];
//    // green = new unsigned char**[num_z_level];
//    // blue = new unsigned char**[num_z_level];
//    // alpha = new bool**[num_z_level];
//    // val_z = new int**[num_z_level];
//
//    red = (unsigned char***)malloc(sizeof(unsigned char**) * num_z_level);
//    green = (unsigned char***)malloc(sizeof(unsigned char**) * num_z_level);
//    blue = (unsigned char***)malloc(sizeof(unsigned char**) * num_z_level);
//    alpha = (bool***)malloc(sizeof(bool**) * num_z_level);
//    val_z = (double***)malloc(sizeof(double**) * num_z_level);
//
//    for (int i = 0; i < num_z_level; i++) {
//
//        // red[i] = new unsigned char*[px_height_img];
//        // green[i] = new unsigned char*[px_height_img];
//        // blue[i] = new unsigned char*[px_height_img];
//        // alpha[i] = new bool*[px_height_img];
//        // val_z[i] = new int*[px_height_img];
//
//        red[i] = (unsigned char**)malloc(sizeof(unsigned char*) * px_height_img);
//        green[i] = (unsigned char**)malloc(sizeof(unsigned char*) * px_height_img);
//        blue[i] = (unsigned char**)malloc(sizeof(unsigned char*) * px_height_img);
//        alpha[i] = (bool**)malloc(sizeof(bool*) * px_height_img);
//        val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
//
//        for (int j = 0; j < px_height_img; j++) {
//
//            // red[i][j] = new unsigned char[px_width_img];
//            // green[i][j] = new unsigned char[px_width_img];
//            // blue[i][j] = new unsigned char[px_width_img];
//            // alpha[i][j] = new bool[px_width_img];
//            // val_z[i][j] = new int[px_width_img];
//
//            red[i][j] = (unsigned char*)malloc(sizeof(unsigned char) * px_width_img);
//            green[i][j] = (unsigned char*)malloc(sizeof(unsigned char) * px_width_img);
//            blue[i][j] = (unsigned char*)malloc(sizeof(unsigned char) * px_width_img);
//            alpha[i][j] = (bool*)malloc(sizeof(bool) * px_width_img);
//            val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
//
//        }
//    }
//
//    cv::Mat img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
//
//    double sum_time = 0;
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
//                    val_z[i][j][k] = 0;
//                }
//            }
//        }
//
//        for (int i = 0; i < display_px; i++) {
//            for (int j = 0; j < display_px; j++) {
//
//                img_display.at<cv::Vec3b>(i, j)[0] = 0;
//                img_display.at<cv::Vec3b>(i, j)[1] = 0;
//                img_display.at<cv::Vec3b>(i, j)[2] = 0;
//
//            }
//        }
//
//        double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double tmp_xt, tmp_yt, tmp_zt;
//        int tmp_nx, tmp_ny, tmp_nz;
//        int pcd_count = 0;
//
//        //点群を箱に格納
//        for (int k = 0; k < rows; k++) {
//
//            tmp_pcd_x = data[k][0];
//            tmp_pcd_y = data[k][1];
//            tmp_pcd_z = data[k][2];
//            tmp_pcd_b = data[k][3];
//            tmp_pcd_g = data[k][4];
//            tmp_pcd_r = data[k][5];
//
//            tmp_zt = 1.0 / tmp_pcd_z;
//            tmp_xt = tmp_pcd_x * tmp_zt;
//            tmp_yt = tmp_pcd_y * tmp_zt;
//
//            tmp_nx = static_cast<int>(floor((gap / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//            tmp_ny = static_cast<int>(floor((gap / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//            tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//            //cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;
//
//            if (0 <= tmp_nx && tmp_nx < px_width_img && 0 <= tmp_ny && tmp_ny < px_height_img && 0 <= tmp_nz && tmp_nz < num_z_level) {
//
//                if (val_z[tmp_nz][tmp_ny][tmp_nx] < tmp_zt) {
//                    blue[tmp_nz][tmp_ny][tmp_nx] = static_cast<unsigned char>(tmp_pcd_b);
//                    green[tmp_nz][tmp_ny][tmp_nx] = static_cast<unsigned char>(tmp_pcd_g);
//                    red[tmp_nz][tmp_ny][tmp_nx] = static_cast<unsigned char>(tmp_pcd_r);
//                    alpha[tmp_nz][tmp_ny][tmp_nx] = true;
//                    val_z[tmp_nz][tmp_ny][tmp_nx] = tmp_zt;
//                }
//            }
//        }
//
//        // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, sp, tp);
//
//        const int numThreads = 16;
//        vector<thread> threads;
//        int rowsPerThread = element_image_px / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
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
//        stream << "./images/lenna/prop-reconstruction/prop-lenna-v1_ImgDisplay_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << static_cast<int>(1 / ptimes) << "_subjectZ" << (int)subject_z << ".png";
//        cv::String filename = stream.str();
//        imwrite(filename, img_display);
//
//        stream.str("");
//        stream.clear(ostringstream::goodbit);
//
//        //// 各イメージプレーンの画像を保存（テスト用）
//        //// ostringstream stream;
//        //// cv::String filename;
//        //   cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//        //for (int zi = 0; zi < num_z_level; zi++) {
//
//        //    stream << "./images/lenna/image-plane/prop-lenna-v1_imagePlane_NumZLevel" << num_z_level << "_zi" << zi << "_Ddash" << Ddash << "_pitchTimes" << static_cast<int>(1 / ptimes) << ".png";
//        //    cout << "zi:" << zi << endl;
//
//        //    for (int i = 0; i < px_height_img; i++) {
//        //        for (int j = 0; j < px_width_img; j++) {
//        //            if (alpha[zi][i][j] > 0) {
//        //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
//        //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
//        //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
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
//
//    }
//
//    cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
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
//    //        cout << "Coudisplay_size not open or find the image: " << filename_original << endl;
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
//        free(data[i]);
//    }
//    free(data);
//
//    for (int i = 0; i < num_z_level; i++) {
//        for (int j = 0; j < px_height_img; j++) {
//            free(red[i][j]);
//            free(green[i][j]);
//            free(blue[i][j]);
//            free(alpha[i][j]);
//            free(val_z[i][j]);
//        }
//        free(red[i]);
//        free(green[i]);
//        free(blue[i]);
//        free(alpha[i]);
//        free(val_z[i]);
//    }
//    free(red);
//    free(green);
//    free(blue);
//    free(alpha);
//    free(val_z);
//
//    //free(sp);
//    //free(tp);
//
//    free(startu);
//    free(startv);
//
//    //for (int i = 0; i < element_image_px; i++) {
//    //    free(xt[i]);
//    //    free(yt[i]);
//    //}
//    //free(xt);
//    //free(yt);
//
//    for (int i = 0; i < element_image_px; i++) {
//        for (int j = 0; j < num_pinhole; j++) {
//            free(nx[i][j]);
//        }
//        for (int j = 0; j < num_pinhole; j++) {
//            free(ny[i][j]);
//        }
//        free(nx[i]);
//        free(ny[i]);
//    }
//
//    return EXIT_SUCCESS;
//}
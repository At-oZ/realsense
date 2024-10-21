///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* lenna画像を用いたプログラム */
///* v1-4と同じアルゴリズム */
///*
//    v0との差分:点群取得カメラの位置を観察位置またはその1/2倍した距離の位置に固定
//    →特に1/2倍した距離の位置に置いた場合、観察範囲に存在する点群は全て3次元配列内に集約できる
//*/
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
//void insert_pixels(int start, int end, int element_image_px, double display_pixel_pitch, int num_pinhole, double intv, int num_z_level, double inv_coef, double gap, double boxel_cam_px_pitch, int boxel_cam_width_px, int boxel_cam_height_px, double boxel_cam_focal_length, double pinhole_pitch, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, double* sp, double* tp) {
//
//    int nx, ny, startu, startv;
//    double u, v, x, y, z, xt, yt, zt, tmp_sp, tmp_tp;
//
//    // 各要素画像の各画素ごとに
//    for (int m = start; m < end; m++) {
//
//        v = ((double)m - (double)(element_image_px - 1) * 0.5) * display_pixel_pitch;
//
//        for (int n = 0; n < element_image_px; n++) {
//
//            u = ((double)n - (double)(element_image_px - 1) * 0.5) * display_pixel_pitch;
//
//            // 各要素カメラごとに
//            for (int i = 0; i < num_pinhole; i++) {
//
//                //tmp_startv = startv[i];
//                startv = static_cast<int>(round(i * intv));
//
//                for (int j = 0; j < num_pinhole; j++) {
//
//                    //tmp_startu = startv[j];
//                    startu = static_cast<int>(round(j * intv));
//
//                    tmp_sp = sp[i * num_pinhole + j];
//                    tmp_tp = tp[i * num_pinhole + j];
//
//                    zt = (double)(num_z_level - 1) * inv_coef;
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int nz = num_z_level - 1; nz >= 0; nz--) {
//
//                        z = 1.0 / zt + boxel_cam_focal_length;
//                        x = tmp_sp + u / gap * z;
//                        y = tmp_tp + v / gap * z;
//
//                        xt = x * zt;
//                        yt = y * zt;
//
//                        nx = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * xt + 0.5) + boxel_cam_width_px * 0.5);
//                        ny = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * yt + 0.5) + boxel_cam_height_px * 0.5);
//
//                        //cout << "nx:" << nx << ", ny:" << ny << endl;
//                        if (0 <= nx && nx < boxel_cam_width_px && 0 <= ny && ny < boxel_cam_height_px) {
//
//                            if (alpha[nz][ny][nx]) {
//                                //img_display[startv + m][startu + n] = tmp_pcd_color - alpha;
//                                img_display.at<cv::Vec3b>(startv + m, startu + n)[0] = blue[nz][ny][nx];
//                                img_display.at<cv::Vec3b>(startv + m, startu + n)[1] = green[nz][ny][nx];
//                                img_display.at<cv::Vec3b>(startv + m, startu + n)[2] = red[nz][ny][nx];
//
//                                // cout << "v, u:" << startv + m << ", " << startu + n << endl;
//                                break;
//                            }
//                        }
//
//                        zt -= inv_coef;
//                        xt -= tmp_sp * inv_coef;
//                        yt -= tmp_tp * inv_coef;
//
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
//int writeCSV1(const std::vector<double> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./images/lenna/prop-reconstruction/average-time-v0-2-home.csv");
//
//    // ファイルが正しく開けたか確認
//    if (!file.is_open()) {
//        std::cerr << "ファイルが開けませんでした" << std::endl;
//        return 1;
//    }
//
//    // 1次元配列の内容をCSV形式でファイルに書き込む
//    for (const auto& row : array) {
//
//        file << row;
//        file << "\n"; // 行の終わりに改行を挿入
//    }
//
//    // ファイルを閉じる
//    file.close();
//
//    std::cout << "書き込みが完了しました。" << std::endl;
//
//    return 0;
//}
//
//int main(int argc, char* argv[])
//{
//
//    cout << "prop-lenna-v0-2" << endl;
//
//    std::vector<double> result(18);
//
//    int index = 0;
//    for (int nph = 160; nph >= 40; nph /= 2) {
//
//
//        for (int nzl = 40; nzl >= 40; nzl /= 2) {
//
//
//            for (double subz = 1024.0; subz >= 256.0; subz /= 2) {
//
//
//                // ディスプレイのパラメータ(mm)
//                double display_size = 13.4 * 25.4; // ディスプレイサイズ
//                int display_width_px = 3840, display_height_px = 2400; // ディスプレイの縦横の解像度
//                double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ
//
//                // ピンホールアレイのパラメータ(mm)
//                double pinhole_array_size = display_pixel_pitch * display_height_px;   // 各軸方向のピンホールアレイのサイズ
//                int num_pinhole = nph;  // 各軸方向のピンホール数
//                double pinhole_pitch = pinhole_array_size / num_pinhole;    // ピンホールピッチ
//
//                // 表示系のパラメータ(mm)
//                double gap = 4.0 * 160.0 / (double)nph; // ギャップ
//                int element_image_px = static_cast<int>(floor(pinhole_pitch / display_pixel_pitch)); // 要素画像の解像度
//                int display_px = 2400; // 各軸方向の表示画像の解像度
//                double intv = pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//
//                // 被写体のパラメータ(mm)
//                int subject_image_resolution = 554; // 被写体の解像度
//                double subject_size = pinhole_array_size; // 被写体のサイズ
//                double subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
//                double subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
//                double subject_z = subz; // 被写体の奥行き方向の位置
//
//                // 点群取得カメラのパラメータ(mm)
//                double boxel_cam_focal_length = -1500;
//                double boxel_cam_sensor_size = pinhole_array_size;
//                int boxel_cam_height_px = 50;
//                int boxel_cam_width_px = 50;
//                double boxel_cam_px_pitch = boxel_cam_sensor_size / (double)boxel_cam_width_px;
//
//                // 点群データ配列の行数と列数
//                int rows = subject_image_resolution * subject_image_resolution;
//                int cols = 6;
//
//                // パラメータ
//                int num_z_level = nzl; // イメージプレーンの層数
//
//                double Ddash = 200.0 - boxel_cam_focal_length;
//                double coef = (double)num_z_level * Ddash;
//                double inv_coef = 1.0 / coef;
//
//                int TIMES = 100;
//
//                cout << "NumPinhole:" << num_pinhole  << ", NumZLevel:" << num_z_level << ", subjectZ:" << subject_z << ", gap:" << gap << endl;
//
//                // 各要素画像の原点画素位置(左上)
//                int* startu = (int*)malloc(sizeof(int) * num_pinhole);
//                int* startv = (int*)malloc(sizeof(int) * num_pinhole);
//                for (int i = 0; i < num_pinhole; i++) {
//                    startu[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//                    startv[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//                }
//
//                // 視点パラメータ(mm)
//                double* sp;
//                double* tp;
//                sp = (double*)malloc(sizeof(double) * num_pinhole * num_pinhole);
//                tp = (double*)malloc(sizeof(double) * num_pinhole * num_pinhole);
//                for (int i = 0; i < num_pinhole; i++) {
//                    for (int j = 0; j < num_pinhole; j++) {
//
//                        sp[i * num_pinhole + j] = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch;
//                        tp[i * num_pinhole + j] = (i - (num_pinhole - 1) * 0.5) * pinhole_pitch;
//
//                        //cout << "uo:" << uo[i][j] << ", vo:" << vo[i][j] << endl;
//
//                    }
//                }
//
//                double** data = (double**)malloc(sizeof(double*) * rows);
//                for (int i = 0; i < rows; i++) {
//                    data[i] = (double*)malloc(sizeof(double) * cols);
//                }
//
//                std::string filenamein = "./images/lenna.bmp";
//                cv::Mat image_input = cv::imread(filenamein);
//
//                if (image_input.empty())
//                {
//                    std::cout << "画像を開くことができませんでした。\n";
//                    return -1;
//                }
//
//                cv::Mat resized_image;
//                cv::resize(image_input, resized_image, cv::Size(554, 554), 0, 0, cv::INTER_NEAREST);
//
//                for (int i = 0; i < subject_image_resolution; i++) {
//                    for (int j = 0; j < subject_image_resolution; j++) {
//                        data[i * subject_image_resolution + j][0] = subject_position_offset + j * subject_pixel_pitch;
//                        data[i * subject_image_resolution + j][1] = subject_position_offset + i * subject_pixel_pitch;
//                        data[i * subject_image_resolution + j][2] = subject_z;
//                        data[i * subject_image_resolution + j][3] = resized_image.at<Vec3b>(i, j)[0];
//                        data[i * subject_image_resolution + j][4] = resized_image.at<Vec3b>(i, j)[1];
//                        data[i * subject_image_resolution + j][5] = resized_image.at<Vec3b>(i, j)[2];
//                    }
//                }
//
//
//                int*** red;
//                int*** green;
//                int*** blue;
//                bool*** alpha;
//                //int*** num_pcd;
//                double*** val_z;
//
//                // red = new int**[num_z_level];
//                // green = new int**[num_z_level];
//                // blue = new int**[num_z_level];
//                // alpha = new bool**[num_z_level];
//                // val_z = new int**[num_z_level];
//
//                red = (int***)malloc(sizeof(int**) * num_z_level);
//                green = (int***)malloc(sizeof(int**) * num_z_level);
//                blue = (int***)malloc(sizeof(int**) * num_z_level);
//                alpha = (bool***)malloc(sizeof(bool**) * num_z_level);
//                //num_pcd = (int***)malloc(sizeof(int**) * num_z_level);
//                val_z = (double***)malloc(sizeof(double**) * num_z_level);
//
//                for (int i = 0; i < num_z_level; i++) {
//
//                    // red[i] = new int*[boxel_cam_height_px];
//                    // green[i] = new int*[boxel_cam_height_px];
//                    // blue[i] = new int*[boxel_cam_height_px];
//                    // alpha[i] = new bool*[boxel_cam_height_px];
//                    // val_z[i] = new int*[boxel_cam_height_px];
//
//                    red[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
//                    green[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
//                    blue[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
//                    alpha[i] = (bool**)malloc(sizeof(bool*) * boxel_cam_height_px);
//                    //num_pcd[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
//                    val_z[i] = (double**)malloc(sizeof(double*) * boxel_cam_height_px);
//
//                    for (int j = 0; j < boxel_cam_height_px; j++) {
//
//                        // red[i][j] = new int[boxel_cam_width_px];
//                        // green[i][j] = new int[boxel_cam_width_px];
//                        // blue[i][j] = new int[boxel_cam_width_px];
//                        // alpha[i][j] = new bool[boxel_cam_width_px];
//                        // val_z[i][j] = new int[boxel_cam_width_px];
//
//                        red[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
//                        green[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
//                        blue[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
//                        alpha[i][j] = (bool*)malloc(sizeof(bool) * boxel_cam_width_px);
//                        //num_pcd[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
//                        val_z[i][j] = (double*)malloc(sizeof(double) * boxel_cam_width_px);
//
//                    }
//                }
//
//                cv::Mat img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
//
//                double sum_time = 0;
//                // フレーム処理
//                for (int tt = 0; tt < TIMES; tt++) {
//
//                    // 測定開始時刻を記録
//                    auto start = std::chrono::high_resolution_clock::now();
//
//                    // 箱とzバッファをオフセット
//                    for (int i = 0; i < num_z_level; i++) {
//                        for (int j = 0; j < boxel_cam_height_px; j++) {
//                            for (int k = 0; k < boxel_cam_width_px; k++) {
//                                //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
//                                red[i][j][k] = 0;
//                                green[i][j][k] = 0;
//                                blue[i][j][k] = 0;
//                                alpha[i][j][k] = false;
//                                //num_pcd[i][j][k] = 0;
//                                val_z[i][j][k] = 0;
//                            }
//                        }
//                    }
//
//                    for (int i = 0; i < display_px; i++) {
//                        for (int j = 0; j < display_px; j++) {
//
//                            img_display.at<cv::Vec3b>(i, j)[0] = 0;
//                            img_display.at<cv::Vec3b>(i, j)[1] = 0;
//                            img_display.at<cv::Vec3b>(i, j)[2] = 0;
//
//                        }
//                    }
//
//                    double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//                    double xt, yt, zt;
//                    int nx, ny, nz;
//                    int pcd_count = 0;
//
//                    //点群を箱に格納
//                    for (int k = 0; k < rows; k++) {
//
//                        tmp_pcd_x = data[k][0];
//                        tmp_pcd_y = data[k][1];
//                        tmp_pcd_z = data[k][2] - boxel_cam_focal_length;
//                        tmp_pcd_b = data[k][3];
//                        tmp_pcd_g = data[k][4];
//                        tmp_pcd_r = data[k][5];
//
//                        zt = 1.0 / tmp_pcd_z;
//                        xt = tmp_pcd_x * zt;
//                        yt = tmp_pcd_y * zt;
//
//                        nx = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * xt + 0.5) + boxel_cam_width_px * 0.5);
//                        ny = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * yt + 0.5) + boxel_cam_height_px * 0.5);
//                        nz = static_cast<int>(floor(coef * zt + 0.5));
//
//                        //cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;
//
//                        if (0 <= nx && nx < boxel_cam_width_px && 0 <= ny && ny < boxel_cam_height_px && 0 <= nz && nz < num_z_level) {
//                            //if (pcd_box[nz][ny][nx] > 0) {
//                            //    pcd_count++;
//                            //}
//
//                            if (val_z[nz][ny][nx] < zt) {
//                                blue[nz][ny][nx] = tmp_pcd_b;
//                                green[nz][ny][nx] = tmp_pcd_g;
//                                red[nz][ny][nx] = tmp_pcd_r;
//                                val_z[nz][ny][nx] = zt;
//                                alpha[nz][ny][nx] = true;
//                            }
//                            // cout << "pcd_box:" << pcd_box[nz][ny][nx] << endl;
//                        }
//                    }
//
//                    // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, nx, ny, startu, startv);
//
//                    const int numThreads = 15;
//                    vector<thread> threads;
//                    int rowsPerThread = element_image_px / numThreads;
//
//                    int startRow, endRow;
//                    for (int i = 0; i < numThreads; i++) {
//                        startRow = i * rowsPerThread;
//                        endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
//                        threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, display_pixel_pitch, num_pinhole, intv, num_z_level, inv_coef, gap, boxel_cam_px_pitch, boxel_cam_width_px, boxel_cam_height_px, boxel_cam_focal_length, pinhole_pitch, img_display, red, green, blue, alpha, sp, tp);
//                    }
//
//                    for (auto& t : threads) {
//                        if (t.joinable()) { t.join(); }
//                    }
//
//                    // 全てのスレッドが終了するのを待つ
//                    {
//                        std::unique_lock<std::mutex> lock(mtx);
//                        conv.wait(lock, [] {return finished_threads == numThreads; });
//                    }
//
//                    //cout << "calc finished" << endl;
//                    finished_threads = 0;
//
//                    // 測定終了時刻を記録
//                    auto end = std::chrono::high_resolution_clock::now();
//
//                    // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                    //cout << "実行時間: " << duration.count() << " ms" << std::endl;
//                    //cout << "重なり回数:" << pcd_count << endl;
//                    sum_time += duration.count();
//
//                    //// 表示画像の保存
//                    //ostringstream stream;
//                    //stream << "./images/lenna/prop-reconstruction/v2-2/comparison/prop-lenna-v2-2_ImgDisplay_NumPinhole" << num_pinhole << "NumZLevel" << num_z_level << "_pitchTimes" << static_cast<int>(ptimes) << "_subjectZ" << (int)subject_z << ".png";
//                    //cv::String filename = stream.str();
//                    //imwrite(filename, img_display);
//
//                }
//
//                // 表示画像の保存
//                ostringstream stream;
//                stream << "./images/lenna/prop-reconstruction/v0-2/prop-lenna-v0-2_ImgDisplay_NumPinhole" << num_pinhole << "_NumZLevel" << num_z_level << "_subjectZ" << (int)subject_z << ".png";
//                cv::String filename = stream.str();
//                imwrite(filename, img_display);
//
//                stream.str("");
//                stream.clear(ostringstream::goodbit);
//
//                //// 各イメージプレーンの画像を保存（テスト用）
//                //// ostringstream stream;
//                //// cv::String filename;
//                //cv::Mat img = cv::Mat::zeros(cv::Size(boxel_cam_width_px, boxel_cam_height_px), CV_8UC3);
//                //for (int zi = 0; zi < num_z_level; zi++) {
//
//                //    stream << "./images/lenna/image-plane/v0/prop-lenna-v0_imagePlane_NumPinhole" << num_pinhole << "_NumZLevel" << num_z_level << "_subjectZ" << (int)subject_z << "_zi" << zi << ".png";
//                //    cout << "zi:" << zi << endl;
//
//                //    for (int i = 0; i < boxel_cam_height_px; i++) {
//                //        for (int j = 0; j < boxel_cam_width_px; j++) {
//                //            if (alpha[zi][i][j] > 0) {
//                //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
//                //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
//                //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
//                //            }
//                //        }
//                //    }
//                //    filename = stream.str();
//                //    imwrite(filename, img);
//                //    stream.str("");
//                //    stream.clear(ostringstream::goodbit);
//                //    img = cv::Mat::zeros(cv::Size(boxel_cam_width_px, boxel_cam_height_px), CV_8UC3);
//
//                //}
//
//                cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//                result[index++] = sum_time / TIMES;
//
//                // 使用したメモリを解放
//                for (int i = 0; i < rows; ++i) {
//                    free(data[i]);
//                }
//                free(data);
//
//                for (int i = 0; i < num_z_level; i++) {
//                    for (int j = 0; j < boxel_cam_height_px; j++) {
//                        free(red[i][j]);
//                        free(green[i][j]);
//                        free(blue[i][j]);
//                        free(alpha[i][j]);
//                        //free(num_pcd[i][j]);
//                        free(val_z[i][j]);
//                    }
//                    free(red[i]);
//                    free(green[i]);
//                    free(blue[i]);
//                    free(alpha[i]);
//                    //free(num_pcd[i]);
//                    free(val_z[i]);
//                }
//                free(red);
//                free(green);
//                free(blue);
//                free(alpha);
//                //free(num_pcd);
//                free(val_z);
//
//                free(sp);
//                free(tp);
//
//                free(startu);
//                free(startv);
//
//                //for (int i = 0; i < element_image_px; i++) {
//                //    free(xt[i]);
//                //    free(yt[i]);
//                //}
//                //free(xt);
//                //free(yt);
//
//
//
//            }
//        }
//    }
//
//    writeCSV1(result);
//
//    return EXIT_SUCCESS;
//}
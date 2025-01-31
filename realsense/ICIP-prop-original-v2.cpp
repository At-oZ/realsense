///* 点群データから4次元光線情報の再構成画像を作成するプログラム */
///* ICIP-prop-original-v1からの派生 */
///* オリジナル版（テーブル化、詳細化と平均化なし） */
///* 変更点：タイルを縦に三分割し、それぞれの奥行を変える */
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
//
//using namespace std;
//using namespace cv;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//void insert_pixels(int start, int end, int element_image_px, double display_pixel_pitch, int nph, double intv, int nzl, double inv_coef, double focal_length, double img_pitch, int px_width_img, int px_height_img, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, double* sp, double* tp);
//int writeCSV2(const std::vector<std::vector<double>> array);
//
//int main(int argc, char* argv[])
//{
//
//    cout << "ICIP-prop-original-v2" << endl;
//
//    std::vector<std::vector<double>> array(10, std::vector<double>(6)); // 横：subz, 縦：nzl
//
//    // 被写体の画像
//    std::string filenamein = "./images/standard/milkdrop.bmp";
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
//
//    // 観察者のパラメータ(mm)
//    double zo_min = 1000.0; // 最短視認距離
//
//    // ディスプレイのパラメータ(mm)
//    double display_size = 13.4 * 25.4; // ディスプレイサイズ
//    int display_width_px = 3840, display_height_px = 2400; // ディスプレイの縦横の解像度
//    double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ
//
//    // ピンホールアレイのパラメータ(mm)
//    double pinhole_array_size = display_pixel_pitch * display_height_px;   // 各軸方向のピンホールアレイのサイズ
//
//    // 表示系のパラメータ(mm)
//    int display_px = 2400; // 各軸方向の表示画像の解像度
//
//    // 被写体のパラメータ(mm)
//    int subject_image_resolution = 554; // 被写体の解像度
//    double subject_size = pinhole_array_size; // 被写体のサイズ
//    double subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
//    double subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
//
//    // 点群データ配列の行数と列数
//    int rows = subject_image_resolution * subject_image_resolution;
//    int cols = 6;
//
//    double Ddash = 100.0;
//
//    double img_pitch = display_pixel_pitch;
//
//    int TIMES = 1;
//
//    int index = 0;
//    for (int nph = 40; nph <= 40; nph *= 2) {
//
//        int idx_nzl = 0;
//        double pinhole_pitch = pinhole_array_size / nph;    // ピンホールピッチ
//        double focal_length = zo_min / (3 * nph - 1); // ギャップ
//        int element_image_px = static_cast<int>(floor(pinhole_pitch / display_pixel_pitch)); // 要素画像の解像度
//        double intv = pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//        int px_height_img = static_cast<int>(floor(round(100 * ptimes));
//        int px_width_img = static_cast<int>(floor(round(100 * ptimes));
//
//        for (int nzl = 10; nzl <= 100; nzl += 10) {
//
//            int idx_subz = 0;
//            double coef = (double)nzl * Ddash;
//            double inv_coef = 1.0 / coef;
//
//            for (double subz = 256.0; subz <= 8192.0; subz *= 2) {
//
//                cout << "NumPinhole:" << nph << ", NumZLevel:" << nzl << ", subjectZ:" << subz << ", focal_length:" << focal_length << endl;
//
//                // 各要素画像の原点画素位置(左上)
//                int* startu = (int*)malloc(sizeof(int) * nph);
//                int* startv = (int*)malloc(sizeof(int) * nph);
//                for (int i = 0; i < nph; i++) {
//                    startu[i] = static_cast<int>(std::round(((i - (nph - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//                    startv[i] = static_cast<int>(std::round(((i - (nph - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//                }
//
//                // 視点パラメータ(mm)
//                double* sp;
//                double* tp;
//                sp = (double*)malloc(sizeof(double) * nph * nph);
//                tp = (double*)malloc(sizeof(double) * nph * nph);
//                for (int i = 0; i < nph; i++) {
//                    for (int j = 0; j < nph; j++) {
//
//                        sp[i * nph + j] = (j - (nph - 1) * 0.5) * pinhole_pitch;
//                        tp[i * nph + j] = (i - (nph - 1) * 0.5) * pinhole_pitch;
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
//                for (int i = 0; i < subject_image_resolution; i++) {
//                    for (int j = 0; j < subject_image_resolution; j++) {
//
//                        data[i * subject_image_resolution + j][0] = subject_position_offset + j * subject_pixel_pitch;
//                        data[i * subject_image_resolution + j][1] = subject_position_offset + i * subject_pixel_pitch;
//                        data[i * subject_image_resolution + j][3] = resized_image.at<Vec3b>(i, j)[0];
//                        data[i * subject_image_resolution + j][4] = resized_image.at<Vec3b>(i, j)[1];
//                        data[i * subject_image_resolution + j][5] = resized_image.at<Vec3b>(i, j)[2];
//
//                        if (j >= 0 && j < (int)(subject_image_resolution / 3)) {
//                            data[i * subject_image_resolution + j][2] = subz / 2.0;
//                        }
//                        else if (j >= (int)(subject_image_resolution / 3) && j < (int)(subject_image_resolution * 2 / 3)) {
//                            data[i * subject_image_resolution + j][2] = subz;
//                        }
//                        else {
//                            data[i * subject_image_resolution + j][2] = subz * 2.0;
//                        }
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
//                // red = new int**[nzl];
//                // green = new int**[nzl];
//                // blue = new int**[nzl];
//                // alpha = new bool**[nzl];
//                // val_z = new int**[nzl];
//
//                red = (int***)malloc(sizeof(int**) * nzl);
//                green = (int***)malloc(sizeof(int**) * nzl);
//                blue = (int***)malloc(sizeof(int**) * nzl);
//                alpha = (bool***)malloc(sizeof(bool**) * nzl);
//                //num_pcd = (int***)malloc(sizeof(int**) * nzl);
//                val_z = (double***)malloc(sizeof(double**) * nzl);
//
//                for (int i = 0; i < nzl; i++) {
//
//                    // red[i] = new int*[px_height_img];
//                    // green[i] = new int*[px_height_img];
//                    // blue[i] = new int*[px_height_img];
//                    // alpha[i] = new bool*[px_height_img];
//                    // val_z[i] = new int*[px_height_img];
//
//                    red[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                    green[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                    blue[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                    alpha[i] = (bool**)malloc(sizeof(bool*) * px_height_img);
//                    //num_pcd[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                    val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
//
//                    for (int j = 0; j < px_height_img; j++) {
//
//                        // red[i][j] = new int[px_width_img];
//                        // green[i][j] = new int[px_width_img];
//                        // blue[i][j] = new int[px_width_img];
//                        // alpha[i][j] = new bool[px_width_img];
//                        // val_z[i][j] = new int[px_width_img];
//
//                        red[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                        green[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                        blue[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                        alpha[i][j] = (bool*)malloc(sizeof(bool) * px_width_img);
//                        //num_pcd[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                        val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
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
//                    for (int i = 0; i < nzl; i++) {
//                        for (int j = 0; j < px_height_img; j++) {
//                            for (int k = 0; k < px_width_img; k++) {
//                                //pcd_box[i][j][k] = static_cast<unsigned char>(((nzl - 1) - i) / (nzl - 1)) + static_cast<unsigned char>(((nzl - 1) - i) / (nzl - 1) * 255) + static_cast<unsigned char>(((nzl - 1) - i) / (nzl - 1) * 255 * 255);
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
//                        tmp_pcd_z = data[k][2];
//                        tmp_pcd_b = data[k][3];
//                        tmp_pcd_g = data[k][4];
//                        tmp_pcd_r = data[k][5];
//
//                        zt = 1.0 / tmp_pcd_z;
//                        xt = tmp_pcd_x * zt;
//                        yt = tmp_pcd_y * zt;
//
//                        nx = static_cast<int>(floor((focal_length / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                        ny = static_cast<int>(floor((focal_length / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//                        nz = static_cast<int>(floor(coef * zt + 0.5));
//
//                        //cout << "nx:" << nx << ", ny:" << ny << ", nz:" << nz << endl;
//
//                        if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img && 0 <= nz && nz < nzl) {
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
//                        threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, display_pixel_pitch, nph, intv, nzl, inv_coef, focal_length, img_pitch, px_width_img, px_height_img, img_display, red, green, blue, alpha, sp, tp);
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
//                    sum_time += duration.count();
//
//                }
//
//                // 表示画像の保存
//                ostringstream stream;
//                stream << "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ICIP-prop-original-v2/prop-v2-milkdrop_Nz" << nzl << "_subjectZ" << (int)subz << ".png";
//                cv::String filename = stream.str();
//                imwrite(filename, img_display);
//
//                stream.str("");
//                stream.clear(ostringstream::goodbit);
//
//                cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//                array[idx_nzl][idx_subz++] = sum_time / TIMES;
//
//                // 使用したメモリを解放
//                for (int i = 0; i < rows; ++i) {
//                    free(data[i]);
//                }
//                free(data);
//
//                for (int i = 0; i < nzl; i++) {
//                    for (int j = 0; j < px_height_img; j++) {
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
//            }
//            idx_nzl++;
//        }
//    }
//
//    writeCSV2(array);
//
//    return EXIT_SUCCESS;
//}
//
//void insert_pixels(int start, int end, int element_image_px, double display_pixel_pitch, int nph, double intv, int nzl, double inv_coef, double focal_length, double img_pitch, int px_width_img, int px_height_img, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, double* sp, double* tp) {
//
//    int nx, ny, startu, startv;
//    double u, v, xt, yt, zt, tmp_sp, tmp_tp;
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
//            for (int i = 0; i < nph; i++) {
//
//                //tmp_startv = startv[i];
//                startv = static_cast<int>(round(i * intv));
//
//                for (int j = 0; j < nph; j++) {
//
//                    //tmp_startu = startv[j];
//                    startu = static_cast<int>(round(j * intv));
//
//                    tmp_sp = sp[i * nph + j];
//                    tmp_tp = tp[i * nph + j];
//
//                    zt = (double)(nzl - 1) * inv_coef;
//                    xt = tmp_sp * zt + u / focal_length;
//                    yt = tmp_tp * zt + v / focal_length;
//
//                    // 各奥行きレベルごとに(手前から)
//                    for (int nz = nzl - 1; nz >= 0; nz--) {
//
//                        nx = static_cast<int>(floor((focal_length / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                        ny = static_cast<int>(floor((focal_length / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//
//                        //cout << "nx:" << nx << ", ny:" << ny << endl;
//                        if (0 <= nx && nx < px_width_img && 0 <= ny && ny < px_height_img) {
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
//int writeCSV2(const std::vector<std::vector<double>> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("C:/Users/taw11/EvacuatedStorage/prop-reconstruction/speed-prop-v1-milkdrop.csv");
//
//    // ファイルが正しく開けたか確認
//    if (!file.is_open()) {
//        std::cerr << "ファイルが開けませんでした" << std::endl;
//        return 1;
//    }
//
//    // 2次元配列の内容をCSV形式でファイルに書き込む
//    for (const auto& row : array) {
//        for (size_t i = 0; i < row.size(); ++i) {
//            file << row[i];
//            if (i < row.size() - 1) {
//                file << ","; // 各要素の間にカンマを挿入
//            }
//        }
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

///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* lenna画像を用いたプログラム */
///* DirectProjectionと同じアルゴリズム */
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
//void insert_pixels(int start, int end, int rows, int num_pinhole, double gap, double display_pixel_pitch, int element_image_px, double intv, cv::Mat img_display, double* sp, double* tp, double** data, double** val_z) {
//
//    double inv_d, tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//    double newx, newy;
//    int nx, ny;
//    int tmp_startv, tmp_startu;
//    for (int k = 0; k < rows; k++) {
//
//        tmp_pcd_x = data[k][0];
//        tmp_pcd_y = data[k][1];
//        tmp_pcd_z = data[k][2];
//        tmp_pcd_b = data[k][3];
//        tmp_pcd_g = data[k][4];
//        tmp_pcd_r = data[k][5];
//
//        for (int i = start; i < end; i++) {
//            for (int j = 0; j < num_pinhole; j++) {
//
//                //cout << "i:" << i << ", j:" << j << endl;
//
//                newx = (gap / tmp_pcd_z) * (tmp_pcd_x - sp[i * num_pinhole + j]);
//                newy = (gap / tmp_pcd_z) * (tmp_pcd_y - tp[i * num_pinhole + j]);
//
//                nx = static_cast<int>(floor(newx / display_pixel_pitch) + element_image_px * 0.5);
//                ny = static_cast<int>(floor(newy / display_pixel_pitch) + element_image_px * 0.5);
//
//                tmp_startv = static_cast<int>(round(i * intv));
//                tmp_startu = static_cast<int>(round(j * intv));
//
//                //cout << "newx:" << newx[i][j] << ", newy:" << newy[i][j] << endl;
//                //cout << "nx:" << nx << ", ny:" << ny << endl;
//
//                if (0 <= nx && nx < element_image_px && 0 <= ny && ny < element_image_px) {
//                    if (val_z[tmp_startv + ny][tmp_startu + nx] > tmp_pcd_z) {
//
//                        img_display.at<cv::Vec3b>(tmp_startv + ny, tmp_startu + nx)[2] = static_cast<int>(tmp_pcd_r);
//                        img_display.at<cv::Vec3b>(tmp_startv + ny, tmp_startu + nx)[1] = static_cast<int>(tmp_pcd_g);
//                        img_display.at<cv::Vec3b>(tmp_startv + ny, tmp_startu + nx)[0] = static_cast<int>(tmp_pcd_b);
//                        val_z[tmp_startv + ny][tmp_startu + nx] = tmp_pcd_z;
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
//int writeCSV1(const std::vector<double> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./images/lenna/prop-reconstruction/average-time-DP-2.csv");
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
//    cout << "DP-lenna-v1" << endl;
//
//    std::vector<double> result(9);
//
//    int index = 0;
//    for (int nph = 160; nph >= 40; nph /= 2) {
//
//        for (double subz = 1024.0; subz >= 256.0; subz /= 2) {
//
//            cout << "num pinhole:" << nph << ", subject z:" << subz << endl;
//
//            // ディスプレイのパラメータ(mm)
//            double display_size = 13.4 * 25.4;  // ディスプレイサイズ
//            int display_width_px = 3840, display_height_px = 2400;                   // ディスプレイの縦横の解像度
//            double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ
//
//            // ピンホールアレイのパラメータ(mm)
//            double pinhole_array_size = display_pixel_pitch * display_height_px;   // 各軸方向のピンホールアレイのサイズ
//            int num_pinhole = nph;  // 各軸方向のピンホール数
//            double pinhole_pitch = pinhole_array_size / num_pinhole;    // ピンホールピッチ
//
//            // 表示系のパラメータ(mm)
//            double gap = 4.0 * 160.0 / (double)nph; // ギャップ
//            int element_image_px = static_cast<int>(floor(pinhole_pitch / display_pixel_pitch)); // 要素画像の解像度
//            int display_image_px = 2400; // 各軸方向の表示画像の解像度
//            double intv = pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//
//            // 被写体のパラメータ(mm)
//            int subject_image_resolution = 554; // 被写体の解像度
//            double subject_size = pinhole_array_size; // 被写体のサイズ
//            double subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
//            double subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
//            double subject_z = subz; // 被写体の奥行き方向の位置
//
//            // 点群データ配列の行数と列数
//            int rows = subject_image_resolution * subject_image_resolution;
//            int cols = 6;
//
//            int TIMES = 1;
//
//            // 視点パラメータ(mm)
//            double* sp;
//            double* tp;
//            sp = (double*)malloc(sizeof(double) * num_pinhole * num_pinhole);
//            tp = (double*)malloc(sizeof(double) * num_pinhole * num_pinhole);
//            for (int i = 0; i < num_pinhole; i++) {
//                for (int j = 0; j < num_pinhole; j++) {
//
//                    sp[i * num_pinhole + j] = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch;
//                    tp[i * num_pinhole + j] = (i - (num_pinhole - 1) * 0.5) * pinhole_pitch;
//
//                    //cout << "sp:" << sp[i][j] << ", tp:" << tp[i][j] << endl;
//
//                }
//            }
//
//            double** data = (double**)malloc(sizeof(double*) * rows);
//            for (int i = 0; i < rows; i++) {
//                data[i] = (double*)malloc(sizeof(double) * cols);
//            }
//
//            std::string filenamein = "./images/lenna.bmp";
//            cv::Mat image_input = cv::imread(filenamein);
//
//            if (image_input.empty())
//            {
//                std::cout << "画像を開くことができませんでした。\n";
//                return -1;
//            }
//
//            cv::Mat resized_image;
//            cv::resize(image_input, resized_image, cv::Size(554, 554), 0, 0, cv::INTER_NEAREST);
//
//            for (int i = 0; i < subject_image_resolution; i++) {
//                for (int j = 0; j < subject_image_resolution; j++) {
//                    data[i * subject_image_resolution + j][0] = subject_position_offset + j * subject_pixel_pitch;
//                    data[i * subject_image_resolution + j][1] = subject_position_offset + i * subject_pixel_pitch;
//                    data[i * subject_image_resolution + j][2] = subject_z;
//                    data[i * subject_image_resolution + j][3] = resized_image.at<Vec3b>(i, j)[0];
//                    data[i * subject_image_resolution + j][4] = resized_image.at<Vec3b>(i, j)[1];
//                    data[i * subject_image_resolution + j][5] = resized_image.at<Vec3b>(i, j)[2];
//                }
//            }
//
//            double** val_z;
//            val_z = (double**)malloc(sizeof(double*) * display_image_px);
//            for (int i = 0; i < display_image_px; i++) {
//                val_z[i] = (double*)malloc(sizeof(double) * display_image_px);
//            }
//
//            cv::Mat img_display = cv::Mat::zeros(cv::Size(display_image_px, display_image_px), CV_8UC3);
//
//            double sum_time = 0;
//            // フレーム処理
//            for (int tt = 0; tt < TIMES; tt++) {
//
//                // 測定開始時刻を記録
//                auto start = std::chrono::high_resolution_clock::now();
//
//                // 表示画像とzバッファをオフセット
//                for (int i = 0; i < display_image_px; i++) {
//                    for (int j = 0; j < display_image_px; j++) {
//
//                        img_display.at<cv::Vec3b>(i, j)[0] = 0;
//                        img_display.at<cv::Vec3b>(i, j)[1] = 0;
//                        img_display.at<cv::Vec3b>(i, j)[2] = 0;
//                        val_z[i][j] = 100000.0;
//
//                    }
//                }
//
//                // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, nx, ny, startu, startv);
//
//                const int numThreads = 16;
//                vector<thread> threads;
//                int rowsPerThread = num_pinhole / numThreads;
//
//                int startRow, endRow;
//                for (int i = 0; i < numThreads; i++) {
//                    startRow = i * rowsPerThread;
//                    endRow = (i == numThreads - 1) ? num_pinhole : (i + 1) * rowsPerThread;
//                    threads.emplace_back(insert_pixels, startRow, endRow, rows, num_pinhole, gap, display_pixel_pitch, element_image_px, intv, std::ref(img_display), std::ref(sp), std::ref(tp), std::ref(data), std::ref(val_z));
//                }
//
//                for (auto& t : threads) {
//                    if (t.joinable()) { t.join(); }
//                }
//
//                // 全てのスレッドが終了するのを待つ
//                {
//                    std::unique_lock<std::mutex> lock(mtx);
//                    conv.wait(lock, [] {return finished_threads == numThreads; });
//                }
//
//                cout << "calc finished" << endl;
//                finished_threads = 0;
//
//                // 測定終了時刻を記録
//                auto end = std::chrono::high_resolution_clock::now();
//
//                // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                cout << "実行時間: " << duration.count() << " ms" << std::endl;
//                //cout << "重なり回数:" << pcd_count << endl;
//                sum_time += duration.count();
//
//                // 表示画像の保存
//                ostringstream stream;
//                stream << "./images/lenna/prop-reconstruction/DirectProjection/DP-lenna_ImgDisplay_gridSize" << num_pinhole << "_subjectZ" << (int)subject_z << ".png";
//                cv::String filename = stream.str();
//                imwrite(filename, img_display);
//
//                stream.str("");
//                stream.clear(ostringstream::goodbit);
//
//                //// 各イメージプレーンの画像を保存（テスト用）
//                //// ostringstream stream;
//                //// cv::String filename;
//                //   cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//                //for (int zi = 0; zi < num_z_level; zi++) {
//
//                //    stream << "./images/lenna/image-plane/prop-lenna-v2_imagePlane_NumZLevel" << num_z_level << "_zi" << zi << "_Ddash" << Ddash << "_pitchTimes" << static_cast<int>(ptimes) << ".png";
//                //    cout << "zi:" << zi << endl;
//
//                //    for (int i = 0; i < px_height_img; i++) {
//                //        for (int j = 0; j < px_width_img; j++) {
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
//                //    img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//                //}
//
//
//            }
//
//            cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//            result[index++] = sum_time / TIMES;
//
//            //try {
//            //    std::string filename_original, filename_compared;
//
//            //    std::cout << "Please enter the name of original image file: ";
//            //    std::getline(std::cin, filename_original);
//            //    std::cout << "Please enter the name of compared image file: ";
//            //    std::getline(std::cin, filename_compared);
//
//            //    Mat img1 = readImage(filename_original);
//            //    Mat img2 = readImage(filename_compared);
//
//            //    if (img1.empty()) {
//            //        cout << "Coudisplay_size not open or find the image: " << filename_original << endl;
//            //        return -1;
//            //    }
//            //    if (img2.empty()) {
//            //        cout << "Could not open or find the image: " << filename_compared << endl;
//            //        return -1;
//            //    }
//
//            //    double psnrValue = getPSNR(img1, img2);
//            //    cout << "PSNR value is: " << psnrValue << " dB" << endl;
//
//            //    Mat img_diff = getDiff(img1, img2);
//
//            //    // img_diffをファイルとして保存して内容を確認する
//            //    imwrite("img_diff.png", img_diff);
//
//            //    // img_diffのコントラストを強調する
//            //    Mat img_diff_enhanced;
//            //    normalize(img_diff, img_diff_enhanced, 0, 255, NORM_MINMAX, CV_8U);
//
//            //    //cv::namedWindow("Difference image", cv::WINDOW_AUTOSIZE);
//            //    //cv::imshow("Difference image", img_diff_enhanced);
//
//            //    //cv::waitKey(0);
//            //    //cv::destroyWindow("Difference image");
//
//            //    ostringstream stream;
//            //    stream << "./images/v1-3v_v11-6v_diff_img.png";
//            //    cv::String filename = stream.str();
//
//            //    imwrite(filename, img_diff);
//
//            //}
//            //catch (cv::Exception& e) {
//            //    cout << "OpenCV exception: " << e.what() << endl;
//            //    return -1;
//            //}
//            //catch (std::exception& e) {
//            //    cout << "Standard exception: " << e.what() << endl;
//            //    return -1;
//            //}
//            //catch (...) {
//            //    cout << "Unknown exception occurred." << endl;
//            //    return -1;
//            //}
//
//            // 使用したメモリを解放
//            for (int i = 0; i < rows; ++i) {
//                free(data[i]);
//            }
//            free(data);
//
//            for (int i = 0; i < display_image_px; ++i) {
//                free(val_z[i]);
//            }
//            free(val_z);
//
//            free(sp);
//            free(tp);
//
//        }
//    }
//
//    //writeCSV1(result);
//
//    return EXIT_SUCCESS;
//}
///* 点群データから4次元光線情報の再構成画像を作成するプログラム */
///* wideview版にレンズ歪み補正を導入 */
///* ガウシアンフィルタの導入 */
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
//#include <Windows.h>
//
//using namespace std;
//using namespace cv;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//void insert_pixels(int start, int end, int element_image_px, int num_pinhole, double intv, int num_z_level, int px_width_img, int px_height_img, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int* startu, int* startv);
//int writeCSV2(const std::vector<std::vector<double>> array, int NxNy, int ptimes);
//
//int main(int argc, char* argv[])
//{
//
//    cout << "SMC-prop-wideview-v2" << endl;
//
//    //std::vector<std::vector<double>> array(5, std::vector<double>(6)); // 横：subz, 縦：ptimes
//
//    int index = 0;
//    bool interpolation = true;
//    for (int NxNy = 300; NxNy <= 300; NxNy += 100) {
//
//        for (int nph = 40; nph <= 40; nph *= 2) {
//
//            int idx_pt = 0;
//            for (int pt = 3; pt <= 3; pt++) {
//
//                std::vector<std::vector<double>> array(19, std::vector<double>(78)); // 横：subz, 縦：nzl
//
//                int idx_nzl = 0;
//                for (int nzl = 60; nzl <= 60; nzl += 5) {
//
//                    int idx_subz = 0;
//                    for (double subz = 300.0; subz <= 300.0; subz += 100) {
//
//
//                        // 観察者のパラメータ
//                        double zo_min = 1000.0;
//
//                        // ディスプレイのパラメータ(mm)
//                        double display_size = 13.4 * 25.4; // ディスプレイサイズ
//                        int display_width_px = 3840, display_height_px = 2400; // ディスプレイの縦横の解像度
//                        double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ
//
//                        // ピンホールアレイのパラメータ(mm)
//                        double pinhole_pitch = 4.51;    // ピンホールピッチ
//                        int num_pinhole = nph;  // 各軸方向のピンホール数
//                        double pinhole_array_size = pinhole_pitch * num_pinhole;   // 各軸方向のピンホールアレイのサイズ
//
//                        // 表示系のパラメータ(mm)
//                        double focal_length = 7.9667; // ギャップ(zo_min / (3 * nph - 1))
//                        double display_area_size = (focal_length + zo_min) / zo_min * pinhole_array_size; //表示画像の解像度
//                        double intv = (focal_length + zo_min) / zo_min * pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//                        int element_image_px = static_cast<int>(floor(intv)); // 要素画像の解像度
//                        int display_px = static_cast<int>(floor((focal_length + zo_min) / zo_min * pinhole_array_size / display_pixel_pitch)); // 各軸方向の表示画像の解像度
//
//                        cout << "display area size:" << display_area_size << fixed << setprecision(3) << ", intv:" << intv << fixed << setprecision(3) << ", element image px:" << element_image_px << fixed << setprecision(3) << ", display px:" << display_px << fixed << setprecision(3) << endl;
//
//                        // 被写体のパラメータ(mm)
//                        int subject_image_resolution = 554; // 被写体の解像度
//                        double subject_size = pinhole_array_size / 2; // 被写体のサイズ(拡大する場合 * (subz + zo_min) / zo_minを追加)
//                        double subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
//                        double subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
//                        double subject_z = subz; // 被写体の奥行き方向の位置
//
//                        // 点群データ配列の行数と列数
//                        int rows = subject_image_resolution * subject_image_resolution;
//                        int cols = 6;
//
//                        // パラメータ
//                        int num_z_level = nzl; // イメージプレーンの層数
//                        double ptimes = pt; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//                        int half_box_size = static_cast<int>(floor(ptimes / 2.0)); // 探索範囲
//
//                        double Ddash = 200.0;
//                        double coef = (double)num_z_level * Ddash;
//                        double inv_coef = 1.0 / coef;
//
//                        double img_pitch = display_pixel_pitch * 100 / NxNy; // 詳細化の場合 /ptimes をかける
//
//                        int px_height_img = NxNy; // 詳細化の場合 static_cast<int>(100 * ptimes) に変更
//                        int px_width_img = NxNy; // 詳細化の場合 static_cast<int>(100 * ptimes) に変更
//                        std::cout << "Nx:" << px_width_img << ", Ny:" << px_height_img << std::endl;
//
//                        int TIMES = 10;
//
//                        cout << "NumPinhole:" << num_pinhole << ", NumZLevel:" << num_z_level << ", subjectZ:" << subject_z << ", pitchTimes:" << ptimes << endl;
//
//                        // 各要素画像の原点画素位置(左上)
//                        int* startu = (int*)malloc(sizeof(int) * num_pinhole);
//                        int* startv = (int*)malloc(sizeof(int) * num_pinhole);
//                        for (int i = 0; i < num_pinhole; i++) {
//                            startu[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//                            startv[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
//                        }
//
//                        // 3次元配列のインデックス
//                        int*** nx = (int***)malloc(sizeof(int**) * element_image_px);
//                        int*** ny = (int***)malloc(sizeof(int**) * element_image_px);
//                        for (int i = 0; i < element_image_px; i++) {
//
//                            nx[i] = (int**)malloc(sizeof(int*) * num_pinhole);
//                            ny[i] = (int**)malloc(sizeof(int*) * num_pinhole);
//
//                            for (int j = 0; j < num_pinhole; j++) {
//                                nx[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//                            }
//                            for (int j = 0; j < num_pinhole; j++) {
//                                ny[i][j] = (int*)malloc(sizeof(int) * num_z_level);
//                            }
//                        }
//
//                        double s, t, u, v, r, mag, du, dv, xt, yt, zt, nz;
//                        double appr_coef[4] = { -0.00294165634810498, 0.00106133918907954, 0.0104433635159957, 0.000148003007096005 };
//                        for (int i = 0; i < element_image_px; i++) {
//                            for (int j = 0; j < element_image_px; j++) {
//
//                                for (int k = 0; k < num_pinhole; k++) {
//                                    for (int l = 0; l < num_pinhole; l++) {
//
//                                        s = (l - (num_pinhole - 1) * 0.5) * pinhole_pitch;
//                                        t = (k - (num_pinhole - 1) * 0.5) * pinhole_pitch;
//
//                                        u = ((double)j - (double)(element_image_px - 1) * 0.5) * display_pixel_pitch + focal_length / zo_min * s;
//                                        v = ((double)i - (double)(element_image_px - 1) * 0.5) * display_pixel_pitch + focal_length / zo_min * t;
//
//                                        r = sqrt(u * u + v * v);
//                                        mag = appr_coef[0] * pow(r, 3.0) + appr_coef[1] * pow(r, 2.0) + appr_coef[2] * r + appr_coef[3];
//
//                                        du = mag * cos(atan2(v, u));
//                                        dv = mag * sin(atan2(v, u));
//
//                                        u += du;
//                                        v += dv;
//
//                                        zt = (double)(num_z_level - 1) * inv_coef;
//                                        xt = s * zt + u / focal_length;
//                                        yt = t * zt + v / focal_length;
//
//                                        for (int nz = num_z_level - 1; nz >= 0; nz--) {
//
//                                            nx[j][l][nz] = static_cast<int>(floor((focal_length / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                                            ny[i][k][nz] = static_cast<int>(floor((focal_length / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//
//                                            zt -= inv_coef;
//                                            xt -= s * inv_coef;
//                                            yt -= t * inv_coef;
//                                        }
//                                    }
//                                }
//                            }
//                        }
//
//                        double** data = (double**)malloc(sizeof(double*) * rows);
//                        for (int i = 0; i < rows; i++) {
//                            data[i] = (double*)malloc(sizeof(double) * cols);
//                        }
//
//                        std::string filenamein = "./images/standard/grid_image.png";
//                        cv::Mat image_input = cv::imread(filenamein);
//
//                        if (image_input.empty())
//                        {
//                            std::cout << "画像を開くことができませんでした。\n";
//                            return -1;
//                        }
//
//                        cv::Mat resized_image;
//                        cv::resize(image_input, resized_image, cv::Size(554, 554), 0, 0, cv::INTER_NEAREST);
//
//                        for (int i = 0; i < subject_image_resolution; i++) {
//                            for (int j = 0; j < subject_image_resolution; j++) {
//                                data[i * subject_image_resolution + j][0] = subject_position_offset + j * subject_pixel_pitch;
//                                data[i * subject_image_resolution + j][1] = subject_position_offset + i * subject_pixel_pitch;
//                                data[i * subject_image_resolution + j][2] = subject_z;
//                                data[i * subject_image_resolution + j][3] = resized_image.at<Vec3b>(i, j)[0];
//                                data[i * subject_image_resolution + j][4] = resized_image.at<Vec3b>(i, j)[1];
//                                data[i * subject_image_resolution + j][5] = resized_image.at<Vec3b>(i, j)[2];
//                            }
//                        }
//
//                        int*** red;
//                        int*** green;
//                        int*** blue;
//                        bool*** alpha;
//                        int*** num_pcd;
//                        //double*** val_z;
//
//                        // red = new int**[num_z_level];
//                        // green = new int**[num_z_level];
//                        // blue = new int**[num_z_level];
//                        // alpha = new bool**[num_z_level];
//                        // val_z = new int**[num_z_level];
//
//                        red = (int***)malloc(sizeof(int**) * num_z_level);
//                        green = (int***)malloc(sizeof(int**) * num_z_level);
//                        blue = (int***)malloc(sizeof(int**) * num_z_level);
//                        alpha = (bool***)malloc(sizeof(bool**) * num_z_level);
//                        num_pcd = (int***)malloc(sizeof(int**) * num_z_level);
//                        //val_z = (double***)malloc(sizeof(double**) * num_z_level);
//
//                        for (int i = 0; i < num_z_level; i++) {
//
//                            // red[i] = new int*[px_height_img];
//                            // green[i] = new int*[px_height_img];
//                            // blue[i] = new int*[px_height_img];
//                            // alpha[i] = new bool*[px_height_img];
//                            // val_z[i] = new int*[px_height_img];
//
//                            red[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                            green[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                            blue[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                            alpha[i] = (bool**)malloc(sizeof(bool*) * px_height_img);
//                            num_pcd[i] = (int**)malloc(sizeof(int*) * px_height_img);
//                            //val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
//
//                            for (int j = 0; j < px_height_img; j++) {
//
//                                // red[i][j] = new int[px_width_img];
//                                // green[i][j] = new int[px_width_img];
//                                // blue[i][j] = new int[px_width_img];
//                                // alpha[i][j] = new bool[px_width_img];
//                                // val_z[i][j] = new int[px_width_img];
//
//                                red[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                                green[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                                blue[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                                alpha[i][j] = (bool*)malloc(sizeof(bool) * px_width_img);
//                                num_pcd[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//                                //val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
//
//                            }
//                        }
//
//                        cv::Mat img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
//
//                        double sum_time = 0;
//                        // フレーム処理
//                        for (int tt = 0; tt < TIMES; tt++) {
//
//                            // 測定開始時刻を記録
//                            auto start = std::chrono::high_resolution_clock::now();
//
//                            // 箱とzバッファをオフセット
//                            for (int i = 0; i < num_z_level; i++) {
//                                for (int j = 0; j < px_height_img; j++) {
//                                    for (int k = 0; k < px_width_img; k++) {
//                                        //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
//                                        red[i][j][k] = 0;
//                                        green[i][j][k] = 0;
//                                        blue[i][j][k] = 0;
//                                        alpha[i][j][k] = false;
//                                        num_pcd[i][j][k] = 0;
//                                    }
//                                }
//                            }
//
//                            for (int i = 0; i < display_px; i++) {
//                                for (int j = 0; j < display_px; j++) {
//
//                                    img_display.at<cv::Vec3b>(i, j)[0] = 0;
//                                    img_display.at<cv::Vec3b>(i, j)[1] = 0;
//                                    img_display.at<cv::Vec3b>(i, j)[2] = 0;
//
//                                }
//                            }
//
//                            double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//                            double tmp_xt, tmp_yt, tmp_zt;
//                            int tmp_nx, tmp_ny, tmp_nz;
//
//                            // 補間しない場合
//                            if (!interpolation) {
//                                for (int k = 0; k < rows; k++) {
//
//                                    tmp_pcd_x = data[k][0];
//                                    tmp_pcd_y = data[k][1];
//                                    tmp_pcd_z = data[k][2];
//                                    tmp_pcd_b = data[k][3];
//                                    tmp_pcd_g = data[k][4];
//                                    tmp_pcd_r = data[k][5];
//
//                                    tmp_zt = 1.0 / tmp_pcd_z;
//                                    tmp_xt = tmp_pcd_x * tmp_zt;
//                                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                                    //cout << "dx:" << dx << ", dy:" << dy << endl;
//
//                                    if (0 <= tmp_nz && tmp_nz < num_z_level) {
//
//                                        if (0 <= tmp_nx && tmp_nx < px_width_img && 0 <= tmp_ny && tmp_ny < px_height_img) {
//                                            //cout << "nz:" << tmp_nz << endl;
//                                            blue[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_b);
//                                            green[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_g);
//                                            red[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_r);
//                                            alpha[tmp_nz][tmp_ny][tmp_nx] = true;
//                                            num_pcd[tmp_nz][tmp_ny][tmp_nx]++;
//                                        }
//                                    }
//                                }
//
//                            }
//                            // 補間する場合
//                            else {
//
//                                //点群を箱に格納
//                                if ((int)ptimes % 2 == 1) {
//                                    for (int k = 0; k < rows; k++) {
//
//                                        tmp_pcd_x = data[k][0];
//                                        tmp_pcd_y = data[k][1];
//                                        tmp_pcd_z = data[k][2];
//                                        tmp_pcd_b = data[k][3];
//                                        tmp_pcd_g = data[k][4];
//                                        tmp_pcd_r = data[k][5];
//
//                                        tmp_zt = 1.0 / tmp_pcd_z;
//                                        tmp_xt = tmp_pcd_x * tmp_zt;
//                                        tmp_yt = tmp_pcd_y * tmp_zt;
//
//                                        tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                                        tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                                        tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                                        //cout << "dx:" << dx << ", dy:" << dy << endl;
//
//                                        if (0 <= tmp_nz && tmp_nz < num_z_level) {
//
//                                            for (int m = -half_box_size; m <= half_box_size; m++) {
//                                                for (int n = -half_box_size; n <= half_box_size; n++) {
//                                                    if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
//                                                        //cout << "nz:" << tmp_nz << endl;
//                                                        blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                                                        green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                                                        red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                                                        alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                                                        num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                                                    }
//                                                }
//                                            }
//                                        }
//                                    }
//                                }
//                                else {
//
//                                    double dx, dy;
//                                    int half_box_size_min_x, half_box_size_max_x, half_box_size_min_y, half_box_size_max_y;
//
//                                    for (int k = 0; k < rows; k++) {
//
//                                        tmp_pcd_x = data[k][0];
//                                        tmp_pcd_y = data[k][1];
//                                        tmp_pcd_z = data[k][2];
//                                        tmp_pcd_b = data[k][3];
//                                        tmp_pcd_g = data[k][4];
//                                        tmp_pcd_r = data[k][5];
//
//                                        tmp_zt = 1.0 / tmp_pcd_z;
//                                        tmp_xt = tmp_pcd_x * tmp_zt;
//                                        tmp_yt = tmp_pcd_y * tmp_zt;
//
//                                        tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                                        tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                                        tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                                        dx = tmp_pcd_x - (tmp_nx - 0.5 - px_width_img * 0.5) * img_pitch / focal_length * tmp_zt;
//                                        dy = tmp_pcd_y - (tmp_ny - 0.5 - px_width_img * 0.5) * img_pitch / focal_length * tmp_zt;
//
//                                        half_box_size_min_x = static_cast<int>(round(dx / abs(dx) * 0.5 - half_box_size * 0.5));
//                                        half_box_size_max_x = static_cast<int>(round(dx / abs(dx) * 0.5 + half_box_size * 0.5));
//                                        half_box_size_min_y = static_cast<int>(round(dy / abs(dy) * 0.5 - half_box_size * 0.5));
//                                        half_box_size_max_y = static_cast<int>(round(dy / abs(dy) * 0.5 + half_box_size * 0.5));
//
//                                        //cout << "min x:" << half_box_size_min_x << ", max x:" << half_box_size_max_x << ", min y:" << half_box_size_min_y << ", max y:" << half_box_size_max_y << endl;
//
//                                        if (0 <= tmp_nz && tmp_nz < num_z_level) {
//                                            for (int m = half_box_size_min_y; m <= half_box_size_max_y; m++) {
//                                                for (int n = half_box_size_min_x; n <= half_box_size_max_x; n++) {
//                                                    if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
//                                                        //cout << "nz:" << tmp_nz << endl;
//                                                        blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                                                        green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                                                        red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                                                        alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                                                        num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                                                    }
//                                                }
//                                            }
//                                        }
//                                    }
//                                }
//                            }
//
//                            for (int i = 0; i < num_z_level; i++) {
//                                for (int j = 0; j < px_height_img; j++) {
//                                    for (int k = 0; k < px_width_img; k++) {
//                                        //cout << i << ", " << j << ", " << k << "; " << "red:" << red[i][j][k] << ", green:" << green[i][j][k] << ", blue:" << blue[i][j][k] << endl;
//                                        if (alpha[i][j][k]) {
//                                            red[i][j][k] = red[i][j][k] / num_pcd[i][j][k];
//                                            green[i][j][k] = green[i][j][k] / num_pcd[i][j][k];
//                                            blue[i][j][k] = blue[i][j][k] / num_pcd[i][j][k];
//                                        }
//                                    }
//                                }
//                            }
//
//                            // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, nx, ny, startu, startv);
//
//                            const int numThreads = 15;
//                            vector<thread> threads;
//                            int rowsPerThread = element_image_px / numThreads;
//
//                            int startRow, endRow;
//                            for (int i = 0; i < numThreads; i++) {
//                                startRow = i * rowsPerThread;
//                                endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
//                                threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, num_pinhole, intv, num_z_level, px_width_img, px_height_img, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(startu), std::ref(startv));
//                            }
//
//                            for (auto& t : threads) {
//                                if (t.joinable()) { t.join(); }
//                            }
//
//                            // 全てのスレッドが終了するのを待つ
//                            {
//                                std::unique_lock<std::mutex> lock(mtx);
//                                conv.wait(lock, [] {return finished_threads == numThreads; });
//                            }
//
//                            //cout << "calc finished" << endl;
//                            finished_threads = 0;
//
//                            // 測定終了時刻を記録
//                            auto end = std::chrono::high_resolution_clock::now();
//
//                            // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                            sum_time += duration.count();
//
//                        }
//
//                        //cv::imshow("image", img_display);
//                        //cv::waitKey(0);
//
//                        // ガウシアンフィルタを適用
//                        int tmp_startv, tmp_startu;
//                        for (int i = 0; i < num_pinhole; i++) {
//
//                            //tmp_startv = startv[i];
//                            tmp_startv = static_cast<int>(round(i * intv));
//
//                            for (int j = 0; j < num_pinhole; j++) {
//
//                                //tmp_startu = startv[j];
//                                tmp_startu = static_cast<int>(round(j * intv));
//
//                                // 各要素画像に対して個別にガウシアンフィルタを適用
//                                // 要素画像を切り出す
//                                cv::Rect roi(tmp_startu, tmp_startv, element_image_px, element_image_px); // 範囲を指定
//                                cv::Mat element_image = img_display(roi);
//
//                                // ガウシアンフィルタを適用
//                                cv::GaussianBlur(element_image, element_image, cv::Size(13, 13), 0);
//                            }
//                        }
//
//                        // 表示画像の保存
//                        ostringstream stream;
//                        //stream << "D:/EvacuatedStorage/prop-reconstruction/prop-improve-v1/prop-improve-v1-grid1_tileNotExpand_Nx" << px_height_img << "_Ny" << px_width_img << "_Nz" << nzl << "_N" << ptimes << "_zi" << (int)subz << ".png";
//                        stream << "D:/EvacuatedStorage/prop-reconstruction/SMC-prop-wideview-v2/prop-wideview-v2-grid1_tileNotExpand_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << subject_size << "_zi" << (int)subz << ".png";
//                        cv::String filename = stream.str();
//                        imwrite(filename, img_display);
//
//                        //stream.str("");
//                        //stream.clear(ostringstream::goodbit);
//
//                        // 各イメージプレーンの画像を保存（テスト用）
//                        //ostringstream stream;
//                        //cv::String filename;
//                        //   cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//                        //for (int zi = 24; zi < 25; zi++) {
//
//                        //    stream << "E:/EvacuatedStorage/image-plane/prop-improve/prop-improve-v1-detail-milkdrop_tileExpand_Nz" << nzl << "_N" << ptimes << "_subjectZ" << (int)subz << "_zi" << zi << ".png";
//                        //    cout << "zi:" << zi << endl;
//
//                        //    for (int i = 0; i < px_height_img; i++) {
//                        //        for (int j = 0; j < px_width_img; j++) {
//                        //            if (alpha[zi][i][j] > 0) {
//                        //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
//                        //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
//                        //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
//                        //            }
//                        //        }
//                        //    }
//                        //    filename = stream.str();
//                        //    imwrite(filename, img);
//                        //    stream.str("");
//                        //    stream.clear(ostringstream::goodbit);
//                        //    img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//                        //}
//
//                        cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//                        array[idx_nzl][idx_subz++] = sum_time / TIMES;
//
//                        // 使用したメモリを解放
//                        for (int i = 0; i < rows; ++i) {
//                            free(data[i]);
//                        }
//                        free(data);
//
//                        for (int i = 0; i < num_z_level; i++) {
//                            for (int j = 0; j < px_height_img; j++) {
//                                free(red[i][j]);
//                                free(green[i][j]);
//                                free(blue[i][j]);
//                                free(alpha[i][j]);
//                                free(num_pcd[i][j]);
//                            }
//                            free(red[i]);
//                            free(green[i]);
//                            free(blue[i]);
//                            free(alpha[i]);
//                            free(num_pcd[i]);
//                        }
//                        free(red);
//                        free(green);
//                        free(blue);
//                        free(alpha);
//                        free(num_pcd);
//
//                        //free(sp);
//                        //free(tp);
//
//                        free(startu);
//                        free(startv);
//
//                        //for (int i = 0; i < element_image_px; i++) {
//                        //    free(xt[i]);
//                        //    free(yt[i]);
//                        //}
//                        //free(xt);
//                        //free(yt);
//
//                        for (int i = 0; i < element_image_px; i++) {
//                            for (int j = 0; j < num_pinhole; j++) {
//                                free(nx[i][j]);
//                            }
//                            for (int j = 0; j < num_pinhole; j++) {
//                                free(ny[i][j]);
//                            }
//                            free(nx[i]);
//                            free(ny[i]);
//                        }
//
//                    }
//                    idx_nzl++;
//
//                }
//                //writeCSV2(array, NxNy, pt);
//            }
//        }
//
//    }
//
//    MessageBeep(-1);
//    return EXIT_SUCCESS;
//}
//
//void insert_pixels(int start, int end, int element_image_px, int num_pinhole, double intv, int num_z_level, int px_width_img, int px_height_img, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int* startu, int* startv) {
//
//    int tmp_nx, tmp_ny;
//    int tmp_startu, tmp_startv;
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
//int writeCSV2(const std::vector<std::vector<double>> array, int NxNy, int ptimes) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/speed/speed-prop-improve-v1-milkdrop_tileExpand_NxNy" + std::to_string(NxNy) + "_pt" + std::to_string(ptimes) + ".csv");
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

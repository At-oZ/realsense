/* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
/* parrots画像を用いたプログラム */
/* v1-10と同じアルゴリズム */
/*
    v2-2との差分:点群取得カメラの位置を観察位置またはその1/2倍した距離の位置に固定
    →特に1/2倍した距離の位置に置いた場合、観察範囲に存在する点群は全て3次元配列内に集約できる
*/

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

void insert_pixels(int start, int end, int each_camera_px, int num_pinhole, int num_z_level, int boxel_cam_width_px, int boxel_cam_height_px, cv::Mat img_camera_array, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny) {

    int tmp_nx, tmp_ny;

    // 各要素画像の各画素ごとに
    for (int m = start; m < end; m++) {

        for (int n = 0; n < each_camera_px; n++) {

            // 各要素カメラごとに
            for (int i = 0; i < num_pinhole; i++) {

                for (int j = 0; j < num_pinhole; j++) {

                    // 各奥行きレベルごとに(手前から)
                    for (int nz = num_z_level - 1; nz >= 0; nz--) {

                        tmp_nx = nx[n][j][nz];
                        tmp_ny = ny[m][i][nz];

                        //cout << "nx:" << nx << ", ny:" << ny << endl;
                        if (0 <= tmp_nx && tmp_nx < boxel_cam_width_px && 0 <= tmp_ny && tmp_ny < boxel_cam_height_px) {
                            if (alpha[nz][tmp_ny][tmp_nx]) {

                                img_camera_array.at<cv::Vec3b>(i * each_camera_px + m, j * each_camera_px + n)[2] = static_cast<int>(red[nz][tmp_ny][tmp_nx]);
                                img_camera_array.at<cv::Vec3b>(i * each_camera_px + m, j * each_camera_px + n)[1] = static_cast<int>(green[nz][tmp_ny][tmp_nx]);
                                img_camera_array.at<cv::Vec3b>(i * each_camera_px + m, j * each_camera_px + n)[0] = static_cast<int>(blue[nz][tmp_ny][tmp_nx]);
                                // cout << "v, u:" << startv + m << ", " << startu + n << endl;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // スレッドの終了をカウントし、条件変数を通知する
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished_threads++;
        conv.notify_one(); // 1つの待機スレッドに通知
    }
}

int writeCSV1(const std::vector<double> array) {

    // 出力ファイルを開く
    std::ofstream file("./images/parrots/prop-reconstruction/average-time-v2-3-note-nzl64-img_px240-nonInterpolation.csv");

    // ファイルが正しく開けたか確認
    if (!file.is_open()) {
        std::cerr << "ファイルが開けませんでした" << std::endl;
        return 1;
    }

    // 1次元配列の内容をCSV形式でファイルに書き込む
    for (const auto& row : array) {

        file << row;
        file << "\n"; // 行の終わりに改行を挿入
    }

    // ファイルを閉じる
    file.close();

    std::cout << "書き込みが完了しました。" << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{

    cout << "ICIP-prop-wideview" << endl;

    std::vector<double> result(45);

    int index = 0;
    bool interpolation = true; // 補間処理を行うかのフラッグ
    for (int nph = 40; nph <= 40; nph *= 2) {


        for (int nzl = 40; nzl <= 40; nzl += 10) {

            int idx_subz = 0;
            for (double subz = 512.0; subz <= 512.0; subz *= 2) {

                int idx_pt = 0;
                for (int pt = 3; pt <= 3; pt++) {

                    // 観察者のパラメータ
                    double zo_min = 1000.0;

                    // ディスプレイのパラメータ(mm)
                    double display_size = 13.4 * 25.4; // ディスプレイサイズ
                    int display_width_px = 3840, display_height_px = 2400; // ディスプレイの縦横の解像度
                    double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ

                    // ピンホールアレイのパラメータ(mm)
                    double pinhole_array_size = display_pixel_pitch * display_height_px;   // 各軸方向のピンホールアレイのサイズ
                    int num_pinhole = nph;  // 各軸方向のピンホール数
                    double pinhole_pitch = pinhole_array_size / num_pinhole;    // ピンホールピッチ

                    std::cout << "pinhole pitch:" << pinhole_pitch << "mm" << std::endl;

                    // 表示系のパラメータ(mm)
                    double focal_length = zo_min / (3 * nph - 1); // ギャップ
                    std::cout << "focal length:" << focal_length << std::endl;

                    // 観察者のパラメータ
                    double observe_z = 1000.0; // 観察者と表示系の距離
                    double observe_range = (observe_z + focal_length) / focal_length * pinhole_pitch; // 視域

                    // 要素画像のパラメータ
                    int element_image_px = static_cast<int>(floor(focal_length / observe_z * observe_range / display_pixel_pitch)); // 要素画像の解像度
                    int display_px = element_image_px * num_pinhole; // 各軸方向の表示画像の解像度
                    double interval_element_image = focal_length / observe_z * observe_range; // 要素画像の物理的間隔(mm)

                    std::cout << "element image pixel:" << element_image_px << "px" << std::endl;
                    std::cout << "interval element image:" << interval_element_image << "mm" << std::endl;

                    // 表示系用カメラアレイのパラメータ
                    double camera_array_range = focal_length / observe_z * (pinhole_array_size + observe_range); // 要素カメラの画角
                    int each_camera_px = static_cast<int>(floor(camera_array_range / display_pixel_pitch)); // 要素カメラの解像度

                    // 被写体のパラメータ(mm)
                    int subject_image_resolution = 554; // 被写体の解像度
                    double subject_size = pinhole_array_size * (subz + zo_min) / zo_min; // 被写体のサイズ(拡大する場合 * (subz + zo_min) / zo_minを追加); // 被写体のサイズ
                    double subject_pixel_pitch = subject_size / subject_image_resolution; // 被写体の画素ピッチ
                    double subject_position_offset = -((subject_size - subject_pixel_pitch) / 2.0); // 被写体の左上の位置
                    double subject_z = subz; // 被写体の奥行き方向の位置

                    // 点群データ配列の行数と列数
                    int rows = subject_image_resolution * subject_image_resolution;
                    int cols = 6;

                    // パラメータ
                    int num_z_level = nzl; // イメージプレーンの層数
                    double ptimes = pt; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
                    int half_box_size = static_cast<int>(floor(ptimes / 2.0)); // 探索範囲

                    double Ddash = 200.0;
                    double coef = (double)num_z_level * Ddash;
                    double inv_coef = 1.0 / coef;

                    double img_pitch = display_pixel_pitch / ptimes;
                    int px_height_img = static_cast<int>(round(100 * ptimes));
                    int px_width_img = static_cast<int>(round(100 * ptimes));
                    std::cout << "Nx:" << px_width_img << ", Ny:" << px_height_img << std::endl;

                    int TIMES = 1;

                    cout << "NumPinhole:" << num_pinhole << ", NumZLevel:" << num_z_level << ", subjectZ:" << subject_z << ", pitchTimes:" << ptimes << endl;

                    // 各要素画像の原点画素位置(左上)
                    int* startu = (int*)malloc(sizeof(int) * num_pinhole);
                    int* startv = (int*)malloc(sizeof(int) * num_pinhole);
                    for (int i = 0; i < num_pinhole; i++) {
                        startu[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
                        startv[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
                    }

                    // 3次元配列のインデックス
                    int*** nx = (int***)malloc(sizeof(int**) * each_camera_px);
                    int*** ny = (int***)malloc(sizeof(int**) * each_camera_px);
                    for (int i = 0; i < each_camera_px; i++) {

                        nx[i] = (int**)malloc(sizeof(int*) * num_pinhole);
                        ny[i] = (int**)malloc(sizeof(int*) * num_pinhole);

                        for (int j = 0; j < num_pinhole; j++) {
                            nx[i][j] = (int*)malloc(sizeof(int) * num_z_level);
                        }
                        for (int j = 0; j < num_pinhole; j++) {
                            ny[i][j] = (int*)malloc(sizeof(int) * num_z_level);
                        }
                    }

                    double u, x, y, z, xt, yt, zt, nz;
                    for (int i = 0; i < each_camera_px; i++) {

                        u = ((double)i - (double)(each_camera_px - 1) * 0.5) * display_pixel_pitch;

                        for (int j = 0; j < num_pinhole; j++) {

                            zt = (double)(num_z_level - 1) * inv_coef;
                            xt = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * zt + u / focal_length;

                            for (int nz = num_z_level - 1; nz >= 0; nz--) {
                                nx[i][j][nz] = static_cast<int>(floor((focal_length / img_pitch) * xt + 0.5) + px_width_img * 0.5);
                                zt -= inv_coef;
                                xt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
                            }
                        }

                        for (int j = 0; j < num_pinhole; j++) {

                            zt = (double)(num_z_level - 1) * inv_coef;
                            yt = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * zt + u / focal_length;

                            for (int nz = num_z_level - 1; nz >= 0; nz--) {
                                ny[i][j][nz] = static_cast<int>(floor((focal_length / img_pitch) * yt + 0.5) + px_height_img * 0.5);
                                zt -= inv_coef;
                                xt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
                            }
                        }
                    }

                    double** data = (double**)malloc(sizeof(double*) * rows);
                    for (int i = 0; i < rows; i++) {
                        data[i] = (double*)malloc(sizeof(double) * cols);
                    }

                    std::string filenamein = "./images/standard/parrots.bmp";
                    cv::Mat image_input = cv::imread(filenamein);

                    if (image_input.empty())
                    {
                        std::cout << "画像を開くことができませんでした。\n";
                        return -1;
                    }

                    cv::Mat resized_image;
                    cv::resize(image_input, resized_image, cv::Size(554, 554), 0, 0, cv::INTER_NEAREST);

                    for (int i = 0; i < subject_image_resolution; i++) {
                        for (int j = 0; j < subject_image_resolution; j++) {
                            data[i * subject_image_resolution + j][0] = subject_position_offset + j * subject_pixel_pitch;
                            data[i * subject_image_resolution + j][1] = subject_position_offset + i * subject_pixel_pitch;
                            //data[i * subject_image_resolution + j][2] = subject_z;
                            data[i * subject_image_resolution + j][3] = resized_image.at<Vec3b>(i, j)[0];
                            data[i * subject_image_resolution + j][4] = resized_image.at<Vec3b>(i, j)[1];
                            data[i * subject_image_resolution + j][5] = resized_image.at<Vec3b>(i, j)[2];

                            if (j >= 0 && j < (int)(subject_image_resolution / 3)) {
                                data[i * subject_image_resolution + j][2] = subz / 2.0;
                            }
                            else if (j >= (int)(subject_image_resolution / 3) && j < (int)(subject_image_resolution * 2 / 3)) {
                                data[i * subject_image_resolution + j][2] = subz;
                            }
                            else {
                                data[i * subject_image_resolution + j][2] = subz * 2.0;
                            }
                        }
                    }

                    int*** red;
                    int*** green;
                    int*** blue;
                    bool*** alpha;
                    int*** num_pcd;
                    //double*** val_z;

                    // red = new int**[num_z_level];
                    // green = new int**[num_z_level];
                    // blue = new int**[num_z_level];
                    // alpha = new bool**[num_z_level];
                    // val_z = new int**[num_z_level];

                    red = (int***)malloc(sizeof(int**) * num_z_level);
                    green = (int***)malloc(sizeof(int**) * num_z_level);
                    blue = (int***)malloc(sizeof(int**) * num_z_level);
                    alpha = (bool***)malloc(sizeof(bool**) * num_z_level);
                    num_pcd = (int***)malloc(sizeof(int**) * num_z_level);
                    //val_z = (double***)malloc(sizeof(double**) * num_z_level);

                    for (int i = 0; i < num_z_level; i++) {

                        // red[i] = new int*[px_height_img];
                        // green[i] = new int*[px_height_img];
                        // blue[i] = new int*[px_height_img];
                        // alpha[i] = new bool*[px_height_img];
                        // val_z[i] = new int*[px_height_img];

                        red[i] = (int**)malloc(sizeof(int*) * px_height_img);
                        green[i] = (int**)malloc(sizeof(int*) * px_height_img);
                        blue[i] = (int**)malloc(sizeof(int*) * px_height_img);
                        alpha[i] = (bool**)malloc(sizeof(bool*) * px_height_img);
                        num_pcd[i] = (int**)malloc(sizeof(int*) * px_height_img);
                        //val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);

                        for (int j = 0; j < px_height_img; j++) {

                            // red[i][j] = new int[px_width_img];
                            // green[i][j] = new int[px_width_img];
                            // blue[i][j] = new int[px_width_img];
                            // alpha[i][j] = new bool[px_width_img];
                            // val_z[i][j] = new int[px_width_img];

                            red[i][j] = (int*)malloc(sizeof(int) * px_width_img);
                            green[i][j] = (int*)malloc(sizeof(int) * px_width_img);
                            blue[i][j] = (int*)malloc(sizeof(int) * px_width_img);
                            alpha[i][j] = (bool*)malloc(sizeof(bool) * px_width_img);
                            num_pcd[i][j] = (int*)malloc(sizeof(int) * px_width_img);
                            //val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);

                        }
                    }

                    cv::Mat img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
                    cv::Mat img_camera_array = cv::Mat::zeros(cv::Size(each_camera_px * num_pinhole, each_camera_px * num_pinhole), CV_8UC3);

                    double sum_time = 0;
                    // フレーム処理
                    for (int tt = 0; tt < TIMES; tt++) {

                        // 測定開始時刻を記録
                        auto start = std::chrono::high_resolution_clock::now();

                        // 箱とzバッファをオフセット
                        for (int i = 0; i < num_z_level; i++) {
                            for (int j = 0; j < px_height_img; j++) {
                                for (int k = 0; k < px_width_img; k++) {
                                    //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
                                    red[i][j][k] = 0;
                                    green[i][j][k] = 0;
                                    blue[i][j][k] = 0;
                                    alpha[i][j][k] = false;
                                    num_pcd[i][j][k] = 0;
                                }
                            }
                        }

                        img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
                        img_camera_array = cv::Mat::zeros(cv::Size(each_camera_px * num_pinhole, each_camera_px * num_pinhole), CV_8UC3);

                        //for (int i = 0; i < display_px; i++) {
                        //    for (int j = 0; j < display_px; j++) {

                        //        img_display.at<cv::Vec3b>(i, j)[0] = 0;
                        //        img_display.at<cv::Vec3b>(i, j)[1] = 0;
                        //        img_display.at<cv::Vec3b>(i, j)[2] = 0;

                        //        img_camera_array.at<cv::Vec3b>(i, j)[0] = 0;
                        //        img_camera_array.at<cv::Vec3b>(i, j)[1] = 0;
                        //        img_camera_array.at<cv::Vec3b>(i, j)[2] = 0;

                        //    }
                        //}

                        double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
                        double tmp_xt, tmp_yt, tmp_zt;
                        int tmp_nx, tmp_ny, tmp_nz;
                        double dz;

                        // 補間処理を行わない場合
                        if (!interpolation) {

                            cout << "no interpolation" << endl;
                            for (int k = 0; k < rows; k++) {

                                tmp_pcd_x = data[k][0];
                                tmp_pcd_y = data[k][1];
                                tmp_pcd_z = data[k][2];
                                tmp_pcd_b = data[k][3];
                                tmp_pcd_g = data[k][4];
                                tmp_pcd_r = data[k][5];

                                tmp_zt = 1.0 / tmp_pcd_z;
                                tmp_xt = tmp_pcd_x * tmp_zt;
                                tmp_yt = tmp_pcd_y * tmp_zt;

                                tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
                                tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
                                tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
                                dz = coef / tmp_nz - tmp_pcd_z;

                                //cout << "dx:" << dx << ", dy:" << dy << endl;
                                if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;

                                if (0 <= tmp_nz && tmp_nz < num_z_level) {

                                    if (0 <= tmp_nx && tmp_nx < px_width_img && 0 <= tmp_ny && tmp_ny < px_height_img) {
                                        //cout << "nz:" << tmp_nz << endl;
                                        blue[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_b);
                                        green[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_g);
                                        red[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_r);
                                        alpha[tmp_nz][tmp_ny][tmp_nx] = true;
                                        num_pcd[tmp_nz][tmp_ny][tmp_nx]++;
                                    }
                                }
                            }
                        }
                        else { // 補間処理を行う場合
                            cout << "interpolation" << endl;
                            //点群を箱に格納
                            if ((int)ptimes % 2 == 1) {
                                for (int k = 0; k < rows; k++) {

                                    tmp_pcd_x = data[k][0];
                                    tmp_pcd_y = data[k][1];
                                    tmp_pcd_z = data[k][2];
                                    tmp_pcd_b = data[k][3];
                                    tmp_pcd_g = data[k][4];
                                    tmp_pcd_r = data[k][5];

                                    tmp_zt = 1.0 / tmp_pcd_z;
                                    tmp_xt = tmp_pcd_x * tmp_zt;
                                    tmp_yt = tmp_pcd_y * tmp_zt;

                                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
                                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
                                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
                                    dz = coef / tmp_nz - tmp_pcd_z;

                                    //cout << "dx:" << dx << ", dy:" << dy << endl;
                                    if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;


                                    if (0 <= tmp_nz && tmp_nz < num_z_level) {

                                        for (int m = -half_box_size; m <= half_box_size; m++) {
                                            for (int n = -half_box_size; n <= half_box_size; n++) {
                                                if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
                                                    //cout << "nz:" << tmp_nz << endl;
                                                    blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
                                                    green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
                                                    red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
                                                    alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
                                                    num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else {

                                double dx, dy;
                                int half_box_size_min_x, half_box_size_max_x, half_box_size_min_y, half_box_size_max_y;

                                for (int k = 0; k < rows; k++) {

                                    tmp_pcd_x = data[k][0];
                                    tmp_pcd_y = data[k][1];
                                    tmp_pcd_z = data[k][2];
                                    tmp_pcd_b = data[k][3];
                                    tmp_pcd_g = data[k][4];
                                    tmp_pcd_r = data[k][5];

                                    tmp_zt = 1.0 / tmp_pcd_z;
                                    tmp_xt = tmp_pcd_x * tmp_zt;
                                    tmp_yt = tmp_pcd_y * tmp_zt;

                                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
                                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
                                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));

                                    dx = tmp_pcd_x - (tmp_nx - 0.5 - px_width_img * 0.5) * img_pitch / focal_length * tmp_zt;
                                    dy = tmp_pcd_y - (tmp_ny - 0.5 - px_width_img * 0.5) * img_pitch / focal_length * tmp_zt;

                                    half_box_size_min_x = static_cast<int>(round(dx / abs(dx) * 0.5 - half_box_size * 0.5));
                                    half_box_size_max_x = static_cast<int>(round(dx / abs(dx) * 0.5 + half_box_size * 0.5));
                                    half_box_size_min_y = static_cast<int>(round(dy / abs(dy) * 0.5 - half_box_size * 0.5));
                                    half_box_size_max_y = static_cast<int>(round(dy / abs(dy) * 0.5 + half_box_size * 0.5));

                                    //cout << "min x:" << half_box_size_min_x << ", max x:" << half_box_size_max_x << ", min y:" << half_box_size_min_y << ", max y:" << half_box_size_max_y << endl;

                                    if (0 <= tmp_nz && tmp_nz < num_z_level) {
                                        for (int m = half_box_size_min_y; m <= half_box_size_max_y; m++) {
                                            for (int n = half_box_size_min_x; n <= half_box_size_max_x; n++) {
                                                if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
                                                    //cout << "nz:" << tmp_nz << endl;
                                                    blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
                                                    green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
                                                    red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
                                                    alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
                                                    num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        for (int i = 0; i < num_z_level; i++) {
                            for (int j = 0; j < px_height_img; j++) {
                                for (int k = 0; k < px_width_img; k++) {
                                    //cout << i << ", " << j << ", " << k << "; " << "red:" << red[i][j][k] << ", green:" << green[i][j][k] << ", blue:" << blue[i][j][k] << endl;
                                    if (alpha[i][j][k]) {
                                        red[i][j][k] = red[i][j][k] / num_pcd[i][j][k];
                                        green[i][j][k] = green[i][j][k] / num_pcd[i][j][k];
                                        blue[i][j][k] = blue[i][j][k] / num_pcd[i][j][k];
                                    }
                                }
                            }
                        }

                        // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, nx, ny, startu, startv);

                        const int numThreads = 15;
                        vector<thread> threads;
                        int rowsPerThread = each_camera_px / numThreads;

                        int startRow, endRow;
                        for (int i = 0; i < numThreads; i++) {
                            startRow = i * rowsPerThread;
                            endRow = (i == numThreads - 1) ? each_camera_px : (i + 1) * rowsPerThread;
                            threads.emplace_back(insert_pixels, startRow, endRow, each_camera_px, num_pinhole, num_z_level, px_width_img, px_height_img, std::ref(img_camera_array), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny));
                        }

                        for (auto& t : threads) {
                            if (t.joinable()) { t.join(); }
                        }

                        // 全てのスレッドが終了するのを待つ
                        {
                            std::unique_lock<std::mutex> lock(mtx);
                            conv.wait(lock, [] {return finished_threads == numThreads; });
                        }

                        //cout << "calc finished" << endl;
                        finished_threads = 0;

                        int offsetu, offsetv;
                        for (int i = 0; i < num_pinhole; ++i) {

                            offsetv = static_cast<int>(floor(focal_length / observe_z * pinhole_pitch / display_pixel_pitch * i + 0.5));

                            for (int j = 0; j < num_pinhole; ++j) {

                                offsetu = static_cast<int>(floor(focal_length / observe_z * pinhole_pitch / display_pixel_pitch * j + 0.5));

                                for (int m = 0; m < element_image_px; ++m) {
                                    for (int n = 0; n < element_image_px; ++n) {
                                        img_display.at<cv::Vec3b>(i * element_image_px + m, j * element_image_px + n)[0] = img_camera_array.at<cv::Vec3b>(i * each_camera_px + m + offsetv, j * each_camera_px + n + offsetu)[0];
                                        img_display.at<cv::Vec3b>(i * element_image_px + m, j * element_image_px + n)[1] = img_camera_array.at<cv::Vec3b>(i * each_camera_px + m + offsetv, j * each_camera_px + n + offsetu)[1];
                                        img_display.at<cv::Vec3b>(i * element_image_px + m, j * element_image_px + n)[2] = img_camera_array.at<cv::Vec3b>(i * each_camera_px + m + offsetv, j * each_camera_px + n + offsetu)[2];
                                    }
                                }

                            }
                        }

                        // 測定終了時刻を記録
                        auto end = std::chrono::high_resolution_clock::now();

                        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                        //cout << "実行時間: " << duration.count() << " ms" << std::endl;
                        //cout << "重なり回数:" << pcd_count << endl;
                        sum_time += duration.count();

                    }

                    // 表示画像の保存
                    ostringstream stream;
                    stream << "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ICIP-prop-wideview/prop-wideview-parrots_Nz" << nzl << "_subjectZ" << (int)subz << ".png";
                    cv::String filename_display = stream.str();
                    imwrite(filename_display, img_display);

                    stream.str("");
                    stream.clear(ostringstream::goodbit);

                    // 表示系用カメラアレイ画像の保存
                    //if (interpolation) stream << "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/lensarray/v2-3-wideview/prop-parrots-v2-3_ImgCameraArray_NumPinhole" << num_pinhole << "_NxNy" << boxel_cam_width_px << "_Nz" << num_z_level << "_pitchTimes" << static_cast<int>(ptimes) << "_subjectZ" << (int)subject_z << "interpolation=T.png";
                    //else stream << "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/lensarray/v2-3-wideview/prop-parrots-v2-3_ImgCameraArray_NumPinhole" << num_pinhole << "_NxNy" << boxel_cam_width_px << "_Nz" << num_z_level << "_pitchTimes" << static_cast<int>(ptimes) << "_subjectZ" << (int)subject_z << "interpolation=F.png";
                    //cv::String filename_camera_array = stream.str();
                    //imwrite(filename_camera_array, img_camera_array);

                    //// 各イメージプレーンの画像を保存（テスト用）
                    //// ostringstream stream;
                    //// cv::String filename;
                    //cv::Mat img = cv::Mat::zeros(cv::Size(boxel_cam_width_px, boxel_cam_height_px), CV_8UC3);
                    //for (int zi = 0; zi < num_z_level; zi++) {

                    //    stream << "V:/images/parrots/image-plane/v2-3/prop-parrots-v2-3_imagePlane_gridSize" << nph << "_NumZLevel" << num_z_level << "_subjectZ" << (int)subject_z << "_zi" << zi << "_Ddash" << Ddash << "_pitchTimes" << static_cast<int>(ptimes) << ".png";
                    //    cout << "zi:" << zi << endl;

                    //    for (int i = 0; i < boxel_cam_height_px; i++) {
                    //        for (int j = 0; j < boxel_cam_width_px; j++) {
                    //            if (alpha[zi][i][j] > 0) {
                    //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
                    //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
                    //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
                    //            }
                    //        }
                    //    }
                    //    filename = stream.str();
                    //    imwrite(filename, img);
                    //    stream.str("");
                    //    stream.clear(ostringstream::goodbit);
                    //    img = cv::Mat::zeros(cv::Size(boxel_cam_width_px, boxel_cam_height_px), CV_8UC3);

                    //}

                    cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
                    result[index++] = sum_time / TIMES;

                    // 使用したメモリを解放
                    for (int i = 0; i < rows; ++i) {
                        free(data[i]);
                    }
                    free(data);

                    for (int i = 0; i < num_z_level; i++) {
                        for (int j = 0; j < px_height_img; j++) {
                            free(red[i][j]);
                            free(green[i][j]);
                            free(blue[i][j]);
                            free(alpha[i][j]);
                            free(num_pcd[i][j]);
                        }
                        free(red[i]);
                        free(green[i]);
                        free(blue[i]);
                        free(alpha[i]);
                        free(num_pcd[i]);
                    }
                    free(red);
                    free(green);
                    free(blue);
                    free(alpha);
                    free(num_pcd);

                    //free(sp);
                    //free(tp);

                    free(startu);
                    free(startv);

                    //for (int i = 0; i < element_image_px; i++) {
                    //    free(xt[i]);
                    //    free(yt[i]);
                    //}
                    //free(xt);
                    //free(yt);

                    for (int i = 0; i < element_image_px; i++) {
                        for (int j = 0; j < num_pinhole; j++) {
                            free(nx[i][j]);
                        }
                        for (int j = 0; j < num_pinhole; j++) {
                            free(ny[i][j]);
                        }
                        free(nx[i]);
                        free(ny[i]);
                    }

                }
                idx_subz++;

            }
        }
    }

    //writeCSV1(result);

    return EXIT_SUCCESS;
}
/* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
/* lenna画像を用いたプログラム */
/* v1-10と同じアルゴリズム */
/*
    v2-2との差分:点群取得カメラの位置を観察位置またはその1/2倍した距離の位置に固定
    →特に1/2倍した距離の位置に置いた場合、観察範囲に存在する点群は全て3次元配列内に集約できる
*/

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
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

void insert_pixels(int start, int end, int element_image_px, int num_pinhole, double intv, int num_z_level, int boxel_cam_width_px, int boxel_cam_height_px, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int* startu, int* startv) {

    int tmp_nx, tmp_ny;
    int tmp_startu, tmp_startv;

    // 各要素画像の各画素ごとに
    for (int m = start; m < end; m++) {

        for (int n = 0; n < element_image_px; n++) {

            // 各要素カメラごとに
            for (int i = 0; i < num_pinhole; i++) {

                //tmp_startv = startv[i];
                tmp_startv = static_cast<int>(round(i * intv));

                for (int j = 0; j < num_pinhole; j++) {

                    //tmp_startu = startv[j];
                    tmp_startu = static_cast<int>(round(j * intv));

                    // 各奥行きレベルごとに(手前から)
                    for (int nz = num_z_level - 1; nz >= 0; nz--) {

                        tmp_nx = nx[n][j][nz];
                        tmp_ny = ny[m][i][nz];

                        //cout << "nx:" << nx << ", ny:" << ny << endl;
                        if (0 <= tmp_nx && tmp_nx < boxel_cam_width_px && 0 <= tmp_ny && tmp_ny < boxel_cam_height_px) {
                            if (alpha[nz][tmp_ny][tmp_nx]) {

                                img_display.at<cv::Vec3b>(tmp_startv + m + 40, tmp_startu + n + 790)[2] = static_cast<int>(red[nz][tmp_ny][tmp_nx]);
                                img_display.at<cv::Vec3b>(tmp_startv + m + 40, tmp_startu + n + 790)[1] = static_cast<int>(green[nz][tmp_ny][tmp_nx]);
                                img_display.at<cv::Vec3b>(tmp_startv + m + 40, tmp_startu + n + 790)[0] = static_cast<int>(blue[nz][tmp_ny][tmp_nx]);
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

int main(int argc, char* argv[]) try
{

    cout << "prop-v2-3-trial" << endl;

    std::vector<double> result(45);

    int index = 0;
    bool interpolation = false; // 補間処理を行うかのフラッグ
    int nph = 136;
    int nzl = 40;
    int pt = 4;

    int WIDTH = 640;
    int HEIGHT = 480;
    int FPS = 30;
    double inv_WIDTH = 1.0 / (double)WIDTH;
    double D = 100.0;

    // ディスプレイのパラメータ(mm)
    double display_size = 13.4 * 25.4; // ディスプレイサイズ
    int display_width_px = 3840, display_height_px = 2400; // ディスプレイの縦横の解像度
    double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ

    // ピンホールアレイのパラメータ(mm)
    double pinhole_array_size = 170;   // 各軸方向のピンホールアレイのサイズ
    int num_pinhole = nph;  // 各軸方向のピンホール数
    double pinhole_pitch = pinhole_array_size / num_pinhole;    // ピンホールピッチ

    // 表示系のパラメータ(mm)
    double gap = 4.0; // ギャップ
    int element_image_px = static_cast<int>(floor(pinhole_pitch / display_pixel_pitch)); // 要素画像の解像度
    int display_px = 2261; // 各軸方向の表示画像の解像度
    double intv = pinhole_pitch / display_pixel_pitch; // 要素画像の間隔

    // 点群取得カメラのパラメータ(mm)
    double boxel_cam_focal_length = -1500;
    double boxel_cam_sensor_size = pinhole_array_size;
    int boxel_cam_height_px = 50 * pt;
    int boxel_cam_width_px = 50 * pt;
    double boxel_cam_px_pitch = boxel_cam_sensor_size / (double)boxel_cam_width_px;

    // 点群データ配列の行数と列数
    int rows = WIDTH * HEIGHT;
    int cols = 6;

    // パラメータ
    int num_z_level = nzl; // イメージプレーンの層数
    double ptimes = pt; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
    int half_box_size = static_cast<int>(floor(ptimes / 2.0)); // 探索範囲

    double Ddash = 200.0 - boxel_cam_focal_length;
    double coef = (double)num_z_level * Ddash;
    double inv_coef = 1.0 / coef;

    cout << "NumPinhole:" << num_pinhole  << ", NumZLevel:" << num_z_level << ", pitchTimes:" << ptimes << endl;

    // 各要素画像の原点画素位置(左上)
    int* startu = (int*)malloc(sizeof(int) * num_pinhole);
    int* startv = (int*)malloc(sizeof(int) * num_pinhole);
    for (int i = 0; i < num_pinhole; i++) {
        startu[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
        startv[i] = static_cast<int>(std::round(((i - (num_pinhole - 1) * 0.5) * pinhole_pitch + pinhole_array_size * 0.5 - pinhole_pitch) / display_pixel_pitch));
    }

    // 3次元配列のインデックス
    int*** nx = (int***)malloc(sizeof(int**) * element_image_px);
    int*** ny = (int***)malloc(sizeof(int**) * element_image_px);
    for (int i = 0; i < element_image_px; i++) {

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
    for (int i = 0; i < element_image_px; i++) {

        u = ((double)i - (double)(element_image_px - 1) * 0.5) * display_pixel_pitch;

        for (int j = 0; j < num_pinhole; j++) {

            zt = (double)(num_z_level - 1) * inv_coef;

            for (int nz = num_z_level - 1; nz >= 0; nz--) {
                                
                z = 1.0 / zt + boxel_cam_focal_length;
                x = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch + u / gap * z;
                xt = x * zt;

                nx[i][j][nz] = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * xt + 0.5) + boxel_cam_width_px * 0.5);
                zt -= inv_coef;
                //xt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
            }
        }

        for (int j = 0; j < num_pinhole; j++) {

            zt = (double)(num_z_level - 1) * inv_coef;

            for (int nz = num_z_level - 1; nz >= 0; nz--) {

                z = 1.0 / zt + boxel_cam_focal_length;
                y = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch + u / gap * z;
                yt = y * zt;

                ny[i][j][nz] = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * yt + 0.5) + boxel_cam_height_px * 0.5);
                zt -= inv_coef;
                //xt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
            }
        }
    }


    rs2::config config;
    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);

    //rs2::sensor sensor;
    //auto depth_scale = rs2::depth_sensor(sensor).get_depth_scale();
    //double clip_distance = 1;
    //clip_distance = clip_distance / depth_scale;

    // Declare pointcloud object, for calculating pointclouds and texture mappings
    rs2::pointcloud pc;
    // We want the points object to be persistent so we can display the last cloud when a frame drops
    rs2::points points;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    // Start streaming with default recommended configuration
    pipe.start();

    auto profile = pipe.get_active_profile();
    auto colorSensors = profile.get_device().query_sensors()[1];
    auto irSensor = profile.get_device().query_sensors()[0];
    std::cout << colorSensors.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
    std::cout << irSensor.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
    colorSensors.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
    colorSensors.set_option(RS2_OPTION_EXPOSURE, 150);
    colorSensors.set_option(RS2_OPTION_GAIN, 64);
    irSensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
    irSensor.set_option(RS2_OPTION_LASER_POWER, 360);

    rs2::colorizer color_map;
    rs2::align align(RS2_STREAM_COLOR);

    for (int i = 0; i < 3; i++)
    {
        rs2::frameset frames = pipe.wait_for_frames();
        cv::waitKey(10);
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

        // red[i] = new int*[boxel_cam_height_px];
        // green[i] = new int*[boxel_cam_height_px];
        // blue[i] = new int*[boxel_cam_height_px];
        // alpha[i] = new bool*[boxel_cam_height_px];
        // val_z[i] = new int*[boxel_cam_height_px];

        red[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
        green[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
        blue[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
        alpha[i] = (bool**)malloc(sizeof(bool*) * boxel_cam_height_px);
        num_pcd[i] = (int**)malloc(sizeof(int*) * boxel_cam_height_px);
        //val_z[i] = (double**)malloc(sizeof(double*) * boxel_cam_height_px);

        for (int j = 0; j < boxel_cam_height_px; j++) {

            // red[i][j] = new int[boxel_cam_width_px];
            // green[i][j] = new int[boxel_cam_width_px];
            // blue[i][j] = new int[boxel_cam_width_px];
            // alpha[i][j] = new bool[boxel_cam_width_px];
            // val_z[i][j] = new int[boxel_cam_width_px];

            red[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
            green[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
            blue[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
            alpha[i][j] = (bool*)malloc(sizeof(bool) * boxel_cam_width_px);
            num_pcd[i][j] = (int*)malloc(sizeof(int) * boxel_cam_width_px);
            //val_z[i][j] = (double*)malloc(sizeof(double) * boxel_cam_width_px);

        }
    }

    cv::Mat img_display = cv::Mat::zeros(cv::Size(display_width_px, display_height_px), CV_8UC3);

    // フレーム処理
    while (true) {

        // 測定開始時刻を記録
        auto start = std::chrono::high_resolution_clock::now();

        // Wait for the next set of frames from the camera
        auto frames = pipe.wait_for_frames();
        auto aligned_frames = align.process(frames);

        //rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
        rs2::video_frame color_frame = aligned_frames.get_color_frame();
        rs2::video_frame depth_frame = aligned_frames.get_depth_frame();

        cv::Mat color_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);

        // Generate the pointcloud and texture mappings
        points = pc.calculate(depth_frame);

        // Tell pointcloud object to map to this color frame
        pc.map_to(color_frame);

        auto verts = points.get_vertices();

        // 箱とzバッファをオフセット
        for (int i = 0; i < num_z_level; i++) {
            for (int j = 0; j < boxel_cam_height_px; j++) {
                for (int k = 0; k < boxel_cam_width_px; k++) {
                    //pcd_box[i][j][k] = static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1)) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255) + static_cast<unsigned char>(((num_z_level - 1) - i) / (num_z_level - 1) * 255 * 255);
                    red[i][j][k] = 0;
                    green[i][j][k] = 0;
                    blue[i][j][k] = 0;
                    alpha[i][j][k] = false;
                    num_pcd[i][j][k] = 0;
                }
            }
        }

        img_display = cv::Scalar::all(0);

        //for (int i = 0; i < display_px; i++) {
        //    for (int j = 0; j < display_px; j++) {

        //        img_display.at<cv::Vec3b>(i, j)[0] = 0;
        //        img_display.at<cv::Vec3b>(i, j)[1] = 0;
        //        img_display.at<cv::Vec3b>(i, j)[2] = 0;

        //    }
        //}

        double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
        double tmp_xt, tmp_yt, tmp_zt;
        int tmp_nx, tmp_ny, tmp_nz;
        double dz;

        // 補間処理を行わない場合
        if (!interpolation) {

            //cout << "no interpolation" << endl;
            for (int k = 0; k < rows; k++) {

                tmp_pcd_x = verts[k].x * 1000.0;
                tmp_pcd_y = verts[k].y * 1000.0;
                tmp_pcd_z = verts[k].z * 1000.0 + D - boxel_cam_focal_length;
                tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
                tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
                tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];

                tmp_zt = 1.0 / tmp_pcd_z;
                tmp_xt = tmp_pcd_x * tmp_zt;
                tmp_yt = tmp_pcd_y * tmp_zt;

                tmp_nx = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * tmp_xt + 0.5) + boxel_cam_width_px * 0.5);
                tmp_ny = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * tmp_yt + 0.5) + boxel_cam_height_px * 0.5);
                tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
                dz = coef / tmp_nz - tmp_pcd_z;

                //cout << "dx:" << dx << ", dy:" << dy << endl;
                //if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;

                if (0 <= tmp_nz && tmp_nz < num_z_level) {

                    if (0 <= tmp_nx && tmp_nx < boxel_cam_width_px && 0 <= tmp_ny && tmp_ny < boxel_cam_height_px) {
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
            //cout << "interpolation" << endl;
            //点群を箱に格納
            if ((int)ptimes % 2 == 1) {
                for (int k = 0; k < rows; k++) {

                    tmp_pcd_x = verts[k].x * 1000.0;
                    tmp_pcd_y = verts[k].y * 1000.0;
                    tmp_pcd_z = verts[k].z * 1000.0 + D - boxel_cam_focal_length;
                    tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
                    tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
                    tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];

                    tmp_zt = 1.0 / tmp_pcd_z;
                    tmp_xt = tmp_pcd_x * tmp_zt;
                    tmp_yt = tmp_pcd_y * tmp_zt;

                    tmp_nx = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * tmp_xt + 0.5) + boxel_cam_width_px * 0.5);
                    tmp_ny = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * tmp_yt + 0.5) + boxel_cam_height_px * 0.5);
                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
                    dz = coef / tmp_nz - tmp_pcd_z;

                    //cout << "dx:" << dx << ", dy:" << dy << endl;
                    //if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;


                    if (0 <= tmp_nz && tmp_nz < num_z_level) {

                        for (int m = -half_box_size; m <= half_box_size; m++) {
                            for (int n = -half_box_size; n <= half_box_size; n++) {
                                if (0 <= tmp_nx + n && tmp_nx + n < boxel_cam_width_px && 0 <= tmp_ny + m && tmp_ny + m < boxel_cam_height_px) {
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

                    tmp_pcd_x = verts[k].x * 1000.0;
                    tmp_pcd_y = verts[k].y * 1000.0;
                    tmp_pcd_z = verts[k].z * 1000.0 + D - boxel_cam_focal_length;
                    tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
                    tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
                    tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];

                    tmp_zt = 1.0 / tmp_pcd_z;
                    tmp_xt = tmp_pcd_x * tmp_zt;
                    tmp_yt = tmp_pcd_y * tmp_zt;

                    tmp_nx = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * tmp_xt + 0.5) + boxel_cam_width_px * 0.5);
                    tmp_ny = static_cast<int>(floor((boxel_cam_focal_length / boxel_cam_px_pitch) * tmp_yt + 0.5) + boxel_cam_height_px * 0.5);
                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
                    dz = coef / tmp_nz - tmp_pcd_z;

                    //cout << "dx:" << dx << ", dy:" << dy << endl;
                    //if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;

                    dx = tmp_pcd_x - (tmp_nx - 0.5 - boxel_cam_width_px * 0.5) * boxel_cam_px_pitch / boxel_cam_focal_length * tmp_zt;
                    dy = tmp_pcd_y - (tmp_ny - 0.5 - boxel_cam_width_px * 0.5) * boxel_cam_px_pitch / boxel_cam_focal_length * tmp_zt;

                    half_box_size_min_x = static_cast<int>(round(dx / abs(dx) * 0.5 - half_box_size * 0.5));
                    half_box_size_max_x = static_cast<int>(round(dx / abs(dx) * 0.5 + half_box_size * 0.5));
                    half_box_size_min_y = static_cast<int>(round(dy / abs(dy) * 0.5 - half_box_size * 0.5));
                    half_box_size_max_y = static_cast<int>(round(dy / abs(dy) * 0.5 + half_box_size * 0.5));

                    //cout << "min x:" << half_box_size_min_x << ", max x:" << half_box_size_max_x << ", min y:" << half_box_size_min_y << ", max y:" << half_box_size_max_y << endl;

                    if (0 <= tmp_nz && tmp_nz < num_z_level) {
                        for (int m = half_box_size_min_y; m <= half_box_size_max_y; m++) {
                            for (int n = half_box_size_min_x; n <= half_box_size_max_x; n++) {
                                if (0 <= tmp_nx + n && tmp_nx + n < boxel_cam_width_px && 0 <= tmp_ny + m && tmp_ny + m < boxel_cam_height_px) {
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
            for (int j = 0; j < boxel_cam_height_px; j++) {
                for (int k = 0; k < boxel_cam_width_px; k++) {
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
        int rowsPerThread = element_image_px / numThreads;

        int startRow, endRow;
        for (int i = 0; i < numThreads; i++) {
            startRow = i * rowsPerThread;
            endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
            threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, num_pinhole, intv, num_z_level, boxel_cam_width_px, boxel_cam_height_px, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(startu), std::ref(startv));
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

        // 測定終了時刻を記録
        auto end = std::chrono::high_resolution_clock::now();

        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        //cout << "実行時間: " << duration.count() << " ms" << std::endl;

        cv::imshow("images", img_display);

        if (cv::waitKey(10) == 27)  // ESCキーで終了
        {
            cv::destroyAllWindows;
            break;
        }

    }
    pipe.stop();

    //// 表示画像の保存
    //ostringstream stream;
    //if (interpolation) stream << "./images/lenna/prop-reconstruction/v2-3/prop-lenna-v2-3_ImgDisplay_NumPinhole" << num_pinhole << "_NxNy" << boxel_cam_width_px << "_Nz" << num_z_level << "_pitchTimes" << static_cast<int>(ptimes) << "_subjectZ" << (int)subject_z << "interpolation=T.png";
    //else stream << "./images/lenna/prop-reconstruction/v2-3/prop-lenna-v2-3_ImgDisplay_NumPinhole" << num_pinhole << "_NxNy" << boxel_cam_width_px << "_Nz" << num_z_level << "_pitchTimes" << static_cast<int>(ptimes) << "_subjectZ" << (int)subject_z << "interpolation=F.png";
    //cv::String filename = stream.str();
    //imwrite(filename, img_display);

    //stream.str("");
    //stream.clear(ostringstream::goodbit);

    //// 各イメージプレーンの画像を保存（テスト用）
    //// ostringstream stream;
    //// cv::String filename;
    //cv::Mat img = cv::Mat::zeros(cv::Size(boxel_cam_width_px, boxel_cam_height_px), CV_8UC3);
    //for (int zi = 0; zi < num_z_level; zi++) {

    //    stream << "V:/images/lenna/image-plane/v2-3/prop-lenna-v2-3_imagePlane_gridSize" << nph << "_NumZLevel" << num_z_level << "_subjectZ" << (int)subject_z << "_zi" << zi << "_Ddash" << Ddash << "_pitchTimes" << static_cast<int>(ptimes) << ".png";
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

    // 使用したメモリを解放
    for (int i = 0; i < num_z_level; i++) {
        for (int j = 0; j < boxel_cam_height_px; j++) {
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

    return EXIT_SUCCESS;
}
catch (const rs2::error& e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

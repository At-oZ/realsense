///* 点群データをcsvファイルから取得して表示画像を構成するプログラム */
///* lenna画像を用いたプログラム */
///* v1-10と同じアルゴリズム */
//
//#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
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
//
//int main(int argc, char* argv[])
//{
//
//    std::cout << "ICIP-prop-trial" << endl;
//
//    std::vector<double> result(45);
//
//    std::string WINNAME = "image";
//    cv::namedWindow(WINNAME);
//    HWND window = FindWindow(NULL, L"image");
//    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
//    SetWindowPos(window, NULL, 2560, 0, 3840, 2420, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
//
//
//    int index = 0;
//    bool interpolation = true; // 補間処理を行うかのフラッグ
//    int nph = 40;
//    int nzl = 60;
//    int pt = 3;
//    int NxNy = 300;
//
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//    double inv_WIDTH = 1.0 / (double)WIDTH;
//    double D = 150.0;
//
//    // 観察者のパラメータ
//    double zo_min = 1000.0;
//
//    // ディスプレイのパラメータ(mm)
//    double display_size = 13.4 * 25.4; // ディスプレイサイズ
//    int display_width_px = 3840, display_height_px = 2400; // ディスプレイの縦横の解像度
//    double display_pixel_pitch = display_size / sqrtf(display_width_px * display_width_px + display_height_px * display_height_px);  // 画素ピッチ
//
//    // ピンホールアレイのパラメータ(mm)
//    double pinhole_array_size = display_pixel_pitch * display_height_px;   // 各軸方向のピンホールアレイのサイズ
//    int num_pinhole = nph;  // 各軸方向のピンホール数
//    double pinhole_pitch = pinhole_array_size / num_pinhole;    // ピンホールピッチ
//
//    std::cout << "pinhole pitch:" << pinhole_pitch << "mm" << std::endl;
//
//    // 表示系のパラメータ(mm)
//    double focal_length = 8.4; // ギャップ
//    std::cout << "focal length:" << focal_length << std::endl;
//
//    // 観察者のパラメータ
//    double observe_z = 1000.0; // 観察者と表示系の距離
//    double observe_range = (observe_z + focal_length) / focal_length * pinhole_pitch; // 視域
//
//    // 要素画像のパラメータ
//    double interval_element_image = focal_length / observe_z * observe_range; // 要素画像の物理的間隔(mm)
//    int element_image_px = static_cast<int>(floor(pinhole_pitch / display_pixel_pitch)); // 要素画像の解像度
//    int display_px = 2400; // 各軸方向の表示画像の解像度
//    double intv = pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//
//    std::cout << "element image pixel:" << element_image_px << "px" << std::endl;
//    std::cout << "interval element image:" << interval_element_image << "mm" << std::endl;
//    std::cout << "display pixel:" << display_px << "px" << std::endl;
//
//    // 点群データ配列の行数と列数
//    int rows = WIDTH * HEIGHT;
//    int cols = 6;
//
//    // パラメータ
//    int num_z_level = nzl; // イメージプレーンの層数
//    double ptimes = pt; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//    int half_box_size = static_cast<int>(floor(ptimes / 2.0)); // 探索範囲
//
//    double Ddash = 200.0;
//    double coef = (double)num_z_level * Ddash;
//    double inv_coef = 1.0 / coef;
//
//    double img_pitch = display_pixel_pitch * 100 / NxNy;
//    int px_height_img = NxNy;
//    int px_width_img = NxNy;
//    std::cout << "Nx:" << px_width_img << ", Ny:" << px_height_img << std::endl;
//
//    int TIMES = 1;
//
//    std::cout << "NumPinhole:" << num_pinhole << ", NumZLevel:" << num_z_level << ", pitchTimes:" << ptimes << endl;
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
//            xt = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * zt + u / focal_length;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                nx[i][j][nz] = static_cast<int>(floor((focal_length / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                zt -= inv_coef;
//                xt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
//            }
//
//        }
//
//        for (int j = 0; j < num_pinhole; j++) {
//
//            zt = (double)(num_z_level - 1) * inv_coef;
//            yt = (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * zt + u / focal_length;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                ny[i][j][nz] = static_cast<int>(floor((focal_length / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//                zt -= inv_coef;
//                yt -= (j - (num_pinhole - 1) * 0.5) * pinhole_pitch * inv_coef;
//            }
//
//        }
//    }
//
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
//
//    //rs2::sensor sensor;
//    //auto depth_scale = rs2::depth_sensor(sensor).get_depth_scale();
//    //double clip_distance = 1;
//    //clip_distance = clip_distance / depth_scale;
//
//    // Declare pointcloud object, for calculating pointclouds and texture mappings
//    rs2::pointcloud pc;
//    // We want the points object to be persistent so we can display the last cloud when a frame drops
//    rs2::points points;
//
//    // Declare RealSense pipeline, encapsulating the actual device and sensors
//    rs2::pipeline pipe;
//    // Start streaming with default recommended configuration
//    pipe.start();
//
//    auto profile = pipe.get_active_profile();
//    auto colorSensors = profile.get_device().query_sensors()[1];
//    auto irSensor = profile.get_device().query_sensors()[0];
//    std::cout << colorSensors.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
//    std::cout << irSensor.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
//    colorSensors.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 0);
//    colorSensors.set_option(RS2_OPTION_EXPOSURE, 150);
//    colorSensors.set_option(RS2_OPTION_GAIN, 64);
//    irSensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, 1);
//    irSensor.set_option(RS2_OPTION_LASER_POWER, 360);
//
//    rs2::colorizer color_map;
//    rs2::align align(RS2_STREAM_COLOR);
//
//    for (int i = 0; i < 3; i++)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        cv::waitKey(10);
//    }
//
//    double** data = (double**)malloc(sizeof(double*) * rows);
//    for (int i = 0; i < rows; i++) {
//        data[i] = (double*)malloc(sizeof(double) * cols);
//    }
//
//    //std::string filenamein = "./images/standard/lenna.bmp";
//    //cv::Mat image_input = cv::imread(filenamein);
//
//    //if (image_input.empty())
//    //{
//    //    std::cout << "画像を開くことができませんでした。\n";
//    //    return -1;
//    //}
//
//    int*** red;
//    int*** green;
//    int*** blue;
//    bool*** alpha;
//    int*** num_pcd;
//    //double*** val_z;
//
//    // red = new int**[num_z_level];
//    // green = new int**[num_z_level];
//    // blue = new int**[num_z_level];
//    // alpha = new bool**[num_z_level];
//    // val_z = new int**[num_z_level];
//
//    red = (int***)malloc(sizeof(int**) * num_z_level);
//    green = (int***)malloc(sizeof(int**) * num_z_level);
//    blue = (int***)malloc(sizeof(int**) * num_z_level);
//    alpha = (bool***)malloc(sizeof(bool**) * num_z_level);
//    num_pcd = (int***)malloc(sizeof(int**) * num_z_level);
//    //val_z = (double***)malloc(sizeof(double**) * num_z_level);
//
//    for (int i = 0; i < num_z_level; i++) {
//
//        // red[i] = new int*[px_height_img];
//        // green[i] = new int*[px_height_img];
//        // blue[i] = new int*[px_height_img];
//        // alpha[i] = new bool*[px_height_img];
//        // val_z[i] = new int*[px_height_img];
//
//        red[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        green[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        blue[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        alpha[i] = (bool**)malloc(sizeof(bool*) * px_height_img);
//        num_pcd[i] = (int**)malloc(sizeof(int*) * px_height_img);
//        //val_z[i] = (double**)malloc(sizeof(double*) * px_height_img);
//
//        for (int j = 0; j < px_height_img; j++) {
//
//            // red[i][j] = new int[px_width_img];
//            // green[i][j] = new int[px_width_img];
//            // blue[i][j] = new int[px_width_img];
//            // alpha[i][j] = new bool[px_width_img];
//            // val_z[i][j] = new int[px_width_img];
//
//            red[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            green[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            blue[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            alpha[i][j] = (bool*)malloc(sizeof(bool) * px_width_img);
//            num_pcd[i][j] = (int*)malloc(sizeof(int) * px_width_img);
//            //val_z[i][j] = (double*)malloc(sizeof(double) * px_width_img);
//
//        }
//    }
//
//    cv::Mat img_display = cv::Mat::zeros(cv::Size(display_px, display_px), CV_8UC3);
//    cv::Mat img_window = cv::Mat::zeros(cv::Size(display_width_px, display_px), CV_8UC3);
//
//    double sum_time = 0;
//    // フレーム処理
//    while (true) {
//
//        // 測定開始時刻を記録
//        auto start = std::chrono::high_resolution_clock::now();
//
//        // Wait for the next set of frames from the camera
//        auto frames = pipe.wait_for_frames();
//        auto aligned_frames = align.process(frames);
//
//        //rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
//        rs2::video_frame color_frame = aligned_frames.get_color_frame();
//        rs2::video_frame depth_frame = aligned_frames.get_depth_frame();
//
//        cv::Mat color_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
//        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        // Generate the pointcloud and texture mappings
//        points = pc.calculate(depth_frame);
//
//        // Tell pointcloud object to map to this color frame
//        pc.map_to(color_frame);
//
//        auto verts = points.get_vertices();
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
//                    num_pcd[i][j][k] = 0;
//                }
//            }
//        }
//
//        img_window = cv::Scalar::all(255);
//        img_display = cv::Scalar::all(0);
//
//        //for (int i = 0; i < display_px; i++) {
//        //    for (int j = 0; j < display_px; j++) {
//
//        //        img_window.at<cv::Vec3b>(i, j)[0] = 0;
//        //        img_window.at<cv::Vec3b>(i, j)[1] = 0;
//        //        img_window.at<cv::Vec3b>(i, j)[2] = 0;
//
//        //        img_camera_array.at<cv::Vec3b>(i, j)[0] = 0;
//        //        img_camera_array.at<cv::Vec3b>(i, j)[1] = 0;
//        //        img_camera_array.at<cv::Vec3b>(i, j)[2] = 0;
//
//        //    }
//        //}
//
//        double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double tmp_xt, tmp_yt, tmp_zt;
//        int tmp_nx, tmp_ny, tmp_nz;
//        double dz;
//
//
//        // 補間処理を行わない場合
//        if (!interpolation) {
//
//            //cout << "no interpolation" << endl;
//            for (int k = 0; k < rows; k++) {
//
//                tmp_pcd_x = verts[k].x * 1000.0;
//                tmp_pcd_y = verts[k].y * 1000.0;
//                tmp_pcd_z = verts[k].z * 1000.0 + D;
//                tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
//                tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
//                tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];
//
//                tmp_zt = 1.0 / tmp_pcd_z;
//                tmp_xt = tmp_pcd_x * tmp_zt;
//                tmp_yt = tmp_pcd_y * tmp_zt;
//
//                tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//                dz = coef / tmp_nz - tmp_pcd_z;
//
//                //cout << "dx:" << dx << ", dy:" << dy << endl;
//                //if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;
//
//                if (0 <= tmp_nz && tmp_nz < num_z_level) {
//
//                    if (0 <= tmp_nx && tmp_nx < px_width_img && 0 <= tmp_ny && tmp_ny < px_height_img) {
//                        //cout << "nz:" << tmp_nz << endl;
//                        blue[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_b);
//                        green[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_g);
//                        red[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_r);
//                        alpha[tmp_nz][tmp_ny][tmp_nx] = true;
//                        num_pcd[tmp_nz][tmp_ny][tmp_nx]++;
//                    }
//                }
//            }
//        }
//        else { // 補間処理を行う場合
//            //cout << "interpolation" << endl;
//            //点群を箱に格納
//            if ((int)ptimes % 2 == 1) {
//                for (int k = 0; k < rows; k++) {
//
//                    tmp_pcd_x = verts[k].x * 1000.0;
//                    tmp_pcd_y = verts[k].y * 1000.0;
//                    tmp_pcd_z = verts[k].z * 1000.0 + D;
//                    tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
//                    tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
//                    tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];
//
//                    tmp_zt = 1.0 / tmp_pcd_z;
//                    tmp_xt = tmp_pcd_x * tmp_zt;
//                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//                    dz = coef / tmp_nz - tmp_pcd_z;
//
//                    //cout << "dx:" << dx << ", dy:" << dy << endl;
//                    //if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;
//
//
//                    if (0 <= tmp_nz && tmp_nz < num_z_level) {
//
//                        for (int m = -half_box_size; m <= half_box_size; m++) {
//                            for (int n = -half_box_size; n <= half_box_size; n++) {
//                                if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
//                                    //cout << "nz:" << tmp_nz << endl;
//                                    blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                                    green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                                    red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                                    alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                                    num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            else {
//
//                double dx, dy;
//                int half_box_size_min_x, half_box_size_max_x, half_box_size_min_y, half_box_size_max_y;
//
//                for (int k = 0; k < rows; k++) {
//
//                    tmp_pcd_x = verts[k].x * 1000.0;
//                    tmp_pcd_y = verts[k].y * 1000.0;
//                    tmp_pcd_z = verts[k].z * 1000.0 + D;
//                    tmp_pcd_b = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[2];
//                    tmp_pcd_g = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[1];
//                    tmp_pcd_r = color_image.at<cv::Vec3b>(static_cast<int>(k * inv_WIDTH), (k % WIDTH))[0];
//
//                    tmp_zt = 1.0 / tmp_pcd_z;
//                    tmp_xt = tmp_pcd_x * tmp_zt;
//                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//                    dz = coef / tmp_nz - tmp_pcd_z;
//
//                    //cout << "dx:" << dx << ", dy:" << dy << endl;
//                    //if (k == 0) cout << "nz:" << tmp_nz << "dz:" << dz << endl;
//
//                    dx = tmp_pcd_x - (tmp_nx - 0.5 - px_width_img * 0.5) * img_pitch / focal_length * tmp_zt;
//                    dy = tmp_pcd_y - (tmp_ny - 0.5 - px_width_img * 0.5) * img_pitch / focal_length * tmp_zt;
//
//                    half_box_size_min_x = static_cast<int>(round(dx / abs(dx) * 0.5 - half_box_size * 0.5));
//                    half_box_size_max_x = static_cast<int>(round(dx / abs(dx) * 0.5 + half_box_size * 0.5));
//                    half_box_size_min_y = static_cast<int>(round(dy / abs(dy) * 0.5 - half_box_size * 0.5));
//                    half_box_size_max_y = static_cast<int>(round(dy / abs(dy) * 0.5 + half_box_size * 0.5));
//
//                    //cout << "min x:" << half_box_size_min_x << ", max x:" << half_box_size_max_x << ", min y:" << half_box_size_min_y << ", max y:" << half_box_size_max_y << endl;
//
//                    if (0 <= tmp_nz && tmp_nz < num_z_level) {
//                        for (int m = half_box_size_min_y; m <= half_box_size_max_y; m++) {
//                            for (int n = half_box_size_min_x; n <= half_box_size_max_x; n++) {
//                                if (0 <= tmp_nx + n && tmp_nx + n < px_width_img && 0 <= tmp_ny + m && tmp_ny + m < px_height_img) {
//                                    //cout << "nz:" << tmp_nz << endl;
//                                    blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                                    green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                                    red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                                    alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                                    num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//
//        for (int i = 0; i < num_z_level; i++) {
//            for (int j = 0; j < px_height_img; j++) {
//                for (int k = 0; k < px_width_img; k++) {
//                    //cout << i << ", " << j << ", " << k << "; " << "red:" << red[i][j][k] << ", green:" << green[i][j][k] << ", blue:" << blue[i][j][k] << endl;
//                    if (alpha[i][j][k]) {
//                        red[i][j][k] = red[i][j][k] / num_pcd[i][j][k];
//                        green[i][j][k] = green[i][j][k] / num_pcd[i][j][k];
//                        blue[i][j][k] = blue[i][j][k] / num_pcd[i][j][k];
//                    }
//                }
//            }
//        }
//
//        // insert_pixels(0, element_image_px, img_window, red, green, blue, alpha, nx, ny, startu, startv);
//
//        const int numThreads = 15;
//        vector<thread> threads;
//        int rowsPerThread = element_image_px / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, num_pinhole, intv, num_z_level, px_width_img, px_height_img, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(startu), std::ref(startv));
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
//        //cout << "calc finished" << endl;
//        finished_threads = 0;
//
//        int shift_y = 10;
//        for (int i = shift_y; i < display_px; ++i) {
//            if (i - static_cast<int>(display_px * 0.5) > 0) {
//                for (int j = static_cast<int>(floor((display_width_px - display_px) / 2)); j < display_width_px - static_cast<int>(floor((display_width_px - display_px) / 2)); ++j) {
//                    img_window.at<cv::Vec3b>(i - static_cast<int>(display_px * 0.5), j)[0] = img_display.at<cv::Vec3b>(i - shift_y, j - static_cast<int>(floor((display_width_px - display_px) / 2)))[0];
//                    img_window.at<cv::Vec3b>(i - static_cast<int>(display_px * 0.5), j)[1] = img_display.at<cv::Vec3b>(i - shift_y, j - static_cast<int>(floor((display_width_px - display_px) / 2)))[1];
//                    img_window.at<cv::Vec3b>(i - static_cast<int>(display_px * 0.5), j)[2] = img_display.at<cv::Vec3b>(i - shift_y, j - static_cast<int>(floor((display_width_px - display_px) / 2)))[2];
//                }
//            }
//        }
//
//        // 測定終了時刻を記録
//        auto end = std::chrono::high_resolution_clock::now();
//
//        // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//        cout << "実行時間: " << duration.count() << " ms" << std::endl;
//
//        cv::imshow(WINNAME, img_window);
//
//        if (cv::waitKey(10) == 27)  // ESCキーで終了
//        {
//            cv::destroyAllWindows;
//            break;
//        }
//
//    }
//    pipe.stop();
//
//    // 表示画像の保存
//    //ostringstream stream;
//    //stream << "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ICIP-prop-improve-v1-2/prop-improve-v1-2-pepper_tileExpand_Nx" << px_height_img << "_Ny" << px_width_img << "_Nz" << nzl << "_N" << ptimes << "_zi" << (int)subz << ".png";
//    //cv::String filename = stream.str();
//    //imwrite(filename, img_display);
//
//    //stream.str("");
//    //stream.clear(ostringstream::goodbit);
//
//    // 各イメージプレーンの画像を保存（テスト用）
//    //ostringstream stream;
//    //cv::String filename;
//    //   cv::Mat img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//    //for (int zi = 24; zi < 25; zi++) {
//
//    //    stream << "E:/EvacuatedStorage/image-plane/prop-improve/prop-improve-v1-detail-pepper_tileExpand_Nz" << nzl << "_N" << ptimes << "_subjectZ" << (int)subz << "_zi" << zi << ".png";
//    //    cout << "zi:" << zi << endl;
//
//    //    for (int i = 0; i < px_height_img; i++) {
//    //        for (int j = 0; j < px_width_img; j++) {
//    //            if (alpha[zi][i][j] > 0) {
//    //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
//    //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
//    //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
//    //            }
//    //        }
//    //    }
//    //    filename = stream.str();
//    //    imwrite(filename, img);
//    //    stream.str("");
//    //    stream.clear(ostringstream::goodbit);
//    //    img = cv::Mat::zeros(cv::Size(px_width_img, px_height_img), CV_8UC3);
//
//    //}
//
//
//    //cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//    //result[index++] = sum_time / TIMES;
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
//            free(num_pcd[i][j]);
//        }
//        free(red[i]);
//        free(green[i]);
//        free(blue[i]);
//        free(alpha[i]);
//        free(num_pcd[i]);
//    }
//    free(red);
//    free(green);
//    free(blue);
//    free(alpha);
//    free(num_pcd);
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
//
//    //writeCSV1(result);
//
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

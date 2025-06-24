///* 点群データから4次元光線情報の再構成画像を作成するプログラム */
///* ICIP-prop-improve-v1-3からの派生 */
///* RGB-Dカメラを使用 */
//
// #include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
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
//#include <Xinput.h>
//
//// XInputのヘッダとライブラリをリンク
//#pragma comment(lib, "XInput.lib")
//
//using namespace std;
//using namespace cv;
//
//std::mutex mtx;
//std::condition_variable conv;
//int finished_threads = 0; // 終了したスレッドの数
//
//void insert_pixels(int start, int end, int element_image_px, int num_pinhole, double intv, int num_z_level, int px_width_img, int px_height_img, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int** u_px, int** v_px);
//int writeCSV2(const std::vector<std::vector<double>> array);
//void shiftImage(cv::Mat& image, int dx, int dy);
//bool ProcessXInput(int& x_shift, int& y_shift, int shift_amount, bool& continue_key_loop, bool& enter_pressed);
//
//int main(int argc, char* argv[])
//{
//
//    cout << "IE-prop-wideview-v1-3" << endl;
//
//    std::string WINNAME = "image";
//    cv::namedWindow(WINNAME);
//    HWND window = FindWindow(NULL, L"image");
//    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
//    SetWindowPos(window, NULL, 2560, 0, 3840, 2420, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
//
//    cv::Mat img_window = cv::Mat::zeros(cv::Size(3840, 2420), CV_8UC3);
//    img_window = cv::Scalar::all(255);
//
//    // 画像シフト用の変数
//    cv::Mat original_img_window;
//    int x_shift = 0, y_shift = 0;
//
//    // シフト量設定用の変数
//    int shift_amount = 1;
//    bool input_mode = false;
//    std::string input_buffer = "";
//
//    // ディスプレイの設定
//    const int displayImageSize = 2400;
//
//    // コントローラーのポーリング間隔 (ms)
//    const int controllerPollInterval = 50;
//    ULONGLONG lastPollTime = 0;
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
//    // ピンホールアレイと表示系のパラメータ(mm)
//    double pinhole_array_width = 181.6;
//    double focal_length = 7.9667; // ギャップ(zo_min / (3 * nph - 1))
//    double pinhole_pitch = pinhole_array_width / nph;   // ピンホールピッチ
//    int num_pinhole = nph;  // 各軸方向のピンホール数
//    double pinhole_array_size = pinhole_pitch * num_pinhole;   // 各軸方向のピンホールアレイのサイズ
//    double intv = (focal_length + zo_min) / zo_min * pinhole_pitch / display_pixel_pitch; // 要素画像の間隔
//    int element_image_px = static_cast<int>(floor(intv)); // 要素画像の解像度
//    double display_area_size = (focal_length + zo_min) / zo_min * pinhole_pitch * nph; //表示画像の大きさ
//    int display_px = static_cast<int>(floor(intv * nph)); // 表示画像の解像度
//
//
//    cout << "display area size:" << display_area_size << fixed << setprecision(3) << ", intv:" << intv << fixed << setprecision(3) << ", element image px:" << element_image_px << fixed << setprecision(3) << ", display px:" << display_px << fixed << setprecision(5) << ", display pixel pitch:" << display_pixel_pitch << fixed << setprecision(5) << endl;
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
//    double img_pitch = display_pixel_pitch * 100 / NxNy; // 詳細化の場合 /ptimes をかける
//
//    int px_height_img = NxNy; // 詳細化の場合 static_cast<int>(100 * ptimes) に変更
//    int px_width_img = NxNy; // 詳細化の場合 static_cast<int>(100 * ptimes) に変更
//    std::cout << "Nx:" << px_width_img << ", Ny:" << px_height_img << std::endl;
//
//    // 各要素画像の原点画素位置(左上)
//    int** u_px = (int**)malloc(sizeof(int*) * element_image_px);
//    int** v_px = (int**)malloc(sizeof(int*) * element_image_px);
//    for (int i = 0; i < element_image_px; i++) {
//        u_px[i] = (int*)malloc(sizeof(int) * num_pinhole);
//        v_px[i] = (int*)malloc(sizeof(int) * num_pinhole);
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
//    double s, t, u, v, xt, yt, zt, nz;
//    double offset_st = -((num_pinhole - 1) * pinhole_pitch) * 0.5;
//    double offset_uv = -((element_image_px - 1) * display_pixel_pitch) * 0.5;
//    for (int i = 0; i < element_image_px; i++) {
//
//        for (int j = 0; j < num_pinhole; j++) {
//
//            s = offset_st + j * pinhole_pitch;
//            u = offset_uv + i * display_pixel_pitch + focal_length / zo_min * s;
//            u_px[i][j] = static_cast<int>(floor((s + u + display_area_size * 0.5) / display_pixel_pitch));
//            zt = (double)(num_z_level - 1) * inv_coef;
//            xt = s * zt + u / focal_length;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                nx[i][j][nz] = static_cast<int>(floor((focal_length / img_pitch) * xt + 0.5) + px_width_img * 0.5);
//                zt -= inv_coef;
//                xt -= s * inv_coef;
//            }
//        }
//
//        for (int j = 0; j < num_pinhole; j++) {
//
//            t = offset_st + j * pinhole_pitch;
//            v = offset_uv + i * display_pixel_pitch + focal_length / zo_min * t;
//            v_px[i][j] = static_cast<int>(floor((t + v + display_area_size * 0.5) / display_pixel_pitch));
//            zt = (double)(num_z_level - 1) * inv_coef;
//            yt = t * zt + v / focal_length;
//
//            for (int nz = num_z_level - 1; nz >= 0; nz--) {
//                ny[i][j][nz] = static_cast<int>(floor((focal_length / img_pitch) * yt + 0.5) + px_height_img * 0.5);
//                zt -= inv_coef;
//                yt -= t * inv_coef;
//            }
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
//    for (int i = 0; i < 10; i++)
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
//
//    /* 画像位置設定 */
//
//    while (true) {
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
//        img_display = cv::Scalar::all(0);
//
//        double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double tmp_xt, tmp_yt, tmp_zt;
//        int tmp_nx, tmp_ny, tmp_nz;
//
//        // 補間しない場合
//        if (!interpolation) {
//            for (int k = 0; k < rows; k++) {
//
//                tmp_pcd_x = data[k][0];
//                tmp_pcd_y = data[k][1];
//                tmp_pcd_z = data[k][2];
//                tmp_pcd_b = data[k][3];
//                tmp_pcd_g = data[k][4];
//                tmp_pcd_r = data[k][5];
//
//                tmp_zt = 1.0 / tmp_pcd_z;
//                tmp_xt = tmp_pcd_x * tmp_zt;
//                tmp_yt = tmp_pcd_y * tmp_zt;
//
//                tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                //cout << "dx:" << dx << ", dy:" << dy << endl;
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
//
//        }
//        // 補間する場合
//        else {
//
//            //点群を箱に格納
//            if ((int)ptimes % 2 == 1) {
//                for (int k = 0; k < rows; k++) {
//
//                    tmp_pcd_x = data[k][0];
//                    tmp_pcd_y = data[k][1];
//                    tmp_pcd_z = data[k][2];
//                    tmp_pcd_b = data[k][3];
//                    tmp_pcd_g = data[k][4];
//                    tmp_pcd_r = data[k][5];
//
//                    tmp_zt = 1.0 / tmp_pcd_z;
//                    tmp_xt = tmp_pcd_x * tmp_zt;
//                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                    //cout << "dx:" << dx << ", dy:" << dy << endl;
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
//                    tmp_pcd_x = data[k][0];
//                    tmp_pcd_y = data[k][1];
//                    tmp_pcd_z = data[k][2];
//                    tmp_pcd_b = data[k][3];
//                    tmp_pcd_g = data[k][4];
//                    tmp_pcd_r = data[k][5];
//
//                    tmp_zt = 1.0 / tmp_pcd_z;
//                    tmp_xt = tmp_pcd_x * tmp_zt;
//                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
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
//        // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, nx, ny, startu, startv);
//
//        const int numThreads = 15;
//        vector<thread> threads;
//        int rowsPerThread = element_image_px / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, num_pinhole, intv, num_z_level, px_width_img, px_height_img, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(u_px), std::ref(v_px));
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
//        cv::imshow("image", img_display);
//
//        // キーボード入力をチェック (非ブロッキング)
//        int key = cv::waitKey(1);
//
//        // キーが押されていない場合
//        if (key == 27) {
//            break;
//        }
//
//
//    }
//
//
//    // 中央に配置するためのオフセットを計算
//    int offsetX = (displayImageSize > display_px) ? (displayImageSize - display_px) / 2 : 0;
//    int offsetY = (displayImageSize > display_px) ? (displayImageSize - display_px) / 2 : 0;
//
//    // メイン表示ウィンドウに描画
//    img_window = cv::Scalar::all(255); // 白い背景
//
//    // 描画領域の計算
//    int copy_width = (std::max)(display_px, displayImageSize);
//    int copy_height = (std::max)(display_px, displayImageSize);
//
//    for (int i = 0; i < copy_height; i++) {
//        for (int j = 0; j < copy_width; j++) {
//            int target_i = i + offsetY;
//            int target_j = j + 720 + offsetX; // 横方向にセンタリング
//
//            if (target_i >= 0 && target_i < img_window.rows &&
//                target_j >= 0 && target_j < img_window.cols) {
//                img_window.at<cv::Vec3b>(target_i, target_j) = img_display.at<cv::Vec3b>(i, j);
//            }
//        }
//    }
//
//    bool continue_key_loop = true;
//    std::string last_input_buffer = "";
//
//    // オリジナルの画像を保存（手動シフト適用前）
//    original_img_window = img_window.clone();
//    cv::Mat base_image = img_window.clone(); // 基本画像の保存
//
//    // モード選択処理とキー入力ループ
//    while (continue_key_loop) {
//        cv::Mat display_img;
//        bool imageUpdated = false;
//        bool enterPressed = false;
//
//        // 手動シフトモード - 全体画像をシフト
//        display_img = original_img_window.clone();
//        if (x_shift != 0 || y_shift != 0) {
//            shiftImage(display_img, x_shift, y_shift);
//        }
//
//        // 入力モード中の表示
//        if (input_mode && last_input_buffer != input_buffer) {
//            // シフト量入力モードの表示
//            std::cout << "シフト量入力モード: " << input_buffer << "_" << std::endl;
//            std::string display_text = "シフト量: " + input_buffer + "_";
//            cv::putText(display_img, display_text, cv::Point(50, 100),
//                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
//
//            last_input_buffer = input_buffer;
//        }
//
//        // 画像を表示
//        cv::imshow(WINNAME, display_img);
//
//        // 入力モード中はコントローラー入力を無視
//        if (!input_mode) {
//            // 現在の時間を取得
//            ULONGLONG currentTime = GetTickCount64();
//
//            // そして、メインループの中で、以下のように修正:
//            // 一定間隔でコントローラー入力をチェック
//            if (currentTime - lastPollTime > controllerPollInterval) {
//                // コントローラー入力の処理
//                bool controller_changed = false;
//
//                // 適切な変数への参照
//                int* targetX = &x_shift;
//                int* targetY = &y_shift;
//
//                // enterPressedをfalseに初期化してから渡す
//                enterPressed = false;
//
//                controller_changed = ProcessXInput(*targetX, *targetY, shift_amount,
//                    continue_key_loop, enterPressed);
//
//                //std::cout << enterPressed << std::endl;
//
//                // 入力が変更された場合、または特別なボタンが押された場合
//                if (controller_changed || enterPressed) {
//                    imageUpdated = true;
//                }
//
//                lastPollTime = currentTime;
//            }
//        }
//
//        // キーボード入力をチェック (非ブロッキング)
//        int key = cv::waitKey(1);
//
//        // キーが押されていない場合
//        if (key == -1 && !enterPressed) {
//            // 短い待機時間を挿入してCPU使用率を抑制
//            Sleep(10);
//            continue;
//        }
//
//        // 入力モード中の処理
//        if (input_mode) {
//            if (key >= '0' && key <= '9' || key == '.') {
//                // 数字キーまたはドットが押されたら入力バッファに追加
//                input_buffer += (char)key;
//                imageUpdated = true;
//            }
//            else if (key == 8) { // バックスペース
//                // バッファから1文字削除
//                if (!input_buffer.empty()) {
//                    input_buffer.pop_back();
//                    imageUpdated = true;
//                }
//            }
//            else if (key == 13) { // Enter
//                // 入力確定、シフト量を設定
//                if (!input_buffer.empty()) {
//                    shift_amount = std::stoi(input_buffer);
//                    std::cout << "シフト量を " << shift_amount << " に設定しました。" << std::endl;
//                }
//                input_mode = false;
//                input_buffer = "";
//                imageUpdated = true;
//            }
//            else if (key == 27) { // ESC
//                // 入力モードをキャンセル
//                input_mode = false;
//                input_buffer = "";
//                imageUpdated = true;
//            }
//            continue;
//        }
//
//        // Enterキーの処理（モード切替）
//        if (key == 13 || enterPressed) {
//            enterPressed = true;
//        }
//
//        switch (key) {
//        case 'w': // 上へ移動
//        case 'W':
//            y_shift -= shift_amount;
//            std::cout << "上に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//            imageUpdated = true;
//            break;
//
//        case 's': // 下へ移動
//        case 'S':
//            y_shift += shift_amount;
//            std::cout << "下に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//            imageUpdated = true;
//            break;
//
//        case 'a': // 左へ移動
//        case 'A':
//            x_shift -= shift_amount;
//            std::cout << "左に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//            imageUpdated = true;
//            break;
//
//        case 'd': // 右へ移動
//        case 'D':
//            x_shift += shift_amount;
//            std::cout << "右に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//            imageUpdated = true;
//            break;
//
//        case 'r': // リセット
//        case 'R':
//            x_shift = 0;
//            y_shift = 0;
//            std::cout << "位置をリセットしました。" << std::endl;
//            imageUpdated = true;
//            break;
//
//        case 'i': // シフト量設定モード
//        case 'I':
//            input_mode = true;
//            input_buffer = "";
//            std::cout << "シフト量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
//            imageUpdated = true;
//            break;
//
//        case 27: // ESC: 終了
//            continue_key_loop = false;
//            break;
//        }
//
//        // Enterキーによるモード切替処理
//        if (enterPressed) {
//            continue_key_loop = false;
//            break;
//        }
//    }
//
//    /* フレーム処理 */
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
//        img_display = cv::Scalar::all(0);
//
//        double tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//        double tmp_xt, tmp_yt, tmp_zt;
//        int tmp_nx, tmp_ny, tmp_nz;
//
//        // 補間しない場合
//        if (!interpolation) {
//            for (int k = 0; k < rows; k++) {
//
//                tmp_pcd_x = data[k][0];
//                tmp_pcd_y = data[k][1];
//                tmp_pcd_z = data[k][2];
//                tmp_pcd_b = data[k][3];
//                tmp_pcd_g = data[k][4];
//                tmp_pcd_r = data[k][5];
//
//                tmp_zt = 1.0 / tmp_pcd_z;
//                tmp_xt = tmp_pcd_x * tmp_zt;
//                tmp_yt = tmp_pcd_y * tmp_zt;
//
//                tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                //cout << "dx:" << dx << ", dy:" << dy << endl;
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
//
//        }
//        // 補間する場合
//        else {
//
//            //点群を箱に格納
//            if ((int)ptimes % 2 == 1) {
//                for (int k = 0; k < rows; k++) {
//
//                    tmp_pcd_x = data[k][0];
//                    tmp_pcd_y = data[k][1];
//                    tmp_pcd_z = data[k][2];
//                    tmp_pcd_b = data[k][3];
//                    tmp_pcd_g = data[k][4];
//                    tmp_pcd_r = data[k][5];
//
//                    tmp_zt = 1.0 / tmp_pcd_z;
//                    tmp_xt = tmp_pcd_x * tmp_zt;
//                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
//
//                    //cout << "dx:" << dx << ", dy:" << dy << endl;
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
//                    tmp_pcd_x = data[k][0];
//                    tmp_pcd_y = data[k][1];
//                    tmp_pcd_z = data[k][2];
//                    tmp_pcd_b = data[k][3];
//                    tmp_pcd_g = data[k][4];
//                    tmp_pcd_r = data[k][5];
//
//                    tmp_zt = 1.0 / tmp_pcd_z;
//                    tmp_xt = tmp_pcd_x * tmp_zt;
//                    tmp_yt = tmp_pcd_y * tmp_zt;
//
//                    tmp_nx = static_cast<int>(floor((focal_length / img_pitch) * tmp_xt + 0.5) + px_width_img * 0.5);
//                    tmp_ny = static_cast<int>(floor((focal_length / img_pitch) * tmp_yt + 0.5) + px_height_img * 0.5);
//                    tmp_nz = static_cast<int>(floor(coef * tmp_zt + 0.5));
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
//        // insert_pixels(0, element_image_px, img_display, red, green, blue, alpha, nx, ny, startu, startv);
//
//        const int numThreads = 15;
//        vector<thread> threads;
//        int rowsPerThread = element_image_px / numThreads;
//
//        int startRow, endRow;
//        for (int i = 0; i < numThreads; i++) {
//            startRow = i * rowsPerThread;
//            endRow = (i == numThreads - 1) ? element_image_px : (i + 1) * rowsPerThread;
//            threads.emplace_back(insert_pixels, startRow, endRow, element_image_px, num_pinhole, intv, num_z_level, px_width_img, px_height_img, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(u_px), std::ref(v_px));
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
//        int shift_y = 0;
//        for (int i = shift_y; i < display_px; ++i) {
//            if (i - static_cast<int>(display_px * 0.5) > 0) {
//                for (int j = static_cast<int>(floor((display_width_px - display_px) / 2)); j < display_width_px - static_cast<int>(floor((display_width_px - display_px) / 2)); ++j) {
//                    img_window.at<cv::Vec3b>(i, j)[0] = img_display.at<cv::Vec3b>(i - shift_y, j - static_cast<int>(floor((display_width_px - display_px) / 2)))[0];
//                    img_window.at<cv::Vec3b>(i, j)[1] = img_display.at<cv::Vec3b>(i - shift_y, j - static_cast<int>(floor((display_width_px - display_px) / 2)))[1];
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
//        free(u_px[i]);
//        free(v_px[i]);
//    }
//    free(nx);
//    free(ny);
//    free(u_px);
//    free(v_px);
//
//    MessageBeep(-1);
//    return EXIT_SUCCESS;
//}
//
//void insert_pixels(int start, int end, int element_image_px, int num_pinhole, double intv, int num_z_level, int px_width_img, int px_height_img, cv::Mat img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int** u_px, int** v_px) {
//
//    int tmp_nx, tmp_ny;
//
//    // 各要素画像の各画素ごとに
//    for (int m = start; m < end; m++) {
//
//        for (int n = 0; n < element_image_px; n++) {
//
//            // 各要素カメラごとに
//            for (int i = 0; i < num_pinhole; i++) {
//
//                for (int j = 0; j < num_pinhole; j++) {
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
//                                img_display.at<cv::Vec3b>(v_px[m][i], u_px[n][j])[2] = static_cast<int>(red[nz][tmp_ny][tmp_nx]);
//                                img_display.at<cv::Vec3b>(v_px[m][i], u_px[n][j])[1] = static_cast<int>(green[nz][tmp_ny][tmp_nx]);
//                                img_display.at<cv::Vec3b>(v_px[m][i], u_px[n][j])[0] = static_cast<int>(blue[nz][tmp_ny][tmp_nx]);
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
//int writeCSV2(const std::vector<std::vector<double>> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/speed/speed-IE-prop-wideview-v1-pepper.csv");
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
//
//// コントローラー入力のチェック関数（拡張版）
//bool ProcessXInput(int& x_shift, int& y_shift, int shift_amount, bool& continue_key_loop, bool& enter_pressed) {
//    // コントローラーの状態を取得
//    XINPUT_STATE state;
//    ZeroMemory(&state, sizeof(XINPUT_STATE));
//
//    // コントローラー0の状態を取得（最初に接続されたコントローラー）
//    DWORD result = XInputGetState(0, &state);
//
//    // コントローラーが接続されていない場合
//    if (result != ERROR_SUCCESS) {
//        return false;
//    }
//
//    // 十字キーの状態を取得
//    bool changed = false;
//    // enterPressedは関数の外から渡されるため、ここでfalseにリセットしないこと
//    // enter_pressed = false;
//    // 
//    // DPADの状態を確認
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
//        // 上キー - 上移動
//        y_shift -= shift_amount;
//        std::cout << "上に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//        changed = true;
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
//        // 下キー - 下移動
//        y_shift += shift_amount;
//        std::cout << "下に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//        changed = true;
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
//        // 左キー - 左移動
//        x_shift -= shift_amount;
//        std::cout << "左に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//        changed = true;
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
//        // 右キー - 右移動
//        x_shift += shift_amount;
//        std::cout << "右に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//        changed = true;
//    }
//
//    // Aボタン - 確定ボタン（Enterキーと同じ）
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
//        static ULONGLONG lastAButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastAButtonPressTime > 300) {
//            enter_pressed = true; // このフラグを明示的にtrueに設定
//            std::cout << "Enterキーが押されました（Aボタン）。" << std::endl;
//            lastAButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    // Bボタン - リセット（Rキーと同じ）
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
//        static ULONGLONG lastBButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastBButtonPressTime > 300) {
//            x_shift = 0;
//            y_shift = 0;
//            std::cout << "位置をリセットしました。" << std::endl;
//            lastBButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    // バックボタン（またはビューボタン） - 終了（ESCと同じ）
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
//        static ULONGLONG lastBackButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastBackButtonPressTime > 300) {
//            continue_key_loop = false;
//            lastBackButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    return changed;
//}
//
//// 画像シフト用関数
//void shiftImage(cv::Mat& image, int dx, int dy) {
//    cv::Mat result = cv::Mat::zeros(image.size(), image.type());
//    result.setTo(cv::Scalar::all(255)); // 背景を白に設定
//
//    // シフト後の有効範囲を計算
//    int startX = (std::max)(0, dx);
//    int startY = (std::max)(0, dy);
//    int endX = (std::min)(image.cols, image.cols + dx);
//    int endY = (std::min)(image.rows, image.rows + dy);
//    int width = endX - startX;
//    int height = endY - startY;
//
//    if (width <= 0 || height <= 0) return; // 有効範囲がない場合は何もしない
//
//    // 元画像の対応する部分をコピー
//    cv::Rect srcRect(startX - dx, startY - dy, width, height);
//    cv::Rect dstRect(startX, startY, width, height);
//
//    image(srcRect).copyTo(result(dstRect));
//    result.copyTo(image);
//}

///* 点群データから4次元光線情報の再構成画像を作成するプログラム */
///* IE-prop-wideview-v1からの派生 */
///* IE-prop-wideview-v1の細かい点を修正 */
//
//// #include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
//#include <opencv2/opencv.hpp>
//#include <algorithm>            // min, max
//#include <vector>
//#include <sstream>
//#include <iostream>
//#include <fstream>
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
//// 定数(観察者側)
////------------------------------
//
//const float MIN_OBSERVE_Z = 1000.0f;
//
////------------------------------
//
//
//// 定数(表示系側)
////------------------------------
//
//// ディスプレイの画素ピッチ
//const float DISPLAY_PX_PITCH = 13.4f * 25.4f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);
//
//// レンズ数
//const unsigned int NUM_LENS_X = 128;
//const unsigned int NUM_LENS_Y = 80;
//
//// レンズ数の半分
//const int HALF_NUM_LENS_X = NUM_LENS_X * 0.5f;
//const int HALF_NUM_LENS_Y = NUM_LENS_Y * 0.5f;
//
//// レンズの焦点距離
//const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * 40.0f - 1.0f);
//
///* 要素画像の解像度至上主義 */
//// 要素画像の解像度
//const unsigned int NUM_ELEM_IMG_PX_X = 60;
//const unsigned int NUM_ELEM_IMG_PX_Y = 60;
//
//// 要素画像の解像度の浮動小数点数
//const float FLOAT_NUM_ELEM_IMG_PX_X = (float)NUM_ELEM_IMG_PX_X;
//const float FLOAT_NUM_ELEM_IMG_PX_Y = (float)NUM_ELEM_IMG_PX_Y;
//
//// 要素画像の間隔
//const float ELEM_IMG_PITCH_X = NUM_ELEM_IMG_PX_X * DISPLAY_PX_PITCH;
//const float ELEM_IMG_PITCH_Y = NUM_ELEM_IMG_PX_Y * DISPLAY_PX_PITCH;
//
///* レンズピッチ */
////// 無限遠に向けた光線場再現の場合(A-1)
////const string VIEW_MODE = "normal";
////const float LENS_PITCH_X = ELEM_IMG_PITCH_X;
////const float LENS_PITCH_Y = ELEM_IMG_PITCH_Y;
//
//// 想定観察距離に向けた光線場再現の場合(A-2)
//const string VIEW_MODE = "wideview";
//const float LENS_PITCH_X = ELEM_IMG_PITCH_X * MIN_OBSERVE_Z / (FOCAL_LENGTH + MIN_OBSERVE_Z);
//const float LENS_PITCH_Y = ELEM_IMG_PITCH_Y * MIN_OBSERVE_Z / (FOCAL_LENGTH + MIN_OBSERVE_Z);
//
//// レンズピッチの半分
//const float HALF_LENS_PITCH_X = LENS_PITCH_X / 2.0f;
//const float HALF_LENS_PITCH_Y = LENS_PITCH_Y / 2.0f;
//
//// 表示画像の解像度
//const unsigned int NUM_DISPLAY_IMG_PX_X = NUM_ELEM_IMG_PX_X * NUM_LENS_X;
//const unsigned int NUM_DISPLAY_IMG_PX_Y = NUM_ELEM_IMG_PX_Y * NUM_LENS_Y;
//
//// 表示画像のサイズ
//const float DISPLAY_IMG_SIZE_X = NUM_DISPLAY_IMG_PX_X * DISPLAY_PX_PITCH;
//const float DISPLAY_IMG_SIZE_Y = NUM_DISPLAY_IMG_PX_Y * DISPLAY_PX_PITCH;
//
//// 仮想カメラ設定
//const float TAN_YALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
//const float FOV_Y = atan(TAN_YALF_Y) * 2.0f;
//
////------------------------------
//
//// 定数(被写体側)
////------------------------------
//
//// 被写体平面の奥行位置
//const float SUBJECT_Z = 1.0f;
//
//// 被写体平面の各軸点群数
//const unsigned int NUM_SUBJECT_POINTS_X = 554;
//const unsigned int NUM_SUBJECT_POINTS_Y = 554;
//
//// 被写体平面の全体点群数
//const int NUM_POINTS = NUM_SUBJECT_POINTS_X * NUM_SUBJECT_POINTS_Y;
//
//// 被写体平面の各軸点群数の半分
//const int HALF_NUM_SUBJECT_POINTS_X = NUM_SUBJECT_POINTS_X * 0.5f;
//const int HALF_NUM_SUBJECT_POINTS_Y = NUM_SUBJECT_POINTS_Y * 0.5f;
//
//// 被写体平面のサイズ
//const float SUBJECT_SIZE_X = DISPLAY_IMG_SIZE_X * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//const float SUBJECT_SIZE_Y = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の垂直方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//
//// 被写体平面の点群間隔
//const float SUBJECT_POINTS_PITCH_X = SUBJECT_SIZE_X / static_cast<float>(NUM_SUBJECT_POINTS_X);
//const float SUBJECT_POINTS_PITCH_Y = SUBJECT_SIZE_Y / static_cast<float>(NUM_SUBJECT_POINTS_Y);
//
//// 被写体平面の点群間隔の半分
//const float HALF_SUBJECT_POINTS_PITCH_X = SUBJECT_POINTS_PITCH_X * 0.5f;
//const float HALF_SUBJECT_POINTS_PITCH_Y = SUBJECT_POINTS_PITCH_Y * 0.5f;
//
////------------------------------
//
//
//// 定数(3次元配列側)
////------------------------------
//
//const int BOX_DETAIL_N = 3;
//const float Z_PLANE_IMG_PITCH = DISPLAY_PX_PITCH / (float)BOX_DETAIL_N;
//const int Z_PLANE_IMG_PX_X = 960;
//const int Z_PLANE_IMG_PX_Y = 600;
//const int NUM_Z_PLANE = 60;
//const float BOX_MIN_Z = 200.0f;
//const float COEF_TRANS = ((float)NUM_Z_PLANE - 0.0f) * BOX_MIN_Z;
//const float INV_COEF_TRANS = 1.0f / COEF_TRANS;
//const int HALF_SEARCH_BOX_SIZE = (int)floor(BOX_DETAIL_N / 2);
////const int HALF_SEARCH_BOX_SIZE = 0;
//
////------------------------------
//
//
//// 定数(そのほか)
////------------------------------
//
//const int NUM_THREADS = 30;
//
////------------------------------
//
//void insert_pixels(int start, int end, int NUM_ELEM_IMG_PX_X, int NUM_LENS_X, int NUM_LENS_Y, float FLOAT_NUM_ELEM_IMG_PX_X, int NUM_Z_PLANE, int Z_PLANE_IMG_PX_X, int Z_PLANE_IMG_PX_Y, cv::Mat& img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int** u_px, int** v_px);
//int writeCSV1(const std::vector<double> array);
//int writeCSV2(const std::vector<std::vector<float>> array);
//void zerosBox(int start, int end, int*** red, int*** green, int*** blue, bool*** alpha, int*** num_pcd);
//void averageSlices(int start, int end, int*** red, int*** green, int*** blue, bool*** alpha, int*** num_pcd);
//
//int main(int argc, char* argv[])
//{
//
//    cout << "PCSJ2025 wideview v1" << endl;
//
//    //std::string WINNAME = "image";
//    //cv::namedWindow(WINNAME);
//    //HWND window = FindWindow(NULL, L"image");
//    //SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
//    //SetWindowPos(window, NULL, 3280, 0, 3840, 2400, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
//
//    //std::vector<std::vector<float>> array(5, std::vector<float>(6)); // 横：subz, 縦：BOX_DETAIL_N
//
//    int index = 0;
//    bool interpolation = true;
//
//    int idx_pt = 0;
//    for (int pt = 3; pt <= 3; pt++) {
//
//        std::vector<double> array(8);
//
//        int idx = 0;
//        for (float subz = 300.0; subz <= 1000.0; subz += 100) {
//
//
//            // 定数(被写体側)
//            //------------------------------
//
//            const float SUBJECT_Z = subz;
//            const float SUBJECT_SIZE_X = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//            const float SUBJECT_SIZE_Y = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の垂直方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
//            const float SUBJECT_POINTS_PITCH_X = SUBJECT_SIZE_X / static_cast<float>(NUM_SUBJECT_POINTS_X);
//            const float SUBJECT_POINTS_PITCH_Y = SUBJECT_SIZE_Y / static_cast<float>(NUM_SUBJECT_POINTS_Y);
//            const float HALF_SUBJECT_POINTS_PITCH_X = SUBJECT_POINTS_PITCH_X * 0.5f;
//            const float HALF_SUBJECT_POINTS_PITCH_Y = SUBJECT_POINTS_PITCH_Y * 0.5f;
//
//            //------------------------------
//
//            int TIMES = 10;
//
//            // 各要素画像の原点画素位置(左上)
//            int** u_px = (int**)malloc(sizeof(int*) * NUM_ELEM_IMG_PX_X);
//            int** v_px = (int**)malloc(sizeof(int*) * NUM_ELEM_IMG_PX_Y);
//            for (int i = 0; i < NUM_ELEM_IMG_PX_X; i++) {
//                u_px[i] = (int*)malloc(sizeof(int) * NUM_LENS_X);
//            }
//            for (int i = 0; i < NUM_ELEM_IMG_PX_Y; i++) {
//                v_px[i] = (int*)malloc(sizeof(int) * NUM_LENS_Y);
//            }
//
//            // 3次元配列のインデックス
//            int*** nx = (int***)malloc(sizeof(int**) * NUM_ELEM_IMG_PX_X);
//            int*** ny = (int***)malloc(sizeof(int**) * NUM_ELEM_IMG_PX_Y);
//            for (int i = 0; i < NUM_ELEM_IMG_PX_X; i++) {
//
//                nx[i] = (int**)malloc(sizeof(int*) * NUM_LENS_X);
//
//                for (int j = 0; j < NUM_LENS_X; j++) {
//                    nx[i][j] = (int*)malloc(sizeof(int) * NUM_Z_PLANE);
//                }
//            }
//            for (int i = 0; i < NUM_ELEM_IMG_PX_Y; i++) {
//
//                ny[i] = (int**)malloc(sizeof(int*) * NUM_LENS_Y);
//
//                for (int j = 0; j < NUM_LENS_Y; j++) {
//                    ny[i][j] = (int*)malloc(sizeof(int) * NUM_Z_PLANE);
//                }
//            }
//
//            float s, t, u, v, xt, yt, zt;
//
//            float offsetS = -((NUM_LENS_X - 1) * LENS_PITCH_X) * 0.5;
//            float offsetU = -((NUM_ELEM_IMG_PX_X - 1) * DISPLAY_PX_PITCH) * 0.5;
//            for (int i = 0; i < NUM_ELEM_IMG_PX_X; i++) {
//
//                for (int j = 0; j < NUM_LENS_X; j++) {
//
//                    s = offsetS + j * LENS_PITCH_X;
//                    u = offsetU + i * DISPLAY_PX_PITCH + FOCAL_LENGTH / MIN_OBSERVE_Z * s;
//                    u_px[i][j] = static_cast<int>(floor((s + u + DISPLAY_IMG_SIZE_X * 0.5) / DISPLAY_PX_PITCH));
//                    zt = (float)(NUM_Z_PLANE - 1) * INV_COEF_TRANS;
//                    xt = s * zt + u / FOCAL_LENGTH;
//
//                    for (int nz = NUM_Z_PLANE - 1; nz >= 0; nz--) {
//                        nx[i][j][nz] = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * xt + 0.5) + Z_PLANE_IMG_PX_X * 0.5);
//                        zt -= INV_COEF_TRANS;
//                        xt -= s * INV_COEF_TRANS;
//                    }
//                }
//            }
//
//            float offsetT = -((NUM_LENS_Y - 1) * LENS_PITCH_Y) * 0.5;
//            float offsetV = -((NUM_ELEM_IMG_PX_Y - 1) * DISPLAY_PX_PITCH) * 0.5;
//            for (int i = 0; i < NUM_ELEM_IMG_PX_Y; i++) {
//
//                for (int j = 0; j < NUM_LENS_Y; j++) {
//
//                    t = offsetT + j * LENS_PITCH_Y;
//                    v = offsetV + i * DISPLAY_PX_PITCH + FOCAL_LENGTH / MIN_OBSERVE_Z * t;
//                    v_px[i][j] = static_cast<int>(floor((t + v + DISPLAY_IMG_SIZE_Y * 0.5) / DISPLAY_PX_PITCH));
//                    zt = (float)(NUM_Z_PLANE - 1) * INV_COEF_TRANS;
//                    yt = t * zt + v / FOCAL_LENGTH;
//
//                    for (int nz = NUM_Z_PLANE - 1; nz >= 0; nz--) {
//                        ny[i][j][nz] = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * yt + 0.5) + Z_PLANE_IMG_PX_Y * 0.5);
//                        zt -= INV_COEF_TRANS;
//                        yt -= t * INV_COEF_TRANS;
//                    }
//                }
//            }
//
//
//            float** data = (float**)malloc(sizeof(float*) * NUM_POINTS);
//            for (int i = 0; i < NUM_POINTS; i++) {
//                data[i] = (float*)malloc(sizeof(float) * 6);
//            }
//
//            std::string filenamein = "./images/standard/pepper.bmp";
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
//            for (int i = -HALF_NUM_SUBJECT_POINTS_Y; i < HALF_NUM_SUBJECT_POINTS_Y; i++) {
//
//                int row = i + HALF_NUM_SUBJECT_POINTS_Y;
//                for (int j = -HALF_NUM_SUBJECT_POINTS_X; j < HALF_NUM_SUBJECT_POINTS_X; j++) {
//
//                    int col = j + HALF_NUM_SUBJECT_POINTS_X;
//                    int idx = row * NUM_SUBJECT_POINTS_X + col;
//
//                    data[idx][0] = (2.0 * (float)j + 1.0) * HALF_SUBJECT_POINTS_PITCH_X;
//                    data[idx][1] = (2.0 * (float)i + 1.0) * HALF_SUBJECT_POINTS_PITCH_Y;
//                    data[idx][2] = SUBJECT_Z;
//                    data[idx][3] = resized_image.at<Vec3b>(row, col)[0];
//                    data[idx][4] = resized_image.at<Vec3b>(row, col)[1];
//                    data[idx][5] = resized_image.at<Vec3b>(row, col)[2];
//                }
//            }
//
//            cout << "upper-left point position:(" << data[0][0] << ", " << data[0][1] << ")" << endl;
//            cout << "lower-right point position:(" << data[(NUM_SUBJECT_POINTS_X - 1) * NUM_SUBJECT_POINTS_X + (NUM_SUBJECT_POINTS_X - 1)][0] << ", " << data[(NUM_SUBJECT_POINTS_X - 1) * NUM_SUBJECT_POINTS_X + (NUM_SUBJECT_POINTS_X - 1)][0] << ")" << endl;
//            cout << "//---------------------------" << endl;
//
//            int*** red;
//            int*** green;
//            int*** blue;
//            bool*** alpha;
//            int*** num_pcd;
//            //float*** val_z;
//
//            // red = new int**[NUM_Z_PLANE];
//            // green = new int**[NUM_Z_PLANE];
//            // blue = new int**[NUM_Z_PLANE];
//            // alpha = new bool**[NUM_Z_PLANE];
//            // val_z = new int**[NUM_Z_PLANE];
//
//            red = (int***)malloc(sizeof(int**) * NUM_Z_PLANE);
//            green = (int***)malloc(sizeof(int**) * NUM_Z_PLANE);
//            blue = (int***)malloc(sizeof(int**) * NUM_Z_PLANE);
//            alpha = (bool***)malloc(sizeof(bool**) * NUM_Z_PLANE);
//            num_pcd = (int***)malloc(sizeof(int**) * NUM_Z_PLANE);
//            //val_z = (float***)malloc(sizeof(float**) * NUM_Z_PLANE);
//
//            for (int i = 0; i < NUM_Z_PLANE; i++) {
//
//                // red[i] = new int*[Z_PLANE_IMG_PX_Y];
//                // green[i] = new int*[Z_PLANE_IMG_PX_Y];
//                // blue[i] = new int*[Z_PLANE_IMG_PX_Y];
//                // alpha[i] = new bool*[Z_PLANE_IMG_PX_Y];
//                // val_z[i] = new int*[Z_PLANE_IMG_PX_Y];
//
//                red[i] = (int**)malloc(sizeof(int*) * Z_PLANE_IMG_PX_Y);
//                green[i] = (int**)malloc(sizeof(int*) * Z_PLANE_IMG_PX_Y);
//                blue[i] = (int**)malloc(sizeof(int*) * Z_PLANE_IMG_PX_Y);
//                alpha[i] = (bool**)malloc(sizeof(bool*) * Z_PLANE_IMG_PX_Y);
//                num_pcd[i] = (int**)malloc(sizeof(int*) * Z_PLANE_IMG_PX_Y);
//                //val_z[i] = (float**)malloc(sizeof(float*) * Z_PLANE_IMG_PX_Y);
//
//                for (int j = 0; j < Z_PLANE_IMG_PX_Y; j++) {
//
//                    // red[i][j] = new int[Z_PLANE_IMG_PX_X];
//                    // green[i][j] = new int[Z_PLANE_IMG_PX_X];
//                    // blue[i][j] = new int[Z_PLANE_IMG_PX_X];
//                    // alpha[i][j] = new bool[Z_PLANE_IMG_PX_X];
//                    // val_z[i][j] = new int[Z_PLANE_IMG_PX_X];
//
//                    red[i][j] = (int*)malloc(sizeof(int) * Z_PLANE_IMG_PX_X);
//                    green[i][j] = (int*)malloc(sizeof(int) * Z_PLANE_IMG_PX_X);
//                    blue[i][j] = (int*)malloc(sizeof(int) * Z_PLANE_IMG_PX_X);
//                    alpha[i][j] = (bool*)malloc(sizeof(bool) * Z_PLANE_IMG_PX_X);
//                    num_pcd[i][j] = (int*)malloc(sizeof(int) * Z_PLANE_IMG_PX_X);
//                    //val_z[i][j] = (float*)malloc(sizeof(float) * Z_PLANE_IMG_PX_X);
//
//                }
//            }
//
//            cv::Mat img_display = cv::Mat::zeros(cv::Size(NUM_DISPLAY_IMG_PX_X, NUM_DISPLAY_IMG_PX_Y), CV_8UC3);
//
//            float sum_time = 0;
//            int rowsPerThread;
//            int startRow, endRow;
//            // フレーム処理
//            for (int tt = 0; tt < TIMES; tt++) {
//
//                // 測定開始時刻を記録
//                auto start = std::chrono::high_resolution_clock::now();
//
//                // 1) 各層バッファの初期化（並列）
//                vector<thread> threads;
//                rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//                for (int i = 0; i < NUM_THREADS; ++i) {
//                    startRow = i * rowsPerThread;
//                    endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//                    threads.emplace_back(zerosBox, startRow, endRow, red, green, blue, alpha, num_pcd);
//                }
//                for (auto& t : threads) {
//                    if (t.joinable()) { t.join(); }
//                }
//                threads.clear();
//
//                img_display.setTo(cv::Scalar(0, 0, 0));
//
//                float tmp_pcd_x, tmp_pcd_y, tmp_pcd_z, tmp_pcd_b, tmp_pcd_g, tmp_pcd_r;
//                float tmp_xt, tmp_yt, tmp_zt;
//                int tmp_nx, tmp_ny, tmp_nz;
//
//                // 2) 点群のビニング（幾何変換 → 量子化 → 近傍書き込み）
//                // 補間しない場合
//                if (!interpolation) {
//                    for (int k = 0; k < NUM_POINTS; k++) {
//
//                        tmp_pcd_x = data[k][0];
//                        tmp_pcd_y = data[k][1];
//                        tmp_pcd_z = data[k][2];
//                        tmp_pcd_b = data[k][3];
//                        tmp_pcd_g = data[k][4];
//                        tmp_pcd_r = data[k][5];
//
//                        tmp_zt = 1.0 / tmp_pcd_z;
//                        tmp_xt = tmp_pcd_x * tmp_zt;
//                        tmp_yt = tmp_pcd_y * tmp_zt;
//
//                        tmp_nx = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_xt + 0.5) + Z_PLANE_IMG_PX_X * 0.5);
//                        tmp_ny = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_yt + 0.5) + Z_PLANE_IMG_PX_Y * 0.5);
//                        tmp_nz = static_cast<int>(floor(COEF_TRANS * tmp_zt + 0.5));
//
//                        //cout << "dx:" << dx << ", dy:" << dy << endl;
//
//                        if (0 <= tmp_nz && tmp_nz < NUM_Z_PLANE) {
//
//                            if (0 <= tmp_nx && tmp_nx < Z_PLANE_IMG_PX_X && 0 <= tmp_ny && tmp_ny < Z_PLANE_IMG_PX_Y) {
//                                //cout << "nz:" << tmp_nz << endl;
//                                blue[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_b);
//                                green[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_g);
//                                red[tmp_nz][tmp_ny][tmp_nx] += static_cast<unsigned char>(tmp_pcd_r);
//                                alpha[tmp_nz][tmp_ny][tmp_nx] = true;
//                                num_pcd[tmp_nz][tmp_ny][tmp_nx]++;
//                            }
//                        }
//                    }
//
//                }
//                // 補間する場合
//                else {
//
//                    //点群を箱に格納
//                    if ((int)BOX_DETAIL_N % 2 == 1) {
//                        for (int k = 0; k < NUM_POINTS; k++) {
//
//                            tmp_pcd_x = data[k][0];
//                            tmp_pcd_y = data[k][1];
//                            tmp_pcd_z = data[k][2];
//                            tmp_pcd_b = data[k][3];
//                            tmp_pcd_g = data[k][4];
//                            tmp_pcd_r = data[k][5];
//
//                            tmp_zt = 1.0 / tmp_pcd_z;
//                            tmp_xt = tmp_pcd_x * tmp_zt;
//                            tmp_yt = tmp_pcd_y * tmp_zt;
//
//                            tmp_nx = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_xt + 0.5) + Z_PLANE_IMG_PX_X * 0.5);
//                            tmp_ny = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_yt + 0.5) + Z_PLANE_IMG_PX_Y * 0.5);
//                            tmp_nz = static_cast<int>(floor(COEF_TRANS * tmp_zt + 0.5));
//
//                            //cout << "dx:" << dx << ", dy:" << dy << endl;
//
//                            if (0 <= tmp_nz && tmp_nz < NUM_Z_PLANE) {
//
//                                for (int m = -HALF_SEARCH_BOX_SIZE; m <= HALF_SEARCH_BOX_SIZE; m++) {
//                                    for (int n = -HALF_SEARCH_BOX_SIZE; n <= HALF_SEARCH_BOX_SIZE; n++) {
//                                        if (0 <= tmp_nx + n && tmp_nx + n < Z_PLANE_IMG_PX_X && 0 <= tmp_ny + m && tmp_ny + m < Z_PLANE_IMG_PX_Y) {
//                                            //cout << "nz:" << tmp_nz << endl;
//                                            blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                                            green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                                            red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                                            alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                                            num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//                    else {
//
//                        float dx, dy;
//                        int half_box_size_min_x, half_box_size_max_x, half_box_size_min_y, half_box_size_max_y;
//
//                        for (int k = 0; k < NUM_POINTS; k++) {
//
//                            tmp_pcd_x = data[k][0];
//                            tmp_pcd_y = data[k][1];
//                            tmp_pcd_z = data[k][2];
//                            tmp_pcd_b = data[k][3];
//                            tmp_pcd_g = data[k][4];
//                            tmp_pcd_r = data[k][5];
//
//                            tmp_zt = 1.0 / tmp_pcd_z;
//                            tmp_xt = tmp_pcd_x * tmp_zt;
//                            tmp_yt = tmp_pcd_y * tmp_zt;
//
//                            tmp_nx = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_xt + 0.5) + Z_PLANE_IMG_PX_X * 0.5);
//                            tmp_ny = static_cast<int>(floor((FOCAL_LENGTH / Z_PLANE_IMG_PITCH) * tmp_yt + 0.5) + Z_PLANE_IMG_PX_Y * 0.5);
//                            tmp_nz = static_cast<int>(floor(COEF_TRANS * tmp_zt + 0.5));
//
//                            dx = tmp_pcd_x - (tmp_nx - 0.5 - Z_PLANE_IMG_PX_X * 0.5) * Z_PLANE_IMG_PITCH / FOCAL_LENGTH * tmp_zt;
//                            dy = tmp_pcd_y - (tmp_ny - 0.5 - Z_PLANE_IMG_PX_Y * 0.5) * Z_PLANE_IMG_PITCH / FOCAL_LENGTH * tmp_zt;
//
//                            half_box_size_min_x = static_cast<int>(round(dx / abs(dx) * 0.5 - HALF_SEARCH_BOX_SIZE * 0.5));
//                            half_box_size_max_x = static_cast<int>(round(dx / abs(dx) * 0.5 + HALF_SEARCH_BOX_SIZE * 0.5));
//                            half_box_size_min_y = static_cast<int>(round(dy / abs(dy) * 0.5 - HALF_SEARCH_BOX_SIZE * 0.5));
//                            half_box_size_max_y = static_cast<int>(round(dy / abs(dy) * 0.5 + HALF_SEARCH_BOX_SIZE * 0.5));
//
//                            //cout << "min x:" << half_box_size_min_x << ", max x:" << half_box_size_max_x << ", min y:" << half_box_size_min_y << ", max y:" << half_box_size_max_y << endl;
//
//                            if (0 <= tmp_nz && tmp_nz < NUM_Z_PLANE) {
//                                for (int m = half_box_size_min_y; m <= half_box_size_max_y; m++) {
//                                    for (int n = half_box_size_min_x; n <= half_box_size_max_x; n++) {
//                                        if (0 <= tmp_nx + n && tmp_nx + n < Z_PLANE_IMG_PX_X && 0 <= tmp_ny + m && tmp_ny + m < Z_PLANE_IMG_PX_Y) {
//                                            //cout << "nz:" << tmp_nz << endl;
//                                            blue[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_b);
//                                            green[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_g);
//                                            red[tmp_nz][tmp_ny + m][tmp_nx + n] += static_cast<unsigned char>(tmp_pcd_r);
//                                            alpha[tmp_nz][tmp_ny + m][tmp_nx + n] = true;
//                                            num_pcd[tmp_nz][tmp_ny + m][tmp_nx + n]++;
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//
//                // 3) 各画素の平均化（並列）
//                rowsPerThread = NUM_Z_PLANE / NUM_THREADS;
//                for (int i = 0; i < NUM_THREADS; ++i) {
//                    startRow = i * rowsPerThread;
//                    endRow = (i == NUM_THREADS - 1) ? NUM_Z_PLANE : (i + 1) * rowsPerThread;
//                    threads.emplace_back(averageSlices, startRow, endRow, red, green, blue, alpha, num_pcd);
//                }
//                for (auto& t : threads) {
//                    if (t.joinable()) { t.join(); }
//                }
//                threads.clear();
//
//
//                // insert_pixels(0, NUM_ELEM_IMG_PX_X, img_display, red, green, blue, alpha, nx, ny, startu, startv);
//                // 3) 対応画素の探索（並列）
//                rowsPerThread = NUM_ELEM_IMG_PX_X / NUM_THREADS;
//                for (int i = 0; i < NUM_THREADS; i++) {
//                    startRow = i * rowsPerThread;
//                    endRow = (i == NUM_THREADS - 1) ? NUM_ELEM_IMG_PX_X : (i + 1) * rowsPerThread;
//                    threads.emplace_back(insert_pixels, startRow, endRow, NUM_ELEM_IMG_PX_X, NUM_LENS_X, NUM_LENS_Y, FLOAT_NUM_ELEM_IMG_PX_X, NUM_Z_PLANE, Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y, std::ref(img_display), std::ref(red), std::ref(green), std::ref(blue), std::ref(alpha), std::ref(nx), std::ref(ny), std::ref(u_px), std::ref(v_px));
//                }
//
//                for (auto& t : threads) {
//                    if (t.joinable()) { t.join(); }
//                }
//                threads.clear();
//
//                // 測定終了時刻を記録
//                auto end = std::chrono::high_resolution_clock::now();
//
//                // 開始時刻と終了時刻の差を計算し、ミリ秒単位で出力
//                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//                sum_time += duration.count();
//
//            }
//
//            //cv::imshow("image", img_display);
//            //cv::waitKey(0);
//
//            // 表示画像の保存
//            ostringstream stream;
//            stream << "D:/ForStudy/reconstruction/PCSJ2025-prop-wideview-v1/prop-wideview-v1-pepper_f" << std::fixed << std::setprecision(4) << FOCAL_LENGTH << "_subsize" << std::fixed << std::setprecision(2) << SUBJECT_SIZE_X << "_zi" << (int)subz << ".png";
//            cv::String filename = stream.str();
//            imwrite(filename, img_display);
//
//            stream.str("");
//            stream.clear(ostringstream::goodbit);
//
//            // 各イメージプレーンの画像を保存（テスト用）
//            //ostringstream stream;
//            //cv::String filename;
//            //   cv::Mat img = cv::Mat::zeros(cv::Size(Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y), CV_8UC3);
//            //for (int zi = 24; zi < 25; zi++) {
//
//            //    stream << "E:/EvacuatedStorage/image-plane/prop-improve/prop-improve-v1-detail-pepper_tileExpand_Nz" << nzl << "_N" << BOX_DETAIL_N << "_subjectZ" << (int)subz << "_zi" << zi << ".png";
//            //    cout << "zi:" << zi << endl;
//
//            //    for (int i = 0; i < Z_PLANE_IMG_PX_Y; i++) {
//            //        for (int j = 0; j < Z_PLANE_IMG_PX_X; j++) {
//            //            if (alpha[zi][i][j] > 0) {
//            //                img.at<cv::Vec3b>(i, j)[0] = blue[zi][i][j];
//            //                img.at<cv::Vec3b>(i, j)[1] = green[zi][i][j];
//            //                img.at<cv::Vec3b>(i, j)[2] = red[zi][i][j];
//            //            }
//            //        }
//            //    }
//            //    filename = stream.str();
//            //    imwrite(filename, img);
//            //    stream.str("");
//            //    stream.clear(ostringstream::goodbit);
//            //    img = cv::Mat::zeros(cv::Size(Z_PLANE_IMG_PX_X, Z_PLANE_IMG_PX_Y), CV_8UC3);
//
//            //}
//
//            cout << "平均実行時間: " << sum_time / TIMES << " ms" << std::endl;
//            array[idx] = sum_time / TIMES;
//            idx++;
//
//            // 使用したメモリを解放
//            for (int i = 0; i < NUM_POINTS; ++i) {
//                free(data[i]);
//            }
//            free(data);
//
//            for (int i = 0; i < NUM_Z_PLANE; i++) {
//                for (int j = 0; j < Z_PLANE_IMG_PX_Y; j++) {
//                    free(red[i][j]);
//                    free(green[i][j]);
//                    free(blue[i][j]);
//                    free(alpha[i][j]);
//                    free(num_pcd[i][j]);
//                }
//                free(red[i]);
//                free(green[i]);
//                free(blue[i]);
//                free(alpha[i]);
//                free(num_pcd[i]);
//            }
//            free(red);
//            free(green);
//            free(blue);
//            free(alpha);
//            free(num_pcd);
//
//            //free(sp);
//            //free(tp);
//
//            //for (int i = 0; i < NUM_ELEM_IMG_PX_X; i++) {
//            //    free(xt[i]);
//            //    free(yt[i]);
//            //}
//            //free(xt);
//            //free(yt);
//
//            for (int i = 0; i < NUM_ELEM_IMG_PX_X; i++) {
//                for (int j = 0; j < NUM_LENS_X; j++) {
//                    free(nx[i][j]);
//                }
//                free(nx[i]);
//                free(u_px[i]);
//            }
//            for (int i = 0; i < NUM_ELEM_IMG_PX_Y; i++) {
//                for (int j = 0; j < NUM_LENS_Y; j++) {
//                    free(ny[i][j]);
//                }
//                free(ny[i]);
//                free(v_px[i]);
//            }
//            free(nx);
//            free(ny);
//            free(u_px);
//            free(v_px);
//
//        }
//
//        writeCSV1(array);
//    }
//
//    MessageBeep(-1);
//    return EXIT_SUCCESS;
//}
//
//void insert_pixels(int start, int end, int NUM_ELEM_IMG_PX_X, int NUM_LENS_X, int NUM_LENS_Y, float FLOAT_NUM_ELEM_IMG_PX_X, int NUM_Z_PLANE, int Z_PLANE_IMG_PX_X, int Z_PLANE_IMG_PX_Y, cv::Mat& img_display, int*** red, int*** green, int*** blue, bool*** alpha, int*** nx, int*** ny, int** u_px, int** v_px) {
//
//    // 各要素画像の各画素ごとに
//    for (int m = start; m < end; m++) {
//        for (int n = 0; n < NUM_ELEM_IMG_PX_X; n++) {
//            // 各要素カメラごとに
//            for (int i = 0; i < NUM_LENS_Y; i++) {
//                // 行ポインタを先に取得（BGR）
//                const int vy = v_px[m][i];
//                if ((unsigned)vy >= (unsigned)img_display.rows) continue;
//                for (int j = 0; j < NUM_LENS_X; j++) {
//                    const int ux = u_px[n][j];
//                    if ((unsigned)ux >= (unsigned)img_display.cols) continue;
//
//                    uchar* dst = img_display.ptr<uchar>(vy) + ux * 3;
//
//                    // 手前から最初に見つかった層で決定
//                    for (int nz = NUM_Z_PLANE - 1; nz >= 0; nz--) {
//                        const int tx = nx[n][j][nz];
//                        const int ty = ny[m][i][nz];
//                        if ((unsigned)tx < (unsigned)Z_PLANE_IMG_PX_X && (unsigned)ty < (unsigned)Z_PLANE_IMG_PX_Y) {
//                            if (alpha[nz][ty][tx]) {
//                                dst[0] = (uchar)blue[nz][ty][tx];   // B
//                                dst[1] = (uchar)green[nz][ty][tx];  // G
//                                dst[2] = (uchar)red[nz][ty][tx];    // R
//                                break;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
//
//int writeCSV1(const std::vector<double> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/speed/PCSJ2025-prop-wideview-v1-pepper.csv");
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
//int writeCSV2(const std::vector<std::vector<float>> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/speed/speed-IE-prop-wideview-v1-2-pepper.csv");
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
//void zerosBox(int start, int end, int*** red, int*** green, int*** blue, bool*** alpha, int*** num_pcd) {
//
//    // start..end-1 のZ層だけ初期化
//    for (int i = start; i < end; ++i) {
//        for (int j = 0; j < Z_PLANE_IMG_PX_Y; ++j) {
//            std::memset(red[i][j], 0, Z_PLANE_IMG_PX_X * sizeof(int));
//            std::memset(green[i][j], 0, Z_PLANE_IMG_PX_X * sizeof(int));
//            std::memset(blue[i][j], 0, Z_PLANE_IMG_PX_X * sizeof(int));
//            std::memset(num_pcd[i][j], 0, Z_PLANE_IMG_PX_X * sizeof(int));
//            std::memset(alpha[i][j], 0, Z_PLANE_IMG_PX_X * sizeof(bool));
//        }
//    }
//}
//
//void averageSlices(int start, int end, int*** red, int*** green, int*** blue, bool*** alpha, int*** num_pcd) {
//
//    for (int i = start; i < end; i++) {
//        for (int j = 0; j < Z_PLANE_IMG_PX_Y; j++) {
//            for (int k = 0; k < Z_PLANE_IMG_PX_X; k++) {
//                //cout << i << ", " << j << ", " << k << "; " << "red:" << red[i][j][k] << ", green:" << green[i][j][k] << ", blue:" << blue[i][j][k] << endl;
//                if (alpha[i][j][k]) {
//                    red[i][j][k] = red[i][j][k] / num_pcd[i][j][k];
//                    green[i][j][k] = green[i][j][k] / num_pcd[i][j][k];
//                    blue[i][j][k] = blue[i][j][k] / num_pcd[i][j][k];
//                }
//            }
//        }
//    }
//}

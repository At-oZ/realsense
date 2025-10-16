#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <Windows.h>

#define NOMINMAX

using namespace cv;
using namespace std;

// 定数(観察者側)
//------------------------------

const float MIN_OBSERVE_Z = 1000.0f;
const float MAX_OBSERVE_Z = 5000.0f;
//const float OBSERVER_Z = -MIN_OBSERVE_Z;

//------------------------------


// 定数(表示系側)
//------------------------------

// ディスプレイの画素ピッチ
const float DISPLAY_PX_PITCH = 13.4f * 25.4f / std::sqrt(3840.f * 3840.f + 2400.f * 2400.f);

// レンズ数
const unsigned int NUM_LENS_X = 128;
const unsigned int NUM_LENS_Y = 80;

// レンズ数の半分
const int HALF_NUM_LENS_X = NUM_LENS_X * 0.5f;
const int HALF_NUM_LENS_Y = NUM_LENS_Y * 0.5f;

// レンズの焦点距離
const float FOCAL_LENGTH = MIN_OBSERVE_Z / (3.0f * 40.0f - 1.0f);

/* 要素画像の解像度至上主義 */
// 要素画像の解像度
const unsigned int NUM_ELEM_IMG_PX_X = 60;
const unsigned int NUM_ELEM_IMG_PX_Y = 60;

// 要素画像の解像度の浮動小数点数
const float FLOAT_NUM_ELEM_IMG_PX_X = (float)NUM_ELEM_IMG_PX_X;
const float FLOAT_NUM_ELEM_IMG_PX_Y = (float)NUM_ELEM_IMG_PX_Y;

// 要素画像の間隔
const float ELEM_IMG_PITCH_X = NUM_ELEM_IMG_PX_X * DISPLAY_PX_PITCH;
const float ELEM_IMG_PITCH_Y = NUM_ELEM_IMG_PX_Y * DISPLAY_PX_PITCH;

/* レンズピッチ */
//// 無限遠に向けた光線場再現の場合(A-1)
//const string VIEW_MODE = "normal";
//const float LENS_PITCH_X = ELEM_IMG_PITCH_X;
//const float LENS_PITCH_Y = ELEM_IMG_PITCH_Y;

// 想定観察距離に向けた光線場再現の場合(A-2)
const string VIEW_MODE = "wideview";
const float LENS_PITCH_X = ELEM_IMG_PITCH_X * MIN_OBSERVE_Z / (FOCAL_LENGTH + MIN_OBSERVE_Z);
const float LENS_PITCH_Y = ELEM_IMG_PITCH_Y * MIN_OBSERVE_Z / (FOCAL_LENGTH + MIN_OBSERVE_Z);

// レンズピッチの半分
const float HALF_LENS_PITCH_X = LENS_PITCH_X / 2.0f;
const float HALF_LENS_PITCH_Y = LENS_PITCH_Y / 2.0f;

// 表示画像の解像度
const unsigned int NUM_DISPLAY_IMG_PX_X = NUM_ELEM_IMG_PX_X * NUM_LENS_X;
const unsigned int NUM_DISPLAY_IMG_PX_Y = NUM_ELEM_IMG_PX_Y * NUM_LENS_Y;

// 表示画像のサイズ
const float DISPLAY_IMG_SIZE_X = NUM_DISPLAY_IMG_PX_X * DISPLAY_PX_PITCH;
const float DISPLAY_IMG_SIZE_Y = NUM_DISPLAY_IMG_PX_Y * DISPLAY_PX_PITCH;

// 仮想カメラ設定
const float TAN_YALF_Y = ELEM_IMG_PITCH_Y / FOCAL_LENGTH * 0.5f;
const float FOV_Y = atan(TAN_YALF_Y) * 2.0f;

//------------------------------


// 定数(被写体側)
//------------------------------

// 被写体平面の奥行位置
const float SUBJECT_Z = 1000.0f;

// 被写体平面の各軸点群数
const unsigned int NUM_SUBJECT_POINTS_X = 554;
const unsigned int NUM_SUBJECT_POINTS_Y = 554;

// 被写体平面の全体点群数
const int NUM_POINTS = NUM_SUBJECT_POINTS_X * NUM_SUBJECT_POINTS_Y;

// 被写体平面の各軸点群数の半分
const int HALF_NUM_SUBJECT_POINTS_X = NUM_SUBJECT_POINTS_X * 0.5f;
const int HALF_NUM_SUBJECT_POINTS_Y = NUM_SUBJECT_POINTS_Y * 0.5f;

// 被写体平面のサイズ
const float SUBJECT_SIZE_X = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の水平方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);
const float SUBJECT_SIZE_Y = DISPLAY_IMG_SIZE_Y * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z; // 被写体の垂直方向のサイズ(拡大する場合" * (SUBJECT_Z + MIN_OBSERVE_Z) / MIN_OBSERVE_Z"を追加);

// 被写体平面の点群間隔
const float SUBJECT_POINTS_PITCH_X = SUBJECT_SIZE_X / static_cast<float>(NUM_SUBJECT_POINTS_X);
const float SUBJECT_POINTS_PITCH_Y = SUBJECT_SIZE_Y / static_cast<float>(NUM_SUBJECT_POINTS_Y);

// 被写体平面の点群間隔の半分
const float HALF_SUBJECT_POINTS_PITCH_X = SUBJECT_POINTS_PITCH_X * 0.5f;
const float HALF_SUBJECT_POINTS_PITCH_Y = SUBJECT_POINTS_PITCH_Y * 0.5f;

//------------------------------


// 定数(3次元配列側)
//------------------------------

const int N = 3;
const float Z_PLANE_IMG_PITCH = DISPLAY_PX_PITCH / (float)N;
const int Z_PLANE_IMG_PX_X = 960;
const int Z_PLANE_IMG_PX_Y = 600;
const int NUM_Z_PLANE = 60;
const float MIN_Z = 200.0f;
const float COEF_TRANS = ((float)NUM_Z_PLANE - 0.0f) * MIN_Z;
const float INV_COEF_TRANS = 1.0f / COEF_TRANS;
const int HALF_SEARCH_BOX_SIZE = (int)floor(N / 2);
//const int HALF_SEARCH_BOX_SIZE = 0;

//------------------------------

double getPSNR(const Mat& I1, const Mat& I2);
Mat getDiff(const Mat& I1, const Mat& I2);
int writeCSV1(const std::vector<double> array);
int writeCSV2(const std::vector<std::vector<double>> array, int NxNy, int ptimes);
//cv::Mat direct_observe(cv::Mat input_image, double observer_x, double observer_y, double observe_z, double pixel_size, int display_image_px, double tile_size_mm, int image_size_px, double tile_pos);
cv::Mat observe(cv::Mat inputImg, int numPinholeX, int numPinholeY, double pinholePitchX, double pinholePitchY, double focalLength, double observerX, double observerY, double observerZ, double pinholeSizeX, double pinholeSizeY, double displayImgSizeX, double displayImgSizeY, double displayPxPitchX, double displayPxPitchY, int numDisplayImgPxX, int numDisplayImgPxY);

int main(int argc, char* argv[]) {

    cout << "PCSJ2025-observe-evaluation" << endl;

    cout << "MIN OBSERVE Z:" << MIN_OBSERVE_Z << endl;
    cout << "//----------------------------" << endl;
    cout << "DISPLAY PIXEL PITCH:" << DISPLAY_PX_PITCH << endl;
    cout << "NUM LENS WIDTH:" << NUM_LENS_X << endl;
    cout << "NUM LENS HEIGHT:" << NUM_LENS_Y << endl;
    cout << "LENS PITCH X:" << LENS_PITCH_X << endl;
    cout << "LENS PITCH Y:" << LENS_PITCH_Y << endl;
    cout << endl;
    cout << "ELEMENTAL IMAGE PITCH X:" << ELEM_IMG_PITCH_X << endl;
    cout << "ELEMENTAL IMAGE PITCH Y:" << ELEM_IMG_PITCH_Y << endl;
    cout << "FLOAT NUM ELEMENTAL IMAGE PX X:" << FLOAT_NUM_ELEM_IMG_PX_X << endl;
    cout << "FLOAT NUM ELEMENTAL IMAGE PX Y:" << FLOAT_NUM_ELEM_IMG_PX_Y << endl;
    cout << "NUM ELEMENTAL IMAGE PX X:" << NUM_ELEM_IMG_PX_X << endl;
    cout << "NUM ELEMENTAL IMAGE PX Y:" << NUM_ELEM_IMG_PX_Y << endl;
    cout << "DISPLAY IMG SIZE X:" << DISPLAY_IMG_SIZE_X << endl;
    cout << "DISPLAY IMG SIZE Y:" << DISPLAY_IMG_SIZE_Y << endl;
    cout << endl;
    cout << "FOCAL LENGTH:" << FOCAL_LENGTH << endl;
    cout << "TAN HALF Y:" << TAN_YALF_Y << endl;
    cout << "FOV Y:" << FOV_Y << endl;
    cout << "WINDOW WIDTH:" << NUM_DISPLAY_IMG_PX_X << endl;
    cout << "WINDOW HEIGHT:" << NUM_DISPLAY_IMG_PX_Y << endl;
    cout << "//----------------------------" << endl;
    cout << "SUBJECT Z:" << SUBJECT_Z << endl;
    cout << "NUM SUBJECT POINTS X:" << NUM_SUBJECT_POINTS_X << endl;
    cout << "NUM SUBJECT POINTS Y:" << NUM_SUBJECT_POINTS_Y << endl;
    cout << "NUM POINTS:" << NUM_POINTS << endl;
    cout << "SUBJECT SIZE X:" << SUBJECT_SIZE_X << endl;
    cout << "SUBJECT SIZE Y:" << SUBJECT_SIZE_Y << endl;
    cout << "SUBJECT POINTS PITCH X:" << SUBJECT_POINTS_PITCH_X << endl;
    cout << "SUBJECT POINTS PITCH Y:" << SUBJECT_POINTS_PITCH_Y << endl;
    cout << "//----------------------------" << endl;
    cout << endl;

    ostringstream stream;

    //std::vector<std::vector<double>> array(20, std::vector<double>(6)); // 横：subz, 縦：nph
    //std::vector<std::vector<double>> array(10, std::vector<double>(6)); // 横：subz, 縦：nzl
    //std::vector<std::vector<double>> array(1, std::vector<double>(8)); // 横：subz, 縦：ptimes
    std::vector<double> array(8);

    // 画像を読み込む
    std::string filenameTile = "./images/standard/mandrill.bmp";
    cv::Mat tileImg = cv::imread(filenameTile);
    int numTilePxX = tileImg.size().width;
    int numTilePxY = tileImg.size().height;
    cout << numTilePxX << endl;
    cout << numTilePxY << endl;

    if (tileImg.empty())
    {
        std::cout << "画像を開くことができませんでした。\n";
        return -1;
    }

    //std::random_device seed_gen;
    int seedx = 0;
    int seedy = 1;
    int seedz = 2;
    std::mt19937 enginex(seedx);
    std::mt19937 enginey(seedy);
    std::mt19937 enginez(seedz);

    ////// 領域2, 3用
    //std::uniform_real_distribution<double> dist(-(FOCAL_LENGTH + MIN_OBSERVE_Z) / FOCAL_LENGTH * LENS_PITCH_X * 0.5, (FOCAL_LENGTH + MIN_OBSERVE_Z) / FOCAL_LENGTH * LENS_PITCH_X * 0.5);

    //int rand_size = 100;
    //double* observer_x = new double[rand_size];
    //double* observer_y = new double[rand_size];

    //// 除外する範囲の設定
    //const double exclude_min = -LENS_PITCH_X * (MIN_OBSERVE_Z / FOCAL_LENGTH - (NUM_LENS_X - 1)) * 0.5;
    //const double exclude_max = LENS_PITCH_X * (MIN_OBSERVE_Z / FOCAL_LENGTH - (NUM_LENS_X - 1)) * 0.5;

    //for (int i = 0; i < rand_size; i++) {
    //    // 除外範囲外の値が得られるまで乱数を生成
    //    double x, y;
    //    do {
    //        x = dist(enginex);
    //        y = dist(enginey);
    //    } while (x >= exclude_min && x <= exclude_max && y >= exclude_min && y <= exclude_max);

    //    observer_x[i] = x;
    //    observer_y[i] = y;
    //}
    //// 最後の要素は元のコードと同様に(0,0)にします
    ////observer_x[99] = 0;
    ////observer_y[99] = 0;

    // 領域1用
    std::uniform_real_distribution<double> distXY(-ELEM_IMG_PITCH_X * MIN_OBSERVE_Z / FOCAL_LENGTH * 0.5 + 10.0, ELEM_IMG_PITCH_X * MIN_OBSERVE_Z / FOCAL_LENGTH * 0.5 - 10.0);
    std::uniform_real_distribution<double> distZ(-MAX_OBSERVE_Z, -MIN_OBSERVE_Z);

    int rand_size = 1000;
    double* observer_x = new double[rand_size];
    double* observer_y = new double[rand_size];
    double* observer_z = new double[rand_size];

    for (int i = 1; i < rand_size; i++) {
        observer_x[i] = distXY(enginex);
        observer_y[i] = distXY(enginey);
        observer_z[i] = distZ(enginez);
    }
    observer_x[0] = 0.0;
    observer_y[0] = 0.0;
    observer_z[0] = -1000.0;

    int idx = 0;
    for (double subz = 300.0; subz <= 1000.0; subz += 100) {

        // 被写体平面の拡大後サイズを一貫して計算
        double tileSizeX = DISPLAY_IMG_SIZE_Y * (subz + MIN_OBSERVE_Z) / MIN_OBSERVE_Z;
        double tileSizeY = DISPLAY_IMG_SIZE_Y * (subz + MIN_OBSERVE_Z) / MIN_OBSERVE_Z;
        double tilePxPitchX = tileSizeX / (double)numTilePxX;
        double tilePxPitchY = tileSizeY / (double)numTilePxY;

        // 理想表示用の表示画像の読み込み
        //std::string filename_display_standard = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal/ideal-tileExpand_mandrill_gridSize" + std::to_string(nph) + "_zi" + std::to_string((int)tile_pos) + ".png";
        //cv::Mat display_standard_image = cv::imread(filename_display_standard);

        //if (display_standard_image.empty())
        //{
        //    std::cout << "画像を開くことができませんでした。\n";
        //    return -1;
        //}

        // 比較対象用の表示画像の読み込み
        stream.str("");
        //stream << "D:/ForStudy/reconstruction/PCSJ2025-prop-normal-v1/prop-normal-v1-grid_f" << std::fixed << std::setprecision(4) << FOCAL_LENGTH << "_subsize" << std::fixed << std::setprecision(2) << tileSize << "_zi" << (int)subz << ".png";
        //stream << "D:/ForStudy/reconstruction/PCSJ2025-OpenGL-normal-v1/PCSJ2025-OpenGL-normal-v1-grid_f" << std::fixed << std::setprecision(4) << FOCAL_LENGTH << "_subsize" << std::fixed << std::setprecision(2) << tileSize << "_zi" << (int)subz << "_2.png";
        //stream << "D:/ForStudy/reconstruction/PCSJ2025-prop-wideview-v1/prop-wideview-v1-mandrill_f" << std::fixed << std::setprecision(4) << FOCAL_LENGTH << "_subsize" << std::fixed << std::setprecision(2) << tileSizeY << "_zi" << (int)subz << ".png";
        stream << "D:/ForStudy/reconstruction/PCSJ2025-OpenGL-wideview-v1/PCSJ2025-OpenGL-wideview-v1-mandrill_f" << std::fixed << std::setprecision(4) << FOCAL_LENGTH << "_subsize" << std::fixed << std::setprecision(2) << tileSizeY << "_zi" << (int)subz << ".png";
        cout << "filename compared image:" << stream.str() << endl;
        std::string filename_display_compared = stream.str();
        cv::Mat display_compared_image = cv::imread(filename_display_compared);

        if (display_compared_image.empty())
        {
            std::cout << "画像を開くことができませんでした。\n";
            return -1;
        }

        double sum_psnr = 0;
        for (int nobs = 0; nobs < rand_size; ++nobs) {

            try {

                // 直接観察の観察画像
                //cv::Mat resized_std_img = direct_observe(tileImg, observer_x[nobs], observer_y[nobs], MIN_OBSERVE_Z, DISPLAY_PX_PITCH, WIN_W, tileSize, tilePx, subz); // 解像度を2400 x 2400にする場合
                cv::Mat standard_image = observe(tileImg, NUM_LENS_X, NUM_LENS_Y, LENS_PITCH_X, LENS_PITCH_Y, subz, observer_x[nobs], observer_y[nobs], observer_z[nobs], LENS_PITCH_X, LENS_PITCH_Y, tileSizeX, tileSizeY, tilePxPitchX, tilePxPitchY, numTilePxX, numTilePxY);
                //cv::Mat standard_image = cv::Mat::zeros(cv::Size(NUM_LENS_X, NUM_LENS_Y), CV_8UC3); // 検証用

                // 理想表示の観察画像
                //cv::Mat standard_image = observe(display_standard_image, nph, LENS_PITCH_X, FOCAL_LENGTH, observer_x[nobs], observer_y[nobs], MIN_OBSERVE_Z, VISIBLE_RANGE_THROUGH_LENS, DISPLAY_IMG_SIZE_X, DISPLAY_PX_PITCH, WIN_W);
                //cv::Mat resized_std_img;
                //cv::resize(standard_image, resized_std_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);

                // 比較対象の観察画像
                cv::Mat compared_image = observe(display_compared_image, NUM_LENS_X, NUM_LENS_Y, LENS_PITCH_X, LENS_PITCH_Y, FOCAL_LENGTH, observer_x[nobs], observer_y[nobs], observer_z[nobs], DISPLAY_PX_PITCH, DISPLAY_PX_PITCH, DISPLAY_IMG_SIZE_X, DISPLAY_IMG_SIZE_Y, DISPLAY_PX_PITCH, DISPLAY_PX_PITCH, NUM_DISPLAY_IMG_PX_X, NUM_DISPLAY_IMG_PX_Y);
                //cv::Mat resized_cmpd_img;
                //cv::resize(compared_image, resized_cmpd_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);

    //            // 観察画像のウィンドウ表示
    //            cv::imshow("images", standard_image);
    //            cv::waitKey(0);

    //            // 観察画像のウィンドウ表示
    //            cv::imshow("images", compared_image);
                //cv::waitKey(0);

                /* 観察画像の保存 */

                // 解像度2400 x 2400の場合
                //std::string std_filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/grid/real-direct_2400_tileExpand/real-direct-observe_" + std::to_string(static_cast<int>(WIN_W)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(MIN_OBSERVE_Z))) + ".png";
                //std::string cmpd_filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/grid/prop-improve-v1_2400_tileExpand/prop-improve-v1-observe_" + std::to_string(static_cast<int>(WIN_W)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(MIN_OBSERVE_Z))) + ".png";

                // 解像度nph x nphの場合->400x400にリサイズして保存
                //std::ostringstream filenameout_stream;
                //filenameout_stream << "D:/ForStudy/observe-images/real-direct/real-direct-grid-observe_" << NUM_LENS_W << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(abs(MIN_OBSERVE_Z)) << "mm.png";
                //std::string std_filenameout = filenameout_stream.str();
                //cv::Mat resized_std_img;
                //cv::resize(standard_image, resized_std_img, cv::Size(400, 400), 0, 0, cv::INTER_NEAREST);
                //cv::imwrite(std_filenameout, resized_std_img);
                //std::cout << "基準画像を" << std_filenameout << "として保存しました。\n";

                std::ostringstream filenameout_stream;
                //filenameout_stream.str("");
                //filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-prop-normal-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-prop-normal-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                //filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-OpenGL-normal-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-OpenGL-normal-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                //filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-prop-wideview-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-prop-wideview-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                filenameout_stream << "D:/ForStudy/observe-images/PCSJ2025-OpenGL-wideview-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-OpenGL-wideview-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                std::string cmpd_filenameout = filenameout_stream.str();
                //cv::Mat resized_cmpd_img;
                //cv::resize(compared_image, resized_cmpd_img, cv::Size(400, 400), 0, 0, cv::INTER_NEAREST);

                cv::imwrite(cmpd_filenameout, compared_image);
                //std::cout << "比較画像を" << cmpd_filenameout << "として保存しました。\n";

                //// 観察画像の差分
                //cv::Mat diff;
                //cv::absdiff(standard_image, compared_image, diff);

                //std::ostringstream filenameout_stream;
                ////filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-prop-normal-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-prop-normal-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                ////filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-OpenGL-normal-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-OpenGL-normal-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                ////filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-prop-wideview-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-prop-wideview-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";
                //filenameout_stream << "D:/ForStudy/observe-images/diff/PCSJ2025-OpenGL-wideview-v1/subz_" << static_cast<int>(subz) << "mm/PCSJ2025-OpenGL-wideview-v1-grid-observe_" << NUM_LENS_X << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(observer_z[nobs]) << "mm.png";

                //std::string diff_filenameout = filenameout_stream.str();
                //cv::imwrite(diff_filenameout, diff);

                //cv::Mat observe_img = cv::Mat::zeros(cv::Size(2400, 2400), CV_8UC3);
                //for (int i = 0; i < 2400; ++i) {

                //    if (i < 1200) {
                //        for (int j = 0; j < 2400; ++j) {
                //            if (j < 1200) {
                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_std_img.at<cv::Vec3b>(i, j)[0];
                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_std_img.at<cv::Vec3b>(i, j)[1];
                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_std_img.at<cv::Vec3b>(i, j)[2];
                //            }
                //            else {
                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_cmpd_img.at<cv::Vec3b>(i, j)[0];
                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_cmpd_img.at<cv::Vec3b>(i, j)[1];
                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_cmpd_img.at<cv::Vec3b>(i, j)[2];
                //            }
                //        }
                //    }
                //    else {
                //        for (int j = 0; j < 2400; ++j) {
                //            if (j < 1200) {
                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_cmpd_img.at<cv::Vec3b>(i, j)[0];
                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_cmpd_img.at<cv::Vec3b>(i, j)[1];
                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_cmpd_img.at<cv::Vec3b>(i, j)[2];
                //            }
                //            else {
                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_std_img.at<cv::Vec3b>(i, j)[0];
                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_std_img.at<cv::Vec3b>(i, j)[1];
                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_std_img.at<cv::Vec3b>(i, j)[2];
                //            }
                //        }
                //    }
                //}
                //std::string observe_filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/compare/prop-improve-v1_2400_tileNotExpand/DeltaFocal" + std::to_string(static_cast<int>(Df)) + "mm/prop-improve-v1-observe_" + std::to_string(static_cast<int>(WIN_W)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(MIN_OBSERVE_Z))) + ".png";
                //cv::imwrite(observe_filenameout, observe_img);
                //std::cout << "比較用観察画像を" << observe_filenameout << "として保存しました。\n";

                // 移植の整合性の確認（点検用）
                //std::string filename_original = "C:/Users/taw11/EvacuatedStorage/observe-image/mandrill/lensarray/ideal/gridSize" + std::to_string(nph) + "/ideal-observe-lenna_" + std::to_string(nph) + "px_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(MIN_OBSERVE_Z))) + ".png";
                //std::string filename_original = "C:/Users/taw11/EvacuatedStorage/observe-image/ICIP/prop-original-v1/prop-v1-observe-mandrill_Nz" + std::to_string(nzl) + "_subjectZ" + std::to_string((int)subz) + "mm_obx" + std::to_string((int)(observer_x[nobs] * 1000)) + "um_oby" + std::to_string((int)(observer_y[nobs] * 1000)) + "um_obz" + std::to_string((int)abs(MIN_OBSERVE_Z)) + "mm.png";
                //cv::Mat original_image = cv::imread(filename_original);

                //if (compared_image.empty())
                //{
                //    std::cout << "画像を開くことができませんでした。\n";
                //    return -1;
                //}

                //psnrValue = getPSNR(resized_std_img, resized_cmpd_img); // 解像度を2400 x 2400に統一した場合
                double psnr = getPSNR(standard_image, compared_image);
                sum_psnr += psnr;
                //cout << "PSNR value is: " << psnrValue << " dB" << endl;

                //cv::Mat img_diff = getDiff(resized_std_img, compared_image);

                //for (int i = 0; i < WIN_W; ++i) {
                //    for (int j = 0; j < WIN_W; ++j) {
                //        if (img_diff.at<cv::Vec3b>(i, j)[0] > 0 && img_diff.at<cv::Vec3b>(i, j)[1] > 0 && img_diff.at<cv::Vec3b>(i, j)[2] > 0) {
                //            img_diff.at<cv::Vec3b>(i, j)[0] = 255;
                //            img_diff.at<cv::Vec3b>(i, j)[1] = 255;
                //            img_diff.at<cv::Vec3b>(i, j)[2] = 255;
                //        }
                //    }
                //}

                //// img_diffをファイルとして保存して内容を確認する
                //cv::imwrite("img_diff_" + std::to_string(static_cast<int>(nobs)) + ".png", img_diff);

                //ostringstream stream;
                stream.str("");
                stream.clear(ostringstream::goodbit);

            }
            catch (cv::Exception& e) {
                cout << "OpenCV exception: " << e.what() << endl;
                return -1;
            }
            catch (std::exception& e) {
                cout << "Standard exception: " << e.what() << endl;
                return -1;
            }
            catch (...) {
                cout << "Unknown exception occurred." << endl;
                return -1;
            }
        }

        cout << "Average PSNR value is: " << (sum_psnr / rand_size) << " dB" << endl;
        array[idx] = sum_psnr / rand_size;
        idx++;
    }
    //writeCSV1(array);

    delete[] observer_x;
    delete[] observer_y;
    delete[] observer_z;

    MessageBeep(-1);
    return 0;
}

double getPSNR(const Mat& I1, const Mat& I2) {
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // Convert to double
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);        // Sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // Sum channels

    if (sse <= 1e-10) {        // For small values return zero
        return 0;
    }
    else {
        double mse = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

Mat getDiff(const Mat& I1, const Mat& I2) {
    Mat s1;
    absdiff(I1, I2, s1);
    s1.convertTo(s1, CV_8U);  // Convert to 8-bit
    return s1;
}

int writeCSV1(const std::vector<double> array) {

    // 出力ファイルを開く
    //std::ofstream file("./numbers/PCSJ2025-prop-wideview-v1-direct-mandrill.csv");
    std::ofstream file("./numbers/PCSJ2025-OpenGL-wideview-v1-direct-mandrill.csv");

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

int writeCSV2(const std::vector<std::vector<double>> array, int NxNy, int ptimes) {

    // 出力ファイルを開く
    std::ofstream file("./numbers/PSNR/prop-improve/by_real/detail/psnr-mandrill_real_prop-improve-NxNy" + std::to_string(NxNy) + "_pt" + std::to_string(ptimes) + "-nph_tileExpand.csv");

    // ファイルが正しく開けたか確認
    if (!file.is_open()) {
        std::cerr << "ファイルが開けませんでした" << std::endl;
        return 1;
    }

    // 2次元配列の内容をCSV形式でファイルに書き込む
    for (const auto& row : array) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) {
                file << ","; // 各要素の間にカンマを挿入
            }
        }
        file << "\n"; // 行の終わりに改行を挿入
    }

    // ファイルを閉じる
    file.close();

    std::cout << "書き込みが完了しました。" << std::endl;

    return 0;
}

cv::Mat observe(cv::Mat inputImg, int numPinholeX, int numPinholeY, double pinholePitchX, double pinholePitchY, double focalLength, double observerX, double observerY, double observerZ, double pinholeSizeX, double pinholeSizeY, double displayImgSizeX, double displayImgSizeY, double displayPxPitchX, double displayPxPitchY, int numDisplayImgPxX, int numDisplayImgPxY) {

    // ピンホールの位置を計算
    //------------------------------

    std::vector<cv::Point2d> pinholePosition;
    int halfNumLensX = numPinholeX / 2;
    int halfNumLensY = numPinholeY / 2;
    double halfPinholePitchX = pinholePitchX / 2.0;
    double halfPinholePitchY = pinholePitchY / 2.0;

    if (numPinholeX % 2 == 0 && numPinholeY % 2 == 0)
    {
        for (int i = -halfNumLensY; i < halfNumLensY; ++i)
        {
            for (int j = -halfNumLensX; j < halfNumLensX; ++j)
            {
                double x = (2.0 * (double)j + 1.0) * halfPinholePitchX;
                double y = (2.0 * (double)i + 1.0) * halfPinholePitchY;
                pinholePosition.emplace_back(x, y);
            }
        }

    }
    else if (numPinholeX % 2 == 0 && numPinholeY % 2 == 1)
    {
        for (int i = -halfNumLensY; i <= halfNumLensY; ++i)
        {
            for (int j = -halfNumLensX; j <= halfNumLensX; ++j)
            {
                double x = (2.0 * (double)j + 1.0) * halfPinholePitchX;
                double y = (double)i * pinholePitchY;
                pinholePosition.emplace_back(x, y);
            }
        }

    }
    else if (numPinholeX % 2 == 1 && numPinholeY % 2 == 0)
    {
        for (int i = -halfNumLensY; i <= halfNumLensY; ++i)
        {
            for (int j = -halfNumLensX; j <= halfNumLensX; ++j)
            {
                double x = (double)j * pinholePitchX;
                double y = (2.0 * (double)i + 1.0) * halfPinholePitchY;
                pinholePosition.emplace_back(x, y);
            }
        }

    }
    else
    {
        for (int i = -halfNumLensY; i <= halfNumLensY; ++i)
        {
            for (int j = -halfNumLensX; j <= halfNumLensX; ++j)
            {
                double x = (double)j * pinholePitchX;
                double y = (double)i * pinholePitchY;
                pinholePosition.emplace_back(x, y);
            }
        }

    }

    //------------------------------


    // 各ピンホールについて観察される色を計算
    //------------------------------

    cv::Mat output_image(numPinholeY, numPinholeX, CV_8UC3);

    int index = 0;
    for (int i = 0; i < numPinholeY; ++i)
    {
        for (int j = 0; j < numPinholeX; ++j)
        {
            const auto& pinholePos = pinholePosition[index++];

            // ピンホールを通して見える被写体平面上の領域を計算
            // 視線を通してピンホールから被写体平面への射影を計算
            double scale = (focalLength - observerZ) / (0.0 - observerZ); // z=observerZからz=focalLengthへの拡大率
            double viewX = (pinholePos.x - observerX) * scale + observerX;
            double viewY = (pinholePos.y - observerY) * scale + observerY;

            // 被写体平面上での可視領域（ピンホールの開口を考慮）
            double halfPinholeSizeX = pinholeSizeX / 2.0;
            double halfPinholeSizeY = pinholeSizeY / 2.0;
            double viewHalfPinholeSizeX = halfPinholeSizeX * scale;
            double viewHalfPinholeSizeY = halfPinholeSizeY * scale;

            double visibleAreaMinX = viewX - viewHalfPinholeSizeX;
            double visibleAreaMaxX = viewX + viewHalfPinholeSizeX;
            double visibleAreaMinY = viewY - viewHalfPinholeSizeY;
            double visibleAreaMaxY = viewY + viewHalfPinholeSizeY;

            // 被写体平面の範囲内に制限
            visibleAreaMinX = (std::max)(-displayImgSizeX / 2.0, visibleAreaMinX);
            visibleAreaMaxX = (std::min)(displayImgSizeX / 2.0, visibleAreaMaxX);
            visibleAreaMinY = (std::max)(-displayImgSizeY / 2.0, visibleAreaMinY);
            visibleAreaMaxY = (std::min)(displayImgSizeY / 2.0, visibleAreaMaxY);

            // 可視領域が被写体平面内に存在しない場合
            if (visibleAreaMinX >= visibleAreaMaxX || visibleAreaMinY >= visibleAreaMaxY)
            {
                // 黒色を設定
                output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                continue;
            }

            // ピクセルインデックスの範囲を計算
            int x_min_idx = (std::max)(0, static_cast<int>(std::floor((visibleAreaMinX + displayImgSizeX / 2) / displayPxPitchX)));
            int x_max_idx = (std::min)(numDisplayImgPxX - 1, static_cast<int>(std::floor((visibleAreaMaxX + displayImgSizeX / 2) / displayPxPitchX)));
            int y_min_idx = (std::max)(0, static_cast<int>(std::floor((visibleAreaMinY + displayImgSizeY / 2) / displayPxPitchY)));
            int y_max_idx = (std::min)(numDisplayImgPxY - 1, static_cast<int>(std::floor((visibleAreaMaxY + displayImgSizeY / 2) / displayPxPitchY)));

            //std::cout << "x min idx:" << x_min_idx << ", x max idx:" << x_max_idx << ", y min idx:" << y_min_idx << ", y max idx:" << y_max_idx << std::endl;

            // 観察される色の加重平均を計算
            cv::Vec3d weighted_color(0, 0, 0);
            double total_area = 0.0;

            for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
            {
                // ピクセルのx座標範囲を計算
                double x_pixel_min = x_idx * displayPxPitchX - displayImgSizeX / 2;
                double x_pixel_max = (x_idx + 1) * displayPxPitchX - displayImgSizeX / 2;

                double x_overlap = (std::max)(0.0, (std::min)(visibleAreaMaxX, x_pixel_max) - (std::max)(visibleAreaMinX, x_pixel_min));

                if (x_overlap <= 0) continue;

                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
                {
                    // ピクセルのy座標範囲を計算
                    double y_pixel_min = y_idx * displayPxPitchY - displayImgSizeY / 2;
                    double y_pixel_max = (y_idx + 1) * displayPxPitchY - displayImgSizeY / 2;

                    double y_overlap = (std::max)(0.0, (std::min)(visibleAreaMaxY, y_pixel_max) - (std::max)(visibleAreaMinY, y_pixel_min));

                    if (y_overlap <= 0) continue;

                    double overlap_area = x_overlap * y_overlap;
                    total_area += overlap_area;

                    // ピクセル値を取得し、加重平均に加算
                    cv::Vec3b pixel = inputImg.at<cv::Vec3b>(y_idx, x_idx);
                    weighted_color += cv::Vec3d(pixel) * overlap_area;
                }
            }

            cv::Vec3b final_color;
            if (total_area > 0)
            {
                weighted_color /= total_area;
                final_color = cv::Vec3b(
                    static_cast<uchar>(weighted_color[0]),
                    static_cast<uchar>(weighted_color[1]),
                    static_cast<uchar>(weighted_color[2])
                );
            }
            else
            {
                // 観察可能な領域がない場合は黒色
                final_color = cv::Vec3b(0, 0, 0);
            }

            // 出力画像に色を設定
            output_image.at<cv::Vec3b>(i, j) = final_color;
        }
    }

    //------------------------------

    return output_image;
}

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <random>

using namespace cv;
using namespace std;

double getPSNR(const Mat& I1, const Mat& I2);
Mat getDiff(const Mat& I1, const Mat& I2);
int writeCSV1(const std::vector<double> array);
int writeCSV2(const std::vector<std::vector<double>> array);

int main(int argc, char* argv[]) {

    ostringstream stream;
    Mat img1;
    Mat img2;

    std::vector<std::vector<double>> array(132, std::vector<double>(101)); // DirectProjectionなら9列、propなら90列

    // ピクセルサイズの計算
    const double coef = 2.0;
    const double pixel_size = 13.4 / std::sqrt(3840 * 3840 + 2400 * 2400) * 25.4 / coef;
    std::cout << "camera pixel size:" << pixel_size << std::endl;

    // ディスプレイの設定
    const int display_image_px = 2400 * coef;
    const double display_area_size = display_image_px * pixel_size;
    cv::Mat display_image = cv::Mat::zeros(cv::Size(display_image_px, display_image_px), CV_8UC3);

    // タイルの設定
    const double tile_size_mm = display_area_size;  // タイルのサイズ（mm）
    const int image_size_px = display_image_px;      // タイルに貼る画像の解像度（lennaの画像を直接みるときは256px, 表示系の再現の場合はdisplay_image_pxと同じ）
    const double pixel_size_mm = tile_size_mm / image_size_px; // タイルに貼る画像のピクセルサイズ（mm）
    std::cout << "pixel size:" << pixel_size_mm << std::endl;

    //std::random_device seed_gen;
    int seedx = 0;
    int seedy = 1;
    std::mt19937 enginex(seedx);
    std::mt19937 enginey(seedy);

    std::uniform_real_distribution<double> dist(-display_area_size / 2.0, display_area_size / 2.0);

    int rand_size = 100;
    double* observer_x = (double*)malloc(sizeof(double) * rand_size);
    double* observer_y = (double*)malloc(sizeof(double) * rand_size);

    for (int i = 0; i < rand_size; i++) {
        observer_x[i] = dist(enginex);
        observer_y[i] = dist(enginey);
        //std::cout << "observer_x:" << observer_x[i] << ", observer_y:" << observer_y[i] << std::endl;
    }

    int index = 0;

    int nphs[20] = { 4, 5, 6, 8, 10, 12, 15, 20, 24, 25, 30, 40, 50, 60, 75, 100, 120, 125, 150, 200 };

    for (int idx_nph = 0; idx_nph < 20; idx_nph++) {

        int nph = nphs[idx_nph];

        for (int nzl = 40; nzl >= 40; nzl /= 2) {

            for (double subz = 256.0; subz <= 8192.0; subz *= 2) {

                for (int pt = 1; pt >= 1; pt--) {

                    cout << "NumPinhole:" << nph << ", NumZLevel:" << nzl << ", subjectZ:" << subz << ", pitchTimes:" << pt << endl;

                    // ピンホールの設定
                    const int num_pinhole_per_axis = nph;     // 各軸のピンホール数
                    //const double pinhole_spacing = display_area_size / num_pinhole_per_axis;    // ピンホール間の間隔（mm）
                    //const double pinhole_size = 0.15;        // ピンホールの一辺の長さ（mm）

                    // 観察者とタイルの位置
                    const double observer_z = -1500.0; // mm
                    //const double tile_pos = 4.0 * 160 / (double)nph; // mm (lennaの画像を直接みるときはsubzと同じ, 表示系の再現の場合はギャップと同じ（4.0 * 160 / (double)nph mm）)
                    const int image_resolution = static_cast<int>(floor(display_image_px / num_pinhole_per_axis)); // 画像の解像度（ピクセル）
                    const double subject_z = subz;

                    // 点群の設定
                    const int num_z_level = nzl;
                    const double Ddash = 250.0;
                    const double ptimes = pt;
                    const int NxNy = 50;

                    double sum_psnr = 0;
                    for (int nobs = 0; nobs < 100; nobs++) {

                        try {

                            //std::string filename_original = "./images/lenna/observe-image/real/gridSize" + std::to_string(num_pinhole_per_axis) + "/real-observe-lenna_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            std::string filename_original = "./images/lenna/observe-image/real-direct/real-direct-observe-lenna_2400px_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_original = "./images/lenna/observe-image/ideal/gridSize" + std::to_string(num_pinhole_per_axis) + "/ideal-observe-lenna-throughPinhole_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";

                            //std::string filename_compared = "./images/lenna/observe-image/v2-2/gridSize" + std::to_string(num_pinhole_per_axis) + "/v2-2-observe-lenna_NxNy" + std::to_string((int)round(50 * ptimes * 160.0 / (double)nph)) + "_Nz" + std::to_string(num_z_level) + "_pitchTimes" + std::to_string(static_cast<int>(ptimes)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "V:/images/lenna/observe-image/v2-3/gridSize" + std::to_string(num_pinhole_per_axis) + "/v2-3-observe-lenna_NumZLevel" + std::to_string(num_z_level) + "_Ddash" + std::to_string(static_cast<int>(Ddash)) + "_pitchTimes" + std::to_string(static_cast<int>(ptimes)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "V:/images/lenna/observe-image/v1-3/gridSize" + std::to_string(num_pinhole_per_axis) + "/v1-3-observe-lenna_NumZLevel" + std::to_string(num_z_level) + "_Ddash" + std::to_string(static_cast<int>(Ddash)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/v1-2/gridSize" + std::to_string(num_pinhole_per_axis) + "/v1-2-observe-lenna_NxNy" + std::to_string((int)round(50 * ptimes * 160.0 / (double)nph)) + "_Nz" + std::to_string(num_z_level) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/v1-3/gridSize" + std::to_string(num_pinhole_per_axis) + "/v1-3-observe-lenna_NxNy" + std::to_string(NxNy) + "_Nz" + std::to_string(num_z_level) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/v0/gridSize" + std::to_string(num_pinhole_per_axis) + "/v0-observe-lenna_NxNy" + std::to_string((int)round(50 * ptimes * 160.0 / (double)nph)) + "_Nz" + std::to_string(num_z_level) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/DP/gridSize" + std::to_string(num_pinhole_per_axis) + "/DP-observe-lenna-throughPinhole_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/v2-3/gridSize" + std::to_string(num_pinhole_per_axis) + "/v2-3-observe-lenna_interpolation=F_NxNy" + std::to_string((int)(NxNy * ptimes)) + "_Nz" + std::to_string(num_z_level) + "_pitchTimes" + std::to_string(static_cast<int>(ptimes)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/real/gridSize" + std::to_string(num_pinhole_per_axis) + "/real-observe-lenna_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            //std::string filename_compared = "./images/lenna/observe-image/real-direct/real-direct-observe-lenna_2400px_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
                            std::string filename_compared = "./images/lenna/observe-image/ideal_pixelSize" + std::to_string(static_cast<int>(pixel_size * 1000)) + "um/gridSize" + std::to_string(num_pinhole_per_axis) + "/ideal-observe-lenna-throughPinhole_imageResolution" + std::to_string(static_cast<int>(image_resolution)) + "_gridSize" + std::to_string(static_cast<int>(num_pinhole_per_axis)) + "_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";

                            img1 = readImage(filename_original);
                            img2 = readImage(filename_compared);

                            if (img1.empty()) {
                                cout << "Could not open or find the image: " << filename_original << endl;
                                return -1;
                            }
                            if (img2.empty()) {
                                cout << "Could not open or find the image: " << filename_compared << endl;
                                return -1;
                            }

                            double psnrValue = getPSNR(img1, img2);
                            //cout << "PSNR value is: " << psnrValue << " dB" << endl;
                            sum_psnr += psnrValue;
                            array[index][nobs] = psnrValue;

                            //Mat img_diff = getDiff(img1, img2);

                            //// img_diffをファイルとして保存して内容を確認する
                            //imwrite("img_diff.png", img_diff);

                            //// img_diffのコントラストを強調する
                            //Mat img_diff_enhanced;
                            //normalize(img_diff, img_diff_enhanced, 0, 255, NORM_MINMAX, CV_8U);

                            //cv::namedWindow("Difference image", cv::WINDOW_AUTOSIZE);
                            //cv::imshow("Difference image", img_diff_enhanced);

                            //cv::waitKey(0);
                            //cv::destroyWindow("Difference image");

                            //ostringstream stream;
                            stream.str("");
                            stream.clear(ostringstream::goodbit);

                            //stream << "./images/True_wpBase_v11-7_pdBase_diff_img_NumZLevel" << tt << "_imgPitch0.500.png";
                            //stream << "./images/True_pdBase_v11-7check_diff_img_NumZLevel" << tt << "_imgPitch1.000.png";
                            //stream << "./images/v11-6v_v11-7checkv_diff_img_z" << tt << ".png";
                            //stream << "./images/True_pdBase_v11-6_1_diff_img_NumZLevel" << tt << ".png";
                            //stream << "D:/Users/taw11/projects/realsense/observe-images/diff_images/True_pdBase_v11-6_NumZLevel25_view" << tt << ".png";
                            //stream << "./images/True_pdBase_v12-1_diff_img_NumZLevel" << tt << "_imgPitch1.000.png";
                            //stream << "./checkerboard/diff-images/vt-v1-3/real-scene_v1-3_diff_img_z" << tt << ".png";
                            //stream << "./checkerboard/diff-images/vt-v11-6/real-scene_v11-6_diff_img_z" << tt << ".png";
                            //stream << "C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/images/diff-images/DirectProjection_prop-v1-7_ImgDisplay_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << "_boxSize" << box_size << ".png";

                            //cv::String filename = stream.str();

                            //imwrite(filename, img_diff);

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

                    cout << "Average PSNR value is: " << sum_psnr / rand_size << " dB" << endl;
                    array[index++][100] = sum_psnr / rand_size;

                }
            }
        }
    }

    writeCSV2(array);

    return 0;
}

double getPSNR(const Mat& I1, const Mat& I2) {
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // Convert to float
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
    std::ofstream file("./images/lenna/observe-image/psnr-wA_real.csv");

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

int writeCSV2(const std::vector<std::vector<double>> array) {

    // 出力ファイルを開く
    std::ofstream file("./images/lenna/psnr-real-direct_ideal-pixelSize37um.csv");

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
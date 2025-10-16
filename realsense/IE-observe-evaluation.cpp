//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <random>
//#include <Windows.h>
//
//#define NOMINMAX
//
//using namespace cv;
//using namespace std;
//
//double getPSNR(const Mat& I1, const Mat& I2);
//Mat getDiff(const Mat& I1, const Mat& I2);
//int writeCSV1(const std::vector<double> array);
//int writeCSV2(const std::vector<std::vector<double>> array, int NxNy, int ptimes);
//cv::Mat direct_observe(cv::Mat input_image, double observer_x, double observer_y, double observer_z, double pixel_size, int display_image_px, double tile_size_mm, int image_size_px, double tile_pos);
//cv::Mat ideal_observe(cv::Mat input_image, int num_pinhole_per_axis, double pinhole_pitch, double focal_length, double observer_x, double observer_y, double observer_z, double pinhole_size, double display_image_size, double display_px_size, int display_image_px);
//
//int main(int argc, char* argv[]) {
//
//    cout << "IE-observe-evaluation" << endl;
//
//    ostringstream stream;
//
//    //std::vector<std::vector<double>> array(20, std::vector<double>(6)); // 横：subz, 縦：nph
//    //std::vector<std::vector<double>> array(10, std::vector<double>(6)); // 横：subz, 縦：nzl
//    //std::vector<std::vector<double>> array(1, std::vector<double>(8)); // 横：subz, 縦：ptimes
//    std::vector<double> array(8);
//
//    int array_index = 0;
//    int NxNy = 300;
//    int pt = 3;
//    int nzl = 60;
//
//    // ピクセルサイズの計算
//    const double coef = 1.0; // 倍率
//    const double display_px_size = 13.4 * 25.4 / std::sqrt(3840 * 3840 + 2400 * 2400) / coef; // ディスプレイのピクセルサイズ
//
//    // ディスプレイの設定
//    const int display_image_px = 2400 * coef;
//    const double display_image_size = display_image_px * display_px_size;
//    //int element_image_px = 60; // 要素画像の解像度
//    //double intv = (double)element_image_px; // 要素画像の間隔
//
//    // 観察者とタイルの設定
//    const double observer_z = -1000.0; // 観察者の位置
//    const double observer_z_min = 1000.0;
//    const int tile_px = 256; // タイルの解像度
//
//    // 表示系の設定
//
//    // ピンホールアレイと表示系のパラメータ(mm)
//    const int num_pinhole = 40;
//	//const int element_image_px = 60; // 要素画像の解像度
//	//double intv = (double)element_image_px; // 要素画像の間隔
//    //const double pinhole_pitch = element_image_px * display_px_size;    // ピンホール間の間隔（mm）(normalview:element_image_px * display_px_size)
//    //double display_area_size = display_image_size; //表示画像の大きさ
//    const double focal_length = observer_z_min / (3 * num_pinhole - 1); // レンズアレイの焦点距離
//    const double pinhole_pitch = 180.4 / num_pinhole; // observer_z_min / (focal_length + observer_z_min) * element_image_px * display_px_size;    // ピンホール間の間隔（mm）(wideview:observer_z_min / (focal_length + observer_z_min) * )
//    double intv = pinhole_pitch / display_px_size; // 要素画像の間隔 (wideview:(focal_length + observer_z_min) / observer_z_min * )
//    int element_image_px = static_cast<int>(floor(intv)); // 要素画像の解像度
//    double pinhole_array_size = pinhole_pitch * num_pinhole;   // 各軸方向のピンホールアレイのサイズ
//    double display_area_size = element_image_px * num_pinhole * display_px_size; //表示画像の大きさ (wideview:(focal_length + observer_z_min) / observer_z_min * )
//    int display_px = element_image_px * num_pinhole; // 表示画像の解像度
//    const double pinhole_size = display_px_size;        // ピンホールの一辺の長さ（mm）
//
//	cout << "focal length:" << focal_length << fixed << setprecision(3) << ", pinhole pitch:" << pinhole_pitch << fixed << setprecision(3) << ", pinhole size:" << pinhole_size << fixed << setprecision(5) << endl;
//	cout << "display area size:" << display_area_size << fixed << setprecision(3) << ", intv:" << intv << fixed << setprecision(3) << ", element image px:" << element_image_px << fixed << setprecision(3) << ", display px:" << display_px << fixed << setprecision(5) << ", display pixel pitch:" << display_px_size << fixed << setprecision(5) << endl;
//
//    // 画像を読み込む
//    std::string filename_tile = "./images/standard/grid_image.png";
//    cv::Mat tile_image = cv::imread(filename_tile);
//
//    if (tile_image.empty())
//    {
//        std::cout << "画像を開くことができませんでした。\n";
//        return -1;
//    }
//
//    //std::random_device seed_gen;
//    int seedx = 0;
//    int seedy = 1;
//    std::mt19937 enginex(seedx);
//    std::mt19937 enginey(seedy);
//
//    //// 領域2, 3用
//    //std::uniform_real_distribution<double> dist(-(focal_length + observer_z_min) / focal_length * pinhole_pitch * 0.5, (focal_length + observer_z_min) / focal_length * pinhole_pitch * 0.5);
//
//    //int rand_size = 100;
//    //double* observer_x = (double*)malloc(sizeof(double) * rand_size);
//    //double* observer_y = (double*)malloc(sizeof(double) * rand_size);
//
//    //// 除外する範囲の設定
//    //const double exclude_min = -pinhole_pitch * (observer_z_min / focal_length - (num_pinhole - 1)) * 0.5;
//    //const double exclude_max = pinhole_pitch * (observer_z_min / focal_length - (num_pinhole - 1)) * 0.5;
//
//    //for (int i = 0; i < rand_size; i++) {
//    //    // 除外範囲外の値が得られるまで乱数を生成
//    //    double x, y;
//    //    do {
//    //        x = dist(enginex);
//    //        y = dist(enginey);
//    //    } while (x >= exclude_min && x <= exclude_max && y >= exclude_min && y <= exclude_max);
//
//    //    observer_x[i] = x;
//    //    observer_y[i] = y;
//    //}
//    // 最後の要素は元のコードと同様に(0,0)にします
//    //observer_x[99] = 0;
//    //observer_y[99] = 0;
//
//    // 領域1用
//    std::uniform_real_distribution<double> dist(-pinhole_pitch * (observer_z_min / focal_length - (num_pinhole - 1)) * 0.5, pinhole_pitch * (observer_z_min / focal_length - (num_pinhole - 1)) * 0.5);
//
//    int rand_size = 100;
//    double* observer_x = (double*)malloc(sizeof(double) * rand_size);
//    double* observer_y = (double*)malloc(sizeof(double) * rand_size);
//
//    for (int i = 0; i < rand_size - 1; i++) {
//        observer_x[i] = dist(enginex);
//        observer_y[i] = dist(enginey);
//    }
//    observer_x[0] = 0;
//    observer_y[0] = 0;
//
//    int idx = 0;
//    for (double subz = 300.0; subz <= 1000.0; subz += 100) {
//
//        const double tile_size = display_area_size * (subz + observer_z_min) / observer_z_min; // 拡大の場合 * (subz + observer_z_min) / observer_z_minを付ける
//        const double tile_px_size = tile_size / tile_px; // タイルに貼る画像のピクセルサイズ（mm）
//
//        std::cout << "filename:" << filename_tile << ", NumPinhole:" << num_pinhole << ", NxNy:" << NxNy << ", NumZLevel:" << nzl << ", subjectZ:" << subz << ", pitchTimes:" << pt << std::endl;
//
//        // 理想表示用の表示画像の読み込み
//        //std::string filename_display_standard = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal/ideal-tileExpand_parrots_gridSize" + std::to_string(nph) + "_zi" + std::to_string((int)tile_pos) + ".png";
//        //cv::Mat display_standard_image = cv::imread(filename_display_standard);
//
//        //if (display_standard_image.empty())
//        //{
//        //    std::cout << "画像を開くことができませんでした。\n";
//        //    return -1;
//        //}
//
//        // 比較対象用の表示画像の読み込み
//        stream.str("");
//        //stream << "./images/reconstruction/IE-prop-wideview-v1-2/prop-wideview-v1-2-grid_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << tile_size << "_zi" << (int)subz << ".png";
//		stream << "D:/ForStudy/reconstruction/IE-prop-normal-v1/prop-normal-v1-grid_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << tile_size << "_zi" << (int)subz << ".png";
//        //stream << "D:/ForStudy/reconstruction/OpenGL-normal-v1/OpenGL-normal-v1-grid_f" << std::fixed << std::setprecision(4) << focal_length << "_subsize" << std::fixed << std::setprecision(2) << tile_size << "_zi" << (int)subz << "_2.png";
//		cout << "filename compared image:" << stream.str() << endl;
//        std::string filename_display_compared = stream.str();
//        cv::Mat display_compared_image = cv::imread(filename_display_compared);
//
//        if (display_compared_image.empty())
//        {
//            std::cout << "画像を開くことができませんでした。\n";
//            return -1;
//        }
//
//        double sum_psnr = 0;
//        double psnrValue;
//        for (int nobs = 0; nobs < rand_size; nobs++) {
//
//            std::cout << "nobs:" << nobs << std::endl;
//
//            try {
//
//                // 直接観察の観察画像
//                //cv::Mat resized_std_img = direct_observe(tile_image, observer_x[nobs], observer_y[nobs], observer_z, display_px_size, display_image_px, tile_size, tile_px, subz); // 解像度を2400 x 2400にする場合
//                cv::Mat standard_image = direct_observe(tile_image, observer_x[nobs], observer_y[nobs], observer_z, pinhole_pitch, num_pinhole, tile_size, tile_px, subz); // 解像度をnph x nphにする場合
//
//                // 理想表示の観察画像
//                //cv::Mat standard_image = ideal_observe(display_standard_image, nph, pinhole_pitch, focal_length, observer_x[nobs], observer_y[nobs], observer_z, pinhole_size, display_image_size, display_px_size, display_image_px);
//                //cv::Mat resized_std_img;
//                //cv::resize(standard_image, resized_std_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//
//                // 比較対象の観察画像
//                cv::Mat compared_image = ideal_observe(display_compared_image, num_pinhole, pinhole_pitch, focal_length, observer_x[nobs], observer_y[nobs], observer_z, pinhole_size, display_image_size, display_px_size, display_image_px);
//                //cv::Mat resized_cmpd_img;
//                //cv::resize(compared_image, resized_cmpd_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//
//    //            // 観察画像のウィンドウ表示
//    //            cv::imshow("images", standard_image);
//    //            cv::waitKey(0);
//
//    //            // 観察画像のウィンドウ表示
//    //            cv::imshow("images", compared_image);
//				//cv::waitKey(0);
//
//                /* 観察画像の保存 */
//
//                // 解像度2400 x 2400の場合
//                //std::string std_filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/grid/real-direct_2400_tileExpand/real-direct-observe_" + std::to_string(static_cast<int>(display_image_px)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                //std::string cmpd_filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/grid/prop-improve-v1_2400_tileExpand/prop-improve-v1-observe_" + std::to_string(static_cast<int>(display_image_px)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//				
//    //            // 解像度nph x nphの場合->400x400にリサイズして保存
//				//std::ostringstream filenameout_stream;
//				////filenameout_stream << "D:/ForStudy/observe-images/real-direct/real-direct-grid-observe_" << num_pinhole << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(abs(observer_z)) << "mm.png";
//				////std::string std_filenameout = filenameout_stream.str();
//    //            //cv::Mat resized_std_img;
//    //            //cv::resize(standard_image, resized_std_img, cv::Size(400, 400), 0, 0, cv::INTER_NEAREST);
//
//				//filenameout_stream.str("");
//    //            //filenameout_stream << "D:/ForStudy/observe-images/IE-prop-normal-v1/IE-prop-normal-v1-grid-observe_" << num_pinhole << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(abs(observer_z)) << "mm.png";
//    //            filenameout_stream << "D:/ForStudy/observe-images/OpenGL-normal-v1/OpenGL-normal-v1-grid-observe_" << num_pinhole << "px_zi" << static_cast<int>(subz) << "_xo" << static_cast<int>(observer_x[nobs] * 1000) << "um_yo" << static_cast<int>(observer_y[nobs] * 1000) << "um_zo" << static_cast<int>(abs(observer_z)) << "mm.png";
//    //            std::string cmpd_filenameout = filenameout_stream.str();
//    //            cv::Mat resized_cmpd_img;
//    //            cv::resize(compared_image, resized_cmpd_img, cv::Size(400, 400), 0, 0, cv::INTER_NEAREST);
//
//    //            //cv::imwrite(std_filenameout, resized_std_img);
//    //            //std::cout << "基準画像を" << std_filenameout << "として保存しました。\n";
//    //            cv::imwrite(cmpd_filenameout, resized_cmpd_img);
//    //            std::cout << "基準画像を" << cmpd_filenameout << "として保存しました。\n";
//
//                //cv::Mat observe_img = cv::Mat::zeros(cv::Size(2400, 2400), CV_8UC3);
//                //for (int i = 0; i < 2400; ++i) {
//
//                //    if (i < 1200) {
//                //        for (int j = 0; j < 2400; ++j) {
//                //            if (j < 1200) {
//                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_std_img.at<cv::Vec3b>(i, j)[0];
//                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_std_img.at<cv::Vec3b>(i, j)[1];
//                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_std_img.at<cv::Vec3b>(i, j)[2];
//                //            }
//                //            else {
//                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_cmpd_img.at<cv::Vec3b>(i, j)[0];
//                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_cmpd_img.at<cv::Vec3b>(i, j)[1];
//                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_cmpd_img.at<cv::Vec3b>(i, j)[2];
//                //            }
//                //        }
//                //    }
//                //    else {
//                //        for (int j = 0; j < 2400; ++j) {
//                //            if (j < 1200) {
//                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_cmpd_img.at<cv::Vec3b>(i, j)[0];
//                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_cmpd_img.at<cv::Vec3b>(i, j)[1];
//                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_cmpd_img.at<cv::Vec3b>(i, j)[2];
//                //            }
//                //            else {
//                //                observe_img.at<cv::Vec3b>(i, j)[0] = resized_std_img.at<cv::Vec3b>(i, j)[0];
//                //                observe_img.at<cv::Vec3b>(i, j)[1] = resized_std_img.at<cv::Vec3b>(i, j)[1];
//                //                observe_img.at<cv::Vec3b>(i, j)[2] = resized_std_img.at<cv::Vec3b>(i, j)[2];
//                //            }
//                //        }
//                //    }
//                //}
//                //std::string observe_filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/compare/prop-improve-v1_2400_tileNotExpand/DeltaFocal" + std::to_string(static_cast<int>(Df)) + "mm/prop-improve-v1-observe_" + std::to_string(static_cast<int>(display_image_px)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                //cv::imwrite(observe_filenameout, observe_img);
//                //std::cout << "比較用観察画像を" << observe_filenameout << "として保存しました。\n";
//
//                // 移植の整合性の確認（点検用）
//                //std::string filename_original = "C:/Users/taw11/EvacuatedStorage/observe-image/parrots/lensarray/ideal/gridSize" + std::to_string(nph) + "/ideal-observe-lenna_" + std::to_string(nph) + "px_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                //std::string filename_original = "C:/Users/taw11/EvacuatedStorage/observe-image/ICIP/prop-original-v1/prop-v1-observe-parrots_Nz" + std::to_string(nzl) + "_subjectZ" + std::to_string((int)subz) + "mm_obx" + std::to_string((int)(observer_x[nobs] * 1000)) + "um_oby" + std::to_string((int)(observer_y[nobs] * 1000)) + "um_obz" + std::to_string((int)abs(observer_z)) + "mm.png";
//                //cv::Mat original_image = cv::imread(filename_original);
//
//                //if (compared_image.empty())
//                //{
//                //    std::cout << "画像を開くことができませんでした。\n";
//                //    return -1;
//                //}
//
//                //psnrValue = getPSNR(resized_std_img, resized_cmpd_img); // 解像度を2400 x 2400に統一した場合
//                psnrValue = getPSNR(standard_image, compared_image); // 解像度をnph x nphに統一した場合
//                sum_psnr += psnrValue;
//                //cout << "PSNR value is: " << psnrValue << " dB" << endl;
//
//                //cv::Mat img_diff = getDiff(resized_std_img, compared_image);
//
//                //for (int i = 0; i < display_image_px; ++i) {
//                //    for (int j = 0; j < display_image_px; ++j) {
//                //        if (img_diff.at<cv::Vec3b>(i, j)[0] > 0 && img_diff.at<cv::Vec3b>(i, j)[1] > 0 && img_diff.at<cv::Vec3b>(i, j)[2] > 0) {
//                //            img_diff.at<cv::Vec3b>(i, j)[0] = 255;
//                //            img_diff.at<cv::Vec3b>(i, j)[1] = 255;
//                //            img_diff.at<cv::Vec3b>(i, j)[2] = 255;
//                //        }
//                //    }
//                //}
//
//                //// img_diffをファイルとして保存して内容を確認する
//                //cv::imwrite("img_diff_" + std::to_string(static_cast<int>(nobs)) + ".png", img_diff);
//
//                //ostringstream stream;
//                stream.str("");
//                stream.clear(ostringstream::goodbit);
//
//            }
//            catch (cv::Exception& e) {
//                cout << "OpenCV exception: " << e.what() << endl;
//                return -1;
//            }
//            catch (std::exception& e) {
//                cout << "Standard exception: " << e.what() << endl;
//                return -1;
//            }
//            catch (...) {
//                cout << "Unknown exception occurred." << endl;
//                return -1;
//            }
//        }
//
//        cout << "Average PSNR value is: " << sum_psnr / rand_size << " dB" << endl;
//        array[idx] = sum_psnr / rand_size;
//        idx++;
//    }
//    //writeCSV1(array);
//
//    MessageBeep(-1);
//    return 0;
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
//int writeCSV1(const std::vector<double> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/OpenGL-direct-normal-grid-area1_2.csv");
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
//int writeCSV2(const std::vector<std::vector<double>> array, int NxNy, int ptimes) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/PSNR/prop-improve/by_real/detail/psnr-parrots_real_prop-improve-NxNy" + std::to_string(NxNy) + "_pt" + std::to_string(ptimes) + "-nph_tileExpand.csv");
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
//cv::Mat direct_observe(cv::Mat input_image, double observer_x, double observer_y, double observer_z, double pixel_size, int display_image_px, double tile_size_mm, int image_size_px, double tile_pos) {
//
//    // ディスプレイの設定
//    const double display_area_size = display_image_px * pixel_size;
//
//    // タイルの設定
//    const double pixel_size_mm = tile_size_mm / image_size_px; // タイルに貼る画像のピクセルサイズ（mm）
//
//    // ピンホールの設定
//    const double pinhole_spacing = display_area_size / display_image_px;    // ピンホール間の間隔（mm）
//    const double pinhole_size = pinhole_spacing;        // ピンホールの一辺の長さ（mm）(lennaの画像を直接みるときはpinhole_spacingと同じ, 表示系の再現の場合は指定する)
//
//    // 観察者とタイルの位置
//    const int image_resolution = static_cast<int>(floor(display_image_px / display_image_px)); // 画像の解像度（ピクセル）
//
//    // ピンホールの位置を計算
//    std::vector<cv::Point2d> pinhole_positions;
//    double offset = -((display_image_px - 1) * pinhole_spacing) / 2.0;
//
//    for (int i = 0; i < display_image_px; ++i)
//    {
//        for (int j = 0; j < display_image_px; ++j)
//        {
//            double x = (offset + j * pinhole_spacing);
//            double y = (offset + i * pinhole_spacing);
//            pinhole_positions.emplace_back(x, y);
//        }
//    }
//
//    // 出力画像を作成
//    cv::Mat output_image(display_image_px, display_image_px, CV_8UC3);
//
//    // 各ピンホールについて観察される色を計算
//    int index = 0;
//    for (int i = 0; i < display_image_px; ++i)
//    {
//        for (int j = 0; j < display_image_px; ++j)
//        {
//            const auto& pinhole_pos = pinhole_positions[index++];
//
//            // ピンホールを通して見えるタイル上の領域を計算
//            // 視線を通してピンホールからタイルへの射影を計算
//            double scale = (tile_pos - observer_z) / (0.0 - observer_z); // z=observe_zからz=tile_posへの拡大率
//            double view_x = (pinhole_pos.x - observer_x) * scale + observer_x;
//            double view_y = (pinhole_pos.y - observer_y) * scale + observer_y;
//
//            // タイル上での可視領域（ピンホールの開口を考慮）
//            double pinhole_half = pinhole_size / 2.0;
//            double view_pinhole_half = pinhole_half * scale;
//
//            double visible_area_min_x = view_x - view_pinhole_half;
//            double visible_area_max_x = view_x + view_pinhole_half;
//            double visible_area_min_y = view_y - view_pinhole_half;
//            double visible_area_max_y = view_y + view_pinhole_half;
//
//            // タイルの範囲内に制限
//            visible_area_min_x = (std::max)(-tile_size_mm / 2.0, visible_area_min_x);
//            visible_area_max_x = (std::min)(tile_size_mm / 2.0, visible_area_max_x);
//            visible_area_min_y = (std::max)(-tile_size_mm / 2.0, visible_area_min_y);
//            visible_area_max_y = (std::min)(tile_size_mm / 2.0, visible_area_max_y);
//
//            // 可視領域がタイル内に存在しない場合
//            if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
//            {
//                // 黒色を設定
//                output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                continue;
//            }
//
//            // ピクセルインデックスの範囲を計算
//            int x_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_x + tile_size_mm / 2) / pixel_size_mm)));
//            int x_max_idx = (std::min)(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_x + tile_size_mm / 2) / pixel_size_mm)));
//            int y_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_y + tile_size_mm / 2) / pixel_size_mm)));
//            int y_max_idx = (std::min)(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_y + tile_size_mm / 2) / pixel_size_mm)));
//
//            //std::cout << "x min idx:" << x_min_idx << ", x max idx:" << x_max_idx << ", y min idx:" << y_min_idx << ", y max idx:" << y_max_idx << std::endl;
//
//            // 観察される色の加重平均を計算
//            cv::Vec3d weighted_color(0, 0, 0);
//            double total_area = 0.0;
//
//            for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
//            {
//                // ピクセルのx座標範囲を計算
//                double x_pixel_min = x_idx * pixel_size_mm - tile_size_mm / 2;
//                double x_pixel_max = (x_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                double x_overlap = (std::max)(0.0, (std::min)(visible_area_max_x, x_pixel_max) - (std::max)(visible_area_min_x, x_pixel_min));
//
//                if (x_overlap <= 0) continue;
//
//                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                {
//                    // ピクセルのy座標範囲を計算
//                    double y_pixel_min = y_idx * pixel_size_mm - tile_size_mm / 2;
//                    double y_pixel_max = (y_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                    double y_overlap = (std::max)(0.0, (std::min)(visible_area_max_y, y_pixel_max) - (std::max)(visible_area_min_y, y_pixel_min));
//
//                    if (y_overlap <= 0) continue;
//
//                    double overlap_area = x_overlap * y_overlap;
//                    total_area += overlap_area;
//
//                    // ピクセル値を取得し、加重平均に加算
//                    cv::Vec3b pixel = input_image.at<cv::Vec3b>(y_idx, x_idx);
//                    weighted_color += cv::Vec3d(pixel) * overlap_area;
//                }
//            }
//
//            cv::Vec3b final_color;
//            if (total_area > 0)
//            {
//                weighted_color /= total_area;
//                final_color = cv::Vec3b(
//                    static_cast<uchar>(weighted_color[0]),
//                    static_cast<uchar>(weighted_color[1]),
//                    static_cast<uchar>(weighted_color[2])
//                );
//            }
//            else
//            {
//                // 観察可能な領域がない場合は黒色
//                final_color = cv::Vec3b(0, 0, 0);
//            }
//
//            // 出力画像に色を設定
//            output_image.at<cv::Vec3b>(i, j) = final_color;
//        }
//    }
//
//    return output_image;
//}
//
//cv::Mat ideal_observe(cv::Mat input_image, int num_pinhole_per_axis, double pinhole_pitch, double focal_length, double observer_x, double observer_y, double observer_z, double pinhole_size, double display_image_size, double display_px_size, int display_image_px) {
//
//    // ピンホールの位置を計算
//    std::vector<cv::Point2d> pinhole_positions;
//    double offset = -((num_pinhole_per_axis - 1) * pinhole_pitch) / 2.0;
//
//    for (int i = 0; i < num_pinhole_per_axis; ++i)
//    {
//        for (int j = 0; j < num_pinhole_per_axis; ++j)
//        {
//            double x = (offset + j * pinhole_pitch);
//            double y = (offset + i * pinhole_pitch);
//            pinhole_positions.emplace_back(x, y);
//        }
//    }
//
//    cv::Mat output_image(num_pinhole_per_axis, num_pinhole_per_axis, CV_8UC3);    // 各ピンホールについて観察される色を計算
//
//    // 各ピンホールについて観察される色を計算
//    int index = 0;
//    for (int i = 0; i < num_pinhole_per_axis; ++i)
//    {
//        for (int j = 0; j < num_pinhole_per_axis; ++j)
//        {
//            const auto& pinhole_pos = pinhole_positions[index++];
//
//            // ピンホールを通して見えるタイル上の領域を計算
//            // 視線を通してピンホールからタイルへの射影を計算
//            double scale = (focal_length - observer_z) / (0.0 - observer_z); // z=observe_zからz=focal_lengthへの拡大率
//            double view_x = (pinhole_pos.x - observer_x) * scale + observer_x;
//            double view_y = (pinhole_pos.y - observer_y) * scale + observer_y;
//
//            // タイル上での可視領域（ピンホールの開口を考慮）
//            double pinhole_half = pinhole_size / 2.0;
//            double view_pinhole_half = pinhole_half * scale;
//
//            double visible_area_min_x = view_x - view_pinhole_half;
//            double visible_area_max_x = view_x + view_pinhole_half;
//            double visible_area_min_y = view_y - view_pinhole_half;
//            double visible_area_max_y = view_y + view_pinhole_half;
//
//            // タイルの範囲内に制限
//            visible_area_min_x = (std::max)(-display_image_size / 2.0, visible_area_min_x);
//            visible_area_max_x = (std::min)(display_image_size / 2.0, visible_area_max_x);
//            visible_area_min_y = (std::max)(-display_image_size / 2.0, visible_area_min_y);
//            visible_area_max_y = (std::min)(display_image_size / 2.0, visible_area_max_y);
//
//            // 可視領域がタイル内に存在しない場合
//            if (visible_area_min_x >= visible_area_max_x || visible_area_min_y >= visible_area_max_y)
//            {
//                // 黒色を設定
//                output_image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                continue;
//            }
//
//            // ピクセルインデックスの範囲を計算
//            int x_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_x + display_image_size / 2) / display_px_size)));
//            int x_max_idx = (std::min)(display_image_px - 1, static_cast<int>(std::floor((visible_area_max_x + display_image_size / 2) / display_px_size)));
//            int y_min_idx = (std::max)(0, static_cast<int>(std::floor((visible_area_min_y + display_image_size / 2) / display_px_size)));
//            int y_max_idx = (std::min)(display_image_px - 1, static_cast<int>(std::floor((visible_area_max_y + display_image_size / 2) / display_px_size)));
//
//            //std::cout << "x min idx:" << x_min_idx << ", x max idx:" << x_max_idx << ", y min idx:" << y_min_idx << ", y max idx:" << y_max_idx << std::endl;
//
//            // 観察される色の加重平均を計算
//            cv::Vec3d weighted_color(0, 0, 0);
//            double total_area = 0.0;
//
//            for (int x_idx = x_min_idx; x_idx <= x_max_idx; ++x_idx)
//            {
//                // ピクセルのx座標範囲を計算
//                double x_pixel_min = x_idx * display_px_size - display_image_size / 2;
//                double x_pixel_max = (x_idx + 1) * display_px_size - display_image_size / 2;
//
//                double x_overlap = (std::max)(0.0, (std::min)(visible_area_max_x, x_pixel_max) - (std::max)(visible_area_min_x, x_pixel_min));
//
//                if (x_overlap <= 0) continue;
//
//                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                {
//                    // ピクセルのy座標範囲を計算
//                    double y_pixel_min = y_idx * display_px_size - display_image_size / 2;
//                    double y_pixel_max = (y_idx + 1) * display_px_size - display_image_size / 2;
//
//                    double y_overlap = (std::max)(0.0, (std::min)(visible_area_max_y, y_pixel_max) - (std::max)(visible_area_min_y, y_pixel_min));
//
//                    if (y_overlap <= 0) continue;
//
//                    double overlap_area = x_overlap * y_overlap;
//                    total_area += overlap_area;
//
//                    // ピクセル値を取得し、加重平均に加算
//                    cv::Vec3b pixel = input_image.at<cv::Vec3b>(y_idx, x_idx);
//                    weighted_color += cv::Vec3d(pixel) * overlap_area;
//                }
//            }
//
//            cv::Vec3b final_color;
//            if (total_area > 0)
//            {
//                weighted_color /= total_area;
//                final_color = cv::Vec3b(
//                    static_cast<uchar>(weighted_color[0]),
//                    static_cast<uchar>(weighted_color[1]),
//                    static_cast<uchar>(weighted_color[2])
//                );
//            }
//            else
//            {
//                // 観察可能な領域がない場合は黒色
//                final_color = cv::Vec3b(0, 0, 0);
//            }
//
//            // 出力画像に色を設定
//            output_image.at<cv::Vec3b>(i, j) = final_color;
//        }
//    }
//
//    return output_image;
//}

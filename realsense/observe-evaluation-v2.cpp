//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <random>
//
//using namespace cv;
//using namespace std;
//
//double getPSNR(const Mat& I1, const Mat& I2);
//Mat getDiff(const Mat& I1, const Mat& I2);
//int writeCSV1(const std::vector<double> array);
//int writeCSV2(const std::vector<std::vector<double>> array);
//cv::Mat direct_observe(cv::Mat input_image, double observer_x, double observer_y, double observer_z, double pixel_size, int display_image_px, double tile_size_mm, int image_size_px, double tile_pos);
//cv::Mat ideal_observe(cv::Mat input_image, int num_pinhole_per_axis, double pinhole_pitch, double focal_length, double observer_x, double observer_y, double observer_z, double pinhole_size, double display_image_size, double display_px_size, int display_image_px);
//
//int main(int argc, char* argv[]) {
//
//    ostringstream stream;
//
//    //std::vector<std::vector<double>> array(20, std::vector<double>(6)); // 横：subz, 縦：nph
//    std::vector<std::vector<double>> array(10, std::vector<double>(6)); // 横：subz, 縦：nzl
//    //std::vector<std::vector<double>> array(5, std::vector<double>(6)); // 横：subz, 縦：ptimes
//
//    // ピクセルサイズの計算
//    const double coef = 1.0; // 倍率
//    const double display_px_size = 13.4 * 25.4 / std::sqrt(3840 * 3840 + 2400 * 2400) / coef; // ディスプレイのピクセルサイズ
//
//    // ディスプレイの設定
//    const int display_image_px = 2400 * coef;
//    const double display_image_size = display_image_px * display_px_size;
//
//    // タイルの設定
//    //const double tile_size = display_image_size;  // タイルのサイズ（mm）
//    const int tile_px = 256; // タイルの解像度
//    //const double tile_px_size = tile_size / tile_px; // タイルに貼る画像のピクセルサイズ（mm）
//
//    // 画像を読み込む
//    std::string filename_tile = "./images/standard/milkdrop.bmp";
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
//    std::uniform_real_distribution<double> dist(-display_image_size / 2.0, display_image_size / 2.0);
//
//    int rand_size = 100;
//    double* observer_x = (double*)malloc(sizeof(double) * rand_size);
//    double* observer_y = (double*)malloc(sizeof(double) * rand_size);
//
//    for (int i = 0; i < rand_size; i++) {
//        observer_x[i] = dist(enginex);
//        observer_y[i] = dist(enginey);
//    }
//
//
//    int array_index = 0;
//
//    int nphs[20] = { 4, 5, 6, 8, 10, 12, 15, 20, 24, 25, 30, 40, 50, 60, 75, 100, 120, 125, 150, 200 };
//
//    for (int idx_nph = 0; idx_nph < 1; idx_nph++) {
//
//        //int nph = nphs[idx_nph];
//        int nph = 40;
//        double pinhole_pitch = display_image_size / (double)nph;
//
//        int idx_nzl = 0;
//        for (int nzl = 10; nzl <= 100; nzl += 10) {
//
//            int idx_subz = 0;
//            for (double subz = 256.0; subz <= 8192.0; subz *= 2) {
//
//                int idx_pt = 0;
//                for (int pt = 1; pt <= 1; pt++) {
//
//                    cout << "NumPinhole:" << nph << ", NumZLevel:" << nzl << ", subjectZ:" << subz << ", pitchTimes:" << pt << endl;
//
//                    // ピンホールの設定
//                    const double pinhole_pitch = display_image_size / nph;    // ピンホール間の間隔（mm）
//                    const double pinhole_size = display_px_size;        // ピンホールの一辺の長さ（mm）
//
//                    // 観察者とタイルの設定
//                    const double observer_z = -1000.0; // 観察者の位置
//                    const double observer_z_min = 1000.0;
//                    const double focal_length = observer_z_min / (3 * nph - 1); // レンズアレイの焦点距離
//                    const double tile_pos = subz; // タイルの位置
//                    const double tile_size = display_image_size * (subz + observer_z_min) / observer_z_min; // 拡大の場合 * (subz + observer_z_min) / observer_z_minを付ける
//                    const double tile_px_size = tile_size / tile_px; // タイルに貼る画像のピクセルサイズ（mm）
//
//                    // 点群の設定
//                    const int num_z_level = nzl;
//                    const double Ddash = 200.0;
//                    const double ptimes = pt;
//
//                    // 理想表示用の表示画像の読み込み
//                    std::string filename_display_standard = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal/ideal-tileExpand_milkdrop_gridSize" + std::to_string(nph) + "_zi" + std::to_string((int)tile_pos) + ".png";
//                    cv::Mat display_standard_image = cv::imread(filename_display_standard);
//
//                    if (display_standard_image.empty())
//                    {
//                        std::cout << "画像を開くことができませんでした。\n";
//                        return -1;
//                    }
//
//                    // 比較対象用の表示画像の読み込み
//                    //std::string filename_display_compared = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ICIP-DP/DP-milkdrop_tileExpand_gridSize" +  std::to_string(nph) + "_zi" + std::to_string((int)subz) + ".png"; // 直接投影
//                    std::string filename_display_compared = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ICIP-prop-original-v1/prop-v1-milkdrop_tileExpand_Nz" + std::to_string(nzl) + "_subjectZ" + std::to_string((int)subz) + ".png"; // 提案手法（オリジナル版）
//                    //std::string filename_display_compared = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ICIP-prop-improve-v1/prop-improve-v1-milkdrop_tileExpand_Nz" + std::to_string(nzl) + "_N" + std::to_string((int)ptimes) + "_subjectZ" + std::to_string((int)subz) + ".png"; //  提案手法(完成版)
//                    //std::string filename_display_compared = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal/ideal-tileExpand_milkdrop_gridSize" + std::to_string(nph) + "_zi" + std::to_string((int)tile_pos) + ".png"; // 理想表示
//                    cv::Mat display_compared_image = cv::imread(filename_display_compared);
//
//                    if (display_compared_image.empty())
//                    {
//                        std::cout << "画像を開くことができませんでした。\n";
//                        return -1;
//                    }
//
//                    double sum_psnr = 0;
//                    double psnrValue;
//                    for (int nobs = 0; nobs < rand_size; nobs++) {
//
//                        std::cout << "nobs:" << nobs << std::endl;
//
//                        try {
//
//                            // 直接観察の観察画像
//                            //cv::Mat resized_std_img = direct_observe(tile_image, observer_x[nobs], observer_y[nobs], observer_z, display_px_size, display_image_px, tile_size, tile_px, subz); // 解像度を2400 x 2400にする場合
//                            //cv::Mat standard_image = direct_observe(tile_image, observer_x[nobs], observer_y[nobs], observer_z, pinhole_pitch, nph, tile_size, tile_px, subz); // 解像度をnph x nphにする場合
//                            
//                            // 理想表示の観察画像
//                            cv::Mat standard_image = ideal_observe(display_standard_image, nph, pinhole_pitch, focal_length, observer_x[nobs], observer_y[nobs], observer_z, pinhole_size, display_image_size, display_px_size, display_image_px);
//                            //cv::Mat resized_std_img;
//                            //cv::resize(standard_image, resized_std_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//
//                            // 比較対象の観察画像
//                            cv::Mat compared_image = ideal_observe(display_compared_image, nph, pinhole_pitch, focal_length, observer_x[nobs], observer_y[nobs], observer_z, pinhole_size, display_image_size, display_px_size, display_image_px);
//                            //cv::Mat resized_cmpd_img;
//                            //cv::resize(compared_image, resized_cmpd_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//
//                            // 観察画像のウィンドウ表示
//                            //cv::imshow("images", resized_cmpd_img);
//
//                            //if (cv::waitKey(10) == 27)  // ESCキーで終了
//                            //{
//                            //    cv::destroyAllWindows;
//                            //    break;
//                            //}
//
//                            // 観察画像の出力
//                            // 解像度2400 x 2400の場合
//                            //std::string filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/ICIP/real-direct_2400_tileExpand/real-direct-observe_" + std::to_string(static_cast<int>(display_image_px)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                            // 解像度nph x nphの場合
//                            //std::string filenameout = "C:/Users/taw11/EvacuatedStorage/observe-image/ICIP/real-direct_nph/real-direct-observe_" + std::to_string(static_cast<int>(nph)) + "px_zi" + std::to_string(static_cast<int>(subz)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                            //cv::Mat resized_std_img;
//                            //cv::resize(standard_image, resized_std_img, cv::Size(2400, 2400), 0, 0, cv::INTER_NEAREST);
//                            //cv::imwrite(filenameout, resized_std_img);
//                            //std::cout << "出力画像を" << filenameout << "として保存しました。\n";
//
//                            // 移植の整合性の確認（点検用）
//                            //std::string filename_original = "C:/Users/taw11/EvacuatedStorage/observe-image/milkdrop/lensarray/ideal/gridSize" + std::to_string(nph) + "/ideal-observe-lenna_" + std::to_string(nph) + "px_zi" + std::to_string(static_cast<int>(subject_z)) + "_xo" + std::to_string(static_cast<int>(observer_x[nobs] * 1000)) + "um_yo" + std::to_string(static_cast<int>(observer_y[nobs] * 1000)) + "um_zo" + std::to_string(static_cast<int>(abs(observer_z))) + ".png";
//                            //std::string filename_original = "C:/Users/taw11/EvacuatedStorage/observe-image/ICIP/prop-original-v1/prop-v1-observe-milkdrop_Nz" + std::to_string(nzl) + "_subjectZ" + std::to_string((int)subz) + "mm_obx" + std::to_string((int)(observer_x[nobs] * 1000)) + "um_oby" + std::to_string((int)(observer_y[nobs] * 1000)) + "um_obz" + std::to_string((int)abs(observer_z)) + "mm.png";
//                            //cv::Mat original_image = cv::imread(filename_original);
//
//                            //if (compared_image.empty())
//                            //{
//                            //    std::cout << "画像を開くことができませんでした。\n";
//                            //    return -1;
//                            //}
//
//                            //double psnrValue = getPSNR(resized_std_img, resized_cmpd_img); // 解像度を2400 x 2400に統一した場合
//                            psnrValue = getPSNR(standard_image, compared_image); // 解像度をnph x nphに統一した場合
//                            sum_psnr += psnrValue;
//                            //cout << "PSNR value is: " << psnrValue << " dB" << endl;
//
//                            //cv::Mat img_diff = getDiff(resized_std_img, compared_image);
//
//                            //for (int i = 0; i < display_image_px; ++i) {
//                            //    for (int j = 0; j < display_image_px; ++j) {
//                            //        if (img_diff.at<cv::Vec3b>(i, j)[0] > 0 && img_diff.at<cv::Vec3b>(i, j)[1] > 0 && img_diff.at<cv::Vec3b>(i, j)[2] > 0) {
//                            //            img_diff.at<cv::Vec3b>(i, j)[0] = 255;
//                            //            img_diff.at<cv::Vec3b>(i, j)[1] = 255;
//                            //            img_diff.at<cv::Vec3b>(i, j)[2] = 255;
//                            //        }
//                            //    }
//                            //}
//
//                            //// img_diffをファイルとして保存して内容を確認する
//                            //cv::imwrite("img_diff_" + std::to_string(static_cast<int>(nobs)) + ".png", img_diff);
//
//                            //ostringstream stream;
//                            stream.str("");
//                            stream.clear(ostringstream::goodbit);
//
//                        }
//                        catch (cv::Exception& e) {
//                            cout << "OpenCV exception: " << e.what() << endl;
//                            return -1;
//                        }
//                        catch (std::exception& e) {
//                            cout << "Standard exception: " << e.what() << endl;
//                            return -1;
//                        }
//                        catch (...) {
//                            cout << "Unknown exception occurred." << endl;
//                            return -1;
//                        }
//                    }
//
//                    cout << "Average PSNR value is: " << sum_psnr / rand_size << " dB" << endl;
//                    //array[idx_nph][idx_subz] = sum_psnr / rand_size; // nph x subzの時
//                    array[idx_nzl][idx_subz] = sum_psnr / rand_size; // nzl x subzの時
//                    //array[idx_pt++][idx_subz] = sum_psnr / rand_size; // pt x subzの時
//                }
//                idx_subz++;
//            }
//            idx_nzl++;
//        }
//    }
//
//    writeCSV2(array);
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
//    std::ofstream file("./images/lenna/observe-image/psnr-wA_real.csv");
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
//int writeCSV2(const std::vector<std::vector<double>> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./numbers/PSNR/prop-original/psnr-milkdrop_ideal_prop-original-v1_nph_tileExpand.csv");
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
//            visible_area_min_x = std::max(-tile_size_mm / 2.0, visible_area_min_x);
//            visible_area_max_x = std::min(tile_size_mm / 2.0, visible_area_max_x);
//            visible_area_min_y = std::max(-tile_size_mm / 2.0, visible_area_min_y);
//            visible_area_max_y = std::min(tile_size_mm / 2.0, visible_area_max_y);
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
//            int x_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_x + tile_size_mm / 2) / pixel_size_mm)));
//            int x_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_x + tile_size_mm / 2) / pixel_size_mm)));
//            int y_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_y + tile_size_mm / 2) / pixel_size_mm)));
//            int y_max_idx = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max_y + tile_size_mm / 2) / pixel_size_mm)));
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
//                double x_overlap = std::max(0.0, std::min(visible_area_max_x, x_pixel_max) - std::max(visible_area_min_x, x_pixel_min));
//
//                if (x_overlap <= 0) continue;
//
//                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                {
//                    // ピクセルのy座標範囲を計算
//                    double y_pixel_min = y_idx * pixel_size_mm - tile_size_mm / 2;
//                    double y_pixel_max = (y_idx + 1) * pixel_size_mm - tile_size_mm / 2;
//
//                    double y_overlap = std::max(0.0, std::min(visible_area_max_y, y_pixel_max) - std::max(visible_area_min_y, y_pixel_min));
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
//            visible_area_min_x = std::max(-display_image_size / 2.0, visible_area_min_x);
//            visible_area_max_x = std::min(display_image_size / 2.0, visible_area_max_x);
//            visible_area_min_y = std::max(-display_image_size / 2.0, visible_area_min_y);
//            visible_area_max_y = std::min(display_image_size / 2.0, visible_area_max_y);
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
//            int x_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_x + display_image_size / 2) / display_px_size)));
//            int x_max_idx = std::min(display_image_px - 1, static_cast<int>(std::floor((visible_area_max_x + display_image_size / 2) / display_px_size)));
//            int y_min_idx = std::max(0, static_cast<int>(std::floor((visible_area_min_y + display_image_size / 2) / display_px_size)));
//            int y_max_idx = std::min(display_image_px - 1, static_cast<int>(std::floor((visible_area_max_y + display_image_size / 2) / display_px_size)));
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
//                double x_overlap = std::max(0.0, std::min(visible_area_max_x, x_pixel_max) - std::max(visible_area_min_x, x_pixel_min));
//
//                if (x_overlap <= 0) continue;
//
//                for (int y_idx = y_min_idx; y_idx <= y_max_idx; ++y_idx)
//                {
//                    // ピクセルのy座標範囲を計算
//                    double y_pixel_min = y_idx * display_px_size - display_image_size / 2;
//                    double y_pixel_max = (y_idx + 1) * display_px_size - display_image_size / 2;
//
//                    double y_overlap = std::max(0.0, std::min(visible_area_max_y, y_pixel_max) - std::max(visible_area_min_y, y_pixel_min));
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

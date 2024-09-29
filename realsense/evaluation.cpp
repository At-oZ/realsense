//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <string>
//#include <fstream>
//
//using namespace cv;
//using namespace std;
//
//double getPSNR(const Mat& I1, const Mat& I2);
//Mat getDiff(const Mat& I1, const Mat& I2);
//Mat readImage(const std::string& filename);
//int writeCSV(const std::vector<std::vector<double>> array);
//
//int main(int argc, char* argv[]) {
//
//    ostringstream stream;
//    Mat img1;
//    Mat img2;
//    std::string filename_original, filename_compared;
//    std::vector<std::vector<double>> array(36, std::vector<double>(2));
//
//    int row = 0;
//    int num_z_level = 40; // イメージプレーンの層数
//    double ptimes = 1; // イメージプレーンの画素ピッチの係数(1でディスプレイの画素ピッチと同じ)
//    int box_size = 1; // 探索範囲
//    int Ddash = 250;
//    for (int tt = 0; tt < 1; tt += 50) {
//
//        cout << "z:" << tt << endl;
//
//        try {
//
//            //std::cout << "Please enter the name of original image file: ";
//            //std::getline(std::cin, filename_original);
//            //std::cout << "Please enter the name of compared image file: ";
//            //std::getline(std::cin, filename_compared);
//
//            stream.str("");
//            stream.clear(ostringstream::goodbit);
//
//            //stream << "C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/images/DirectProjection_output-v3_pdBase_img_display_z0_g4_wp1.2_pd0.075_D100.png";
//            //stream << "./images/v2-observe-DirectProjection_pdBase_img_display_z0_g4_wp1.2_pd0.075_D100.png";
//            stream << "./images/lenna/observe-lenna-throughPinhole_zi512.png";
//
//            filename_original = stream.str();
//
//            stream.str("");
//            stream.clear(ostringstream::goodbit);
//
//            //stream << "C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/images/prop-v1-7_ImgDisplay_output-v3_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << "_boxSize" << box_size << ".png";
//            //stream << "./images/v2-observe_prop-v1-10_ImgDisplay_NumZLevel65_Ddash250_pitchTimes0.20_boxSize4.png";
//            stream << "./images/lenna/weightedAveraging_observe-lenna-throughDisplay_zi512.png";
//
//            filename_compared = stream.str();
//
//            img1 = readImage(filename_original);
//            img2 = readImage(filename_compared);
//
//            if (img1.empty()) {
//                cout << "Could not open or find the image: " << filename_original << endl;
//                return -1;
//            }
//            if (img2.empty()) {
//                cout << "Could not open or find the image: " << filename_compared << endl;
//                return -1;
//            }
//
//            double psnrValue = getPSNR(img1, img2);
//            cout << "PSNR value is: " << psnrValue << " dB" << endl;
//
//            //Mat img_diff = getDiff(img1, img2);
//
//            //// img_diffをファイルとして保存して内容を確認する
//            //imwrite("img_diff.png", img_diff);
//
//            //// img_diffのコントラストを強調する
//            //Mat img_diff_enhanced;
//            //normalize(img_diff, img_diff_enhanced, 0, 255, NORM_MINMAX, CV_8U);
//
//            //cv::namedWindow("Difference image", cv::WINDOW_AUTOSIZE);
//            //cv::imshow("Difference image", img_diff_enhanced);
//
//            //cv::waitKey(0);
//            //cv::destroyWindow("Difference image");
//
//            //ostringstream stream;
//            stream.str("");
//            stream.clear(ostringstream::goodbit);
//
//            //stream << "./images/True_wpBase_v11-7_pdBase_diff_img_NumZLevel" << tt << "_imgPitch0.500.png";
//            //stream << "./images/True_pdBase_v11-7check_diff_img_NumZLevel" << tt << "_imgPitch1.000.png";
//            //stream << "./images/v11-6v_v11-7checkv_diff_img_z" << tt << ".png";
//            //stream << "./images/True_pdBase_v11-6_1_diff_img_NumZLevel" << tt << ".png";
//            //stream << "D:/Users/taw11/projects/realsense/observe-images/diff_images/True_pdBase_v11-6_NumZLevel25_view" << tt << ".png";
//            //stream << "./images/True_pdBase_v12-1_diff_img_NumZLevel" << tt << "_imgPitch1.000.png";
//            //stream << "./checkerboard/diff-images/vt-v1-3/real-scene_v1-3_diff_img_z" << tt << ".png";
//            //stream << "./checkerboard/diff-images/vt-v11-6/real-scene_v11-6_diff_img_z" << tt << ".png";
//            //stream << "C:/Users/taw11/source/repos2/At-oZ/realsense/realsense/images/diff-images/DirectProjection_prop-v1-7_ImgDisplay_NumZLevel" << num_z_level << "_Ddash" << Ddash << "_pitchTimes" << fixed << setprecision(2) << ptimes << "_boxSize" << box_size << ".png";
//
//            //cv::String filename = stream.str();
//
//            //imwrite(filename, img_diff);
//
//            array[row][0] = (double)tt;
//            array[row][1] = psnrValue;
//            row++;
//
//        }
//        catch (cv::Exception& e) {
//            cout << "OpenCV exception: " << e.what() << endl;
//            return -1;
//        }
//        catch (std::exception& e) {
//            cout << "Standard exception: " << e.what() << endl;
//            return -1;
//        }
//        catch (...) {
//            cout << "Unknown exception occurred." << endl;
//            return -1;
//        }
//
//    }
//
//    //writeCSV(array);
//
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
//Mat readImage(const std::string& filename) {
//    std::ifstream file(filename, std::ios::binary);
//    if (!file) {
//        return Mat();
//    }
//
//    std::vector<uchar> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//    return imdecode(buffer, IMREAD_COLOR);
//}
//
//int writeCSV(const std::vector<std::vector<double>> array) {
//
//    // 出力ファイルを開く
//    std::ofstream file("./checkerboard/diff-images/vt-v1-3/output.csv");
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
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;

double getPSNR(const Mat& I1, const Mat& I2);
Mat getDiff(const Mat& I1, const Mat& I2);

Mat readImage(const std::string& filename);

int main(int argc, char* argv[]) {

    ostringstream stream;
    Mat img1;
    Mat img2;
    std::string filename_original, filename_compared;

    for (int tt = 10; tt <= 320; tt *= 2) {

        cout << "z:" << tt << endl;

        try {

            //std::cout << "Please enter the name of original image file: ";
            //std::getline(std::cin, filename_original);
            //std::cout << "Please enter the name of compared image file: ";
            //std::getline(std::cin, filename_compared);

            stream.str("");
            stream.clear(ostringstream::goodbit);

            //stream << "./images/v11-7v_ImgDisplay_NumZLevel500_z" << tt << "_min_d100_max_d2650_pitchTimes1.000_boxSize1.png";
            //stream << "./images/Truev_pdBase_img_display_z" << tt << "_g4_wp1.2_pd0.075_D100.png";
            stream << "./images/True/True_pdBase_img_display_g4_wp1.2_pd0.075_D100.png";
            //stream << "./images/v11-7check_ImgDisplay_NumZLevel" << tt << "_min_d250_max_d2650_pitchTimes1.000_boxSize1.png";
            //stream << "./images/v11-6v_ImgDisplay_NumZLevel500_z" << tt << "_Ddash100_pitchTimes1.000_boxSize1.png";
            //stream << "D:/Users/taw11/projects/realsense/observe-images/True_pdBase/img_view" << tt << ".png";

            filename_original = stream.str();

            stream.str("");
            stream.clear(ostringstream::goodbit);

            //stream << "./images/v11-7v_ImgDisplay_NumZLevel500_z" << tt << "_min_d100_max_d2650_pitchTimes1.000_boxSize1.png";
            //stream << "./images/v11-7check_ImgDisplay_NumZLevel" << tt << "_min_d250_max_d2650_pitchTimes1.000_boxSize1.png";
            //stream << "./images/v11-7_wpBase_ImgDisplay_NumZLevel" << tt << "_min_d250_max_d2650_pitchTimes0.500_boxSize3.png";
            stream << "./images/v11-6_ImgDisplay_NumZLevel" << tt << "_Ddash250_pitchTimes1.00_boxSize1.png";
            //stream << "./images/v11-7checkv_ImgDisplay_NumZLevel500_z" << tt << "_min_d100_max_d2650_pitchTimes1.000_boxSize1.png";
            //stream << "./images/v11-6v_ImgDisplay_NumZLevel500_z" << tt << "_Ddash100_pitchTimes1.000_boxSize1.png";
            //stream << "D:/Users/taw11/projects/realsense/observe-images/v11-7_NumZLevel25/img_view" << tt << ".png";

            filename_compared = stream.str();

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
            cout << "PSNR value is: " << psnrValue << " dB" << endl;

            Mat img_diff = getDiff(img1, img2);

            // img_diffをファイルとして保存して内容を確認する
            imwrite("img_diff.png", img_diff);

            // img_diffのコントラストを強調する
            Mat img_diff_enhanced;
            normalize(img_diff, img_diff_enhanced, 0, 255, NORM_MINMAX, CV_8U);

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
            stream << "./images/True_pdBase_v11-6_diff_img_z" << tt << ".png";
            //stream << "D:/Users/taw11/projects/realsense/observe-images/diff_images/True_pdBase_v11-7_NumZLevel25_view" << tt << ".png";
            cv::String filename = stream.str();

            imwrite(filename, img_diff);

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

Mat readImage(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return Mat();
    }

    std::vector<uchar> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return imdecode(buffer, IMREAD_COLOR);
}

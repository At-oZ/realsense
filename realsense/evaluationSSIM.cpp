//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <string>
//#include <fstream>
//
//using namespace cv;
//using namespace std;
//
//double calculateSSIM(const Mat& img1, const Mat& img2);
//Mat readImage(const std::string& filename);
//
//int main(int argc, char* argv[])
//{
//    ostringstream stream;
//    Mat img1;
//    Mat img2;
//    std::string filename_original, filename_compared;
//
//    std::cout << "Please enter the name of original image file: ";
//    std::getline(std::cin, filename_original);
//    std::cout << "Please enter the name of compared image file: ";
//    std::getline(std::cin, filename_compared);
//
//    //if (argc != 3)
//    //{
//    //    cout << "Usage: " << argv[0] << " <image1> <image2>" << endl;
//    //    return -1;
//    //}
//
//    img1 = readImage(filename_original);
//    img2 = readImage(filename_compared);
//
//    if (img1.empty()) {
//        cout << "Could not open or find the image: " << filename_original << endl;
//        return -1;
//    }
//    if (img2.empty()) {
//        cout << "Could not open or find the image: " << filename_compared << endl;
//        return -1;
//    }
//
//    double ssim = calculateSSIM(img1, img2);
//    cout << "SSIM: " << ssim << endl;
//
//    return 0;
//}
//
//double calculateSSIM(const Mat& img1, const Mat& img2)
//{
//    const double C1 = 6.5025, C2 = 58.5225;
//    int d = CV_32F;
//
//    Mat I1, I2;
//    img1.convertTo(I1, d);
//    img2.convertTo(I2, d);
//
//    Mat I2_2 = I2.mul(I2);        // I2^2
//    Mat I1_2 = I1.mul(I1);        // I1^2
//    Mat I1_I2 = I1.mul(I2);       // I1 * I2
//
//    Mat mu1, mu2;
//    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
//    GaussianBlur(I2, mu2, Size(11, 11), 1.5);
//
//    Mat mu1_2 = mu1.mul(mu1);
//    Mat mu2_2 = mu2.mul(mu2);
//    Mat mu1_mu2 = mu1.mul(mu2);
//
//    Mat sigma1_2, sigma2_2, sigma12;
//
//    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
//    sigma1_2 -= mu1_2;
//
//    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
//    sigma2_2 -= mu2_2;
//
//    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
//    sigma12 -= mu1_mu2;
//
//    Mat t1, t2, t3;
//
//    t1 = 2 * mu1_mu2 + C1;
//    t2 = 2 * sigma12 + C2;
//    t3 = t1.mul(t2);
//
//    t1 = mu1_2 + mu2_2 + C1;
//    t2 = sigma1_2 + sigma2_2 + C2;
//    t1 = t1.mul(t2);
//
//    Mat ssim_map;
//    divide(t3, t1, ssim_map);
//    Scalar mssim = mean(ssim_map);
//
//    return mssim[0];
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

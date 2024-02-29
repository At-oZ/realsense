//#include <opencv2/opencv.hpp>
//#include <vector>
//#include <random>
//#include <cmath>
//
//int main() {
//    const int width = 100;
//    const int height = 100;
//    const int depthA = 3;
//    const int depthB = 6;
//
//    // A, Cの配列を準備
//    std::vector<std::vector<std::vector<double>>> A(depthA, std::vector<std::vector<double>>(height, std::vector<double>(width)));
//    std::vector<std::vector<std::vector<double>>> C(depthA, std::vector<std::vector<double>>(height, std::vector<double>(width)));
//
//    // Bの配列を準備
//    std::vector<std::vector<std::vector<double>>> B(height, std::vector<std::vector<double>>(width, std::vector<double>(depthB)));
//
//    // 乱数生成器
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0.0, 1.0);
//
//    // 配列Aに値を代入
//    for (int z = 0; z < depthA; ++z) {
//        for (int y = 0; y < height; ++y) {
//            for (int x = 0; x < width; ++x) {
//                A[z][y][x] = (z * 100 + y) / 100.0 / (x + 1);
//            }
//        }
//    }
//
//    // 配列Bに乱数を格納
//    for (auto& row : B) {
//        for (auto& cell : row) {
//            std::generate(cell.begin(), cell.end(), [&] { return dis(gen); });
//        }
//    }
//
//    // ここでは具体的な最近傍探索の実装は省略します。代わりに、単純化のためにBの乱数値を直接Cにコピーします。
//    // 実際には、ここで最近傍探索アルゴリズムを実装する必要があります。
//
//    // OpenCVを使って配列Cを画像として表示
//    cv::Mat image(height, width, CV_8UC3); // 8ビット符号なし整数型、3チャンネルの画像
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
//            for (int z = 0; z < depthA; ++z) {
//                // 値を0-255の範囲に正規化
//                pixel[z] = static_cast<unsigned char>(std::min(std::max(int(C[z][y][x] * 255), 0), 255));
//            }
//        }
//    }
//
//    cv::imshow("Image from Array C", image);
//    cv::waitKey(0);
//
//    return 0;
//}

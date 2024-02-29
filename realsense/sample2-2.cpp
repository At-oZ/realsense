//#include <iostream>
//#include <vector>
//#include <cmath>
//#include <limits>
//#include <random>
//#include <opencv2/opencv.hpp>
//
//// ユークリッド距離を計算する関数
//double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
//    double sum = 0.0;
//    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
//        sum += (a[i] - b[i]) * (a[i] - b[i]);
//    }
//    return std::sqrt(sum);
//}
//
//int main() {
//    const int width = 100, height = 100, depthA = 3, depthB = 6;
//    std::vector<std::vector<std::vector<double>>> A(depthA, std::vector<std::vector<double>>(height, std::vector<double>(width)));
//    std::vector<std::vector<std::vector<double>>> B(height, std::vector<std::vector<double>>(width, std::vector<double>(depthB)));
//    //std::vector<std::vector<std::vector<double>>> C(depthA, std::vector<std::vector<double>>(height, std::vector<double>(width)));
//
//    // CV_8UC3は、8ビットの符号なし整数で3チャンネル（例：RGB）を意味する
//    cv::Mat C(height, width, CV_8UC3);
//
//    // 乱数生成器の初期化
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0.0, 1.0);
//
//    // 配列Aの初期化
//    for (int z = 0; z < depthA; ++z) {
//        for (int y = 0; y < height; ++y) {
//            for (int x = 0; x < width; ++x) {
//                A[z][y][x] = (z * 100 + y) / 100.0 / (x + 1); // 0除算を避ける
//            }
//        }
//    }
//
//    // 配列Bの初期化（乱数を使用）
//    for (auto& row : B) {
//        for (auto& cell : row) {
//            std::generate(cell.begin(), cell.end(), [&]() { return dis(gen); });
//        }
//    }
//
//    auto clip = [](double value) -> unsigned char {
//        return static_cast<unsigned char>(std::max(0.0, std::min(255.0, value * 255.0)));
//    };
//
//    // 最近傍探索と配列Cの更新
//    for (int y = 0; y < height; ++y) {
//        for (int x = 0; x < width; ++x) {
//            double minDistance = std::numeric_limits<double>::max();
//            std::vector<double> nearestPoint(depthB);
//            for (const auto& row : B) {
//                for (const auto& cell : row) {
//                    double distance = euclideanDistance({ (double)x / width, (double)y / height }, { cell[0], cell[1] });
//                    if (distance < minDistance) {
//                        minDistance = distance;
//                        nearestPoint = cell;
//                    }
//                }
//            }
//            // 配列Cに最近傍点の値を代入
//            for (int z = 0; z < depthA; ++z) {
//                C.at<cv::Vec3b>(y, x) = cv::Vec3b(clip(nearestPoint[z + 3]), clip(nearestPoint[z + 3]), clip(nearestPoint[z + 3])); // インデックス3から5まで
//            }
//        }
//    }
//
//    //// OpenCVを使って配列Cを画像として表示
//    //cv::Mat image(height, width, CV_8UC3);
//    //for (int y = 0; y < height; ++y) {
//    //    for (int x = 0; x < width; ++x) {
//    //        cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
//    //        for (int z = 0; z < depthA; ++z) {
//    //            pixel[z] = static_cast<unsigned char>(std::min(std::max(int(C[z][y][x] * 255), 0), 255));
//    //        }
//    //    }
//    //}
//
//    cv::imshow("Image from Array C", C);
//    cv::waitKey(0);
//
//    return 0;
//}

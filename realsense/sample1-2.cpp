//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <chrono>
//
//int main() {
//    // 配列（画像）の次元とサイズを定義
//    int width = 2261, height = 2261;
//
//    // CV_8UC3は、8ビットの符号なし整数で3チャンネル（例：RGB）を意味する
//    cv::Mat A(height, width, CV_8UC3);
//
//    // 計測開始時間
//    auto start = std::chrono::high_resolution_clock::now();
//
//    // 指定された計算で各ピクセルの値を設定
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            A.at<cv::Vec3b>(i, j)[0] = 100; // 赤
//            A.at<cv::Vec3b>(i, j)[1] = 100; // 緑
//            A.at<cv::Vec3b>(i, j)[2] = 100; // 青
//        }
//    }
//
//    // 計測終了時間
//    auto finish = std::chrono::high_resolution_clock::now();
//
//    // 経過時間を出力
//    std::chrono::duration<double> elapsed = finish - start;
//    std::cout << "Time to fill the array: " << elapsed.count() << " seconds" << std::endl;
//
//    // 画像を表示
//    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//    cv::imshow("Display window", A);
//
//    // 任意のキーが押されるまで待つ
//    cv::waitKey(0);
//
//    return 0;
//}

//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <chrono>
//
//int main() {
//    // 画像のサイズとチャンネル数を定義
//    int width = 2261, height = 2261, channels = 3;
//
//    // 計測開始時間
//    auto start = std::chrono::high_resolution_clock::now();
//
//    // すべての要素が0（黒）の画像を作成
//    // CV_8UC3は、8ビットの符号なし整数で3チャンネル（例：RGB）を意味する
//    cv::Mat A(height, width, CV_8UC3, cv::Scalar(100, 100, 100));
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

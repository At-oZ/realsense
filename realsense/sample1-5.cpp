//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <chrono>
//#include <thread>
//#include <vector>
//
//void fillSection(cv::Mat& A, int startRow, int endRow, int width) {
//    for (int i = startRow; i < endRow; i++) {
//        for (int j = 0; j < width; j++) {
//            A.at<cv::Vec3b>(i, j)[0] = 100; // ê‘
//            A.at<cv::Vec3b>(i, j)[1] = 100; // óŒ
//            A.at<cv::Vec3b>(i, j)[2] = 100; // ê¬
//        }
//    }
//}
//
//int main() {
//    int width = 2261, height = 2261;
//    cv::Mat A(height, width, CV_8UC3);
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    const int numThreads = 20;
//    std::vector<std::thread> threads;
//    int rowsPerThread = height / numThreads;
//
//    for (int i = 0; i < numThreads; i++) {
//        int startRow = i * rowsPerThread;
//        int endRow = (i == numThreads - 1) ? height : (i + 1) * rowsPerThread;
//        threads.emplace_back(fillSection, std::ref(A), startRow, endRow, width);
//    }
//
//    for (auto& t : threads) {
//        t.join();
//    }
//
//    auto finish = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> elapsed = finish - start;
//    std::cout << "Time to fill the array: " << elapsed.count() << " seconds" << std::endl;
//
//    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
//    cv::imshow("Display window", A);
//    cv::waitKey(0);
//
//    return 0;
//}

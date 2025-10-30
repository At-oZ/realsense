#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

// g++ resize_image.cpp -o resize_image $(pkg-config --cflags --libs opencv4)

/**
 * @brief 画像をN倍にニアレストネイバー法でリサイズする
 * @param inputImage 入力画像
 * @param N スケーリングファクタ (例: 2.0, 4.0)
 * @return リサイズされた画像
 */
cv::Mat resizeNearestNeighbor(const cv::Mat& inputImage, double N) {
    cv::Mat outputImage;

    // cv::resize を使用
    cv::resize(
        inputImage,         // 入力画像
        outputImage,        // 出力画像
        cv::Size(0, 0),     // 出力サイズ (0,0 は fx, fy から計算)
        N,                  // X軸のスケールファクタ (fx)
        N,                  // Y軸のスケールファクタ (fy)
        cv::INTER_NEAREST   // 補間方法: ニアレストネイバー
    );

    return outputImage;
}

int main() {
    // 1. スケールファクタNを定義
    const double SCALE_FACTOR = 2.5; // ここをN倍したい値に変更 (例: 2.0, 3.5 など)

    // 2. 入力画像を読み込む
    std::string inputFilename = "D:/ForStudy/observe-images/real-direct/subz_300mm/real-direct-mandrill-observe_128px_zi300_xo-2264um_yo-107802um_zo-2233mm.png";
    cv::Mat image = cv::imread(inputFilename);

    if (image.empty()) {
        std::cerr << "エラー: 画像ファイル '" << inputFilename << "' を読み込めませんでした。" << std::endl;
        return -1;
    }

    std::cout << "読み込み成功: " << inputFilename
        << " (" << image.cols << "x" << image.rows << ")" << std::endl;

    // 3. リサイズ関数を呼び出し
    cv::Mat resizedImage = resizeNearestNeighbor(image, SCALE_FACTOR);

    std::cout << "リサイズ成功 (N=" << SCALE_FACTOR << "): "
        << resizedImage.cols << "x" << resizedImage.rows << std::endl;

    // 4. 結果を保存
    std::string outputFilename = "./images/output_nearest.jpg";
    if (cv::imwrite(outputFilename, resizedImage)) {
        std::cout << "結果を '" << outputFilename << "' に保存しました。" << std::endl;
    }
    else {
        std::cerr << "エラー: 結果の保存に失敗しました。" << std::endl;
        return -1;
    }

    /*
    // 5. (オプション) 結果を表示
    cv::imshow("Input Image", image);
    cv::imshow("Resized (Nearest Neighbor)", resizedImage);
    cv::waitKey(0); // 何かキーが押されるまで待機
    */

    return 0;
}
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <string>
//
//int main(int argc, char* argv[]) {
//    // コマンドライン引数で画像パスを受け取る
//    if (argc < 2) {
//        std::cerr << "./images/standard/grid_image.png" << std::endl;
//        std::cerr << "使用方法: " << argv[0] << " <画像ファイルパス>" << std::endl;
//        return -1;
//    }
//
//    std::string imagePath = argv[1];
//
//    // 画像を読み込む
//    cv::Mat image = cv::imread(imagePath);
//
//    // 画像が正しく読み込めたか確認
//    if (image.empty()) {
//        std::cerr << "画像を開くことができませんでした: " << imagePath << std::endl;
//        return -1;
//    }
//
//    // ウィンドウに画像を表示
//    std::string windowName = "表示画像";
//    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
//    cv::imshow(windowName, image);
//
//    // ユーザーがキーを押すまで待機
//    std::cout << "画像を表示しています。キーを押すと終了します。" << std::endl;
//    cv::waitKey(0);
//
//    // ウィンドウを閉じる
//    cv::destroyAllWindows();
//
//    return 0;
//}

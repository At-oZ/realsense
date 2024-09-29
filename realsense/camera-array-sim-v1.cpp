//// ピンホールカメラアレイを用いて表示画像を作成するプログラム
///*
//    中央に配置した一つのピンホールカメラで撮影
//*/
//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//int main() {
//    // タイルの設定
//    const double tileSize = 100.0; // タイルのサイズ（mm）
//    const int tileResolution = 256; // タイルの解像度（ピクセル）
//    const double tileZ = 100.0; // タイルの位置（z座標、mm）
//
//    // カメラの設定
//    const double focalLength = 4.0; // 焦点距離（mm）
//    const double sensorSize = 1.25; // センサーサイズ（mm）
//    const int imageResolution = 16; // 画像の解像度（ピクセル）
//
//    // ピクセルサイズの計算
//    const double pixelSize = sensorSize / imageResolution;
//
//    // 画像中心からのオフセット計算用
//    const double halfSensorSize = sensorSize / 2.0;
//
//    // タイル画像の読み込み
//    cv::Mat tileImage = cv::imread("./images/lenna.bmp");
//    if (tileImage.empty()) {
//        std::cerr << "タイル画像が見つかりません。" << std::endl;
//        return -1;
//    }
//    cv::resize(tileImage, tileImage, cv::Size(tileResolution, tileResolution));
//
//    // 出力画像の初期化
//    cv::Mat outputImage(imageResolution, imageResolution, CV_8UC3, cv::Scalar(0, 0, 0));
//
//    // 各ピクセルについて計算
//    for (int i = 0; i < imageResolution; ++i) {
//        for (int j = 0; j < imageResolution; ++j) {
//            // センサー平面上のピクセル位置（カメラ座標系）
//            double x_sensor = (j + 0.5) * pixelSize - halfSensorSize;
//            double y_sensor = (i + 0.5) * pixelSize - halfSensorSize;
//            double z_sensor = -focalLength; // 画像面はピンホールの後方にある
//
//            // レイの方向ベクトル（ピンホールからピクセルへ）
//            cv::Vec3d rayDir(x_sensor, y_sensor, z_sensor);
//
//            // レイを正規化
//            rayDir = rayDir / cv::norm(rayDir);
//
//            // レイとタイルの交点計算
//            double t = (tileZ - 0.0) / rayDir[2]; // ピンホールは原点にある
//            double x_intersect = rayDir[0] * t;
//            double y_intersect = rayDir[1] * t;
//
//            // タイル内に交点があるか確認
//            if (std::abs(x_intersect) <= tileSize / 2.0 && std::abs(y_intersect) <= tileSize / 2.0) {
//                // タイル上のテクスチャ座標に変換
//                int u = static_cast<int>((x_intersect + tileSize / 2.0) / tileSize * tileResolution);
//                int v = static_cast<int>((y_intersect + tileSize / 2.0) / tileSize * tileResolution);
//
//                // 座標が画像範囲内か確認
//                u = std::min(std::max(u, 0), tileResolution - 1);
//                v = std::min(std::max(v, 0), tileResolution - 1);
//
//                // 出力画像に色を設定
//                outputImage.at<cv::Vec3b>(i, j) = tileImage.at<cv::Vec3b>(tileResolution - v - 1, u);
//            }
//        }
//    }
//
//    // 画像の表示と保存
//    //cv::resize(outputImage, outputImage, cv::Size(256, 256), 0, 0, cv::INTER_NEAREST);
//    //cv::imshow("Captured Image", outputImage);
//    cv::imwrite("captured_image.png", outputImage);
//    cv::waitKey(0);
//
//    return 0;
//}

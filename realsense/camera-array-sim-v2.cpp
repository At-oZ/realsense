//// ピンホールカメラアレイを用いて表示画像を作成するプログラム
///*
//    v1との差分:ピンホールカメラをグリッド状に複数配置（最近傍により画素を埋める）
//*/
//
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//
//int main() {
//    // タイルの設定
//    const double tileSize = 170.0; // タイルのサイズ（mm）
//    const int tileResolution = 256; // タイルの解像度（ピクセル）
//    const double tileZ = 1024.0; // タイルの位置（z座標、mm）
//
//    // カメラの設定
//    const double focalLength = 4.0; // 焦点距離（mm）
//    const double sensorSize = 1.25; // センサーサイズ（mm）
//    const int imageResolution = 16; // 画像の解像度（ピクセル）
//
//    // 画像中心からのオフセット計算用
//    const double halfSensorSize = sensorSize / 2.0;
//
//    const int displayImageSize = 2261;
//    const double displayAreaSize = 170.0;
//    cv::Mat displayImage = cv::Mat::zeros(cv::Size(displayImageSize, displayImageSize), CV_8UC3);
//
//    // ピクセルサイズの計算
//    const double pixelSize = displayAreaSize / (double)displayImageSize;
//    std::cout << "pixel size:" << pixelSize << std::endl;
//    const double intv = sensorSize / pixelSize;
//
//    // タイル画像の読み込み
//    cv::Mat tileImage = cv::imread("./images/lenna.bmp");
//    if (tileImage.empty()) {
//        std::cerr << "タイル画像が見つかりません。" << std::endl;
//        return -1;
//    }
//    cv::resize(tileImage, tileImage, cv::Size(tileResolution, tileResolution));
//
//    // カメラのグリッド設定
//    const int gridSize = 136; // 4×4のグリッド
//    const double gridSpacing = 1.25; // グリッド間隔（mm）
//    const double gridOriginOffset = -((gridSize - 1) * gridSpacing) / 2.0; // グリッドの中心を原点に合わせるためのオフセット
//
//    // カメラ位置の計算
//    std::vector<double> cameraPositions;
//    for (int i = 0; i < gridSize; ++i) {
//        double pos = (gridOriginOffset + i * gridSpacing);
//        cameraPositions.push_back(pos);
//    }
//
//    // 各カメラについて計算
//    int cameraIndex = 0;
//    int camXidx = 0;
//    int camYidx = 135;
//    int startu, startv;
//    for (double camY : cameraPositions) {
//
//        startv = static_cast<int>(camYidx * intv);
//
//        for (double camX : cameraPositions) {
//
//            startu = static_cast<int>(camXidx * intv);
//
//            // 出力画像の初期化
//            cv::Mat outputImage(imageResolution, imageResolution, CV_8UC3, cv::Scalar(0, 0, 0));
//
//            // 各ピクセルについて計算
//            for (int i = 0; i < imageResolution; ++i) {
//                for (int j = 0; j < imageResolution; ++j) {
//                    // センサー平面上のピクセル位置（カメラ座標系）
//                    double x_sensor = -((j + 0.5) * pixelSize - halfSensorSize);
//                    double y_sensor = (i + 0.5) * pixelSize - halfSensorSize; // y軸の正負を反転
//                    double z_sensor = -focalLength; // 画像面はピンホールの後方にある
//
//                    // レイの方向ベクトル（ピンホールからピクセルへ）
//                    cv::Vec3d rayDir(x_sensor, y_sensor, z_sensor);
//
//                    // レイを正規化
//                    rayDir = rayDir / cv::norm(rayDir);
//
//                    // レイとタイルの交点計算
//                    double t = (tileZ - 0.0) / rayDir[2]; // ピンホールのz座標は0
//
//                    // カメラ位置を考慮した交点計算
//                    double x_intersect = camX + rayDir[0] * t;
//                    double y_intersect = camY + rayDir[1] * t;
//
//                    // タイル内に交点があるか確認
//                    if (std::abs(x_intersect) <= tileSize / 2.0 && std::abs(y_intersect) <= tileSize / 2.0) {
//                        // タイル上のテクスチャ座標に変換
//                        int u = static_cast<int>((x_intersect + tileSize / 2.0) / tileSize * tileResolution);
//                        int v = static_cast<int>((y_intersect + tileSize / 2.0) / tileSize * tileResolution);
//
//                        // 座標が画像範囲内か確認
//                        u = std::min(std::max(u, 0), tileResolution - 1);
//                        v = std::min(std::max(v, 0), tileResolution - 1);
//
//                        // 出力画像に色を設定
//                        outputImage.at<cv::Vec3b>(i, j) = tileImage.at<cv::Vec3b>(tileResolution - v - 1, u);
//                    }
//                }
//            }
//
//            for (int i = 0; i < imageResolution; ++i) {
//                for (int j = 0; j < imageResolution; ++j) {
//                    displayImage.at<cv::Vec3b>(startv + i, startu + j) = outputImage.at<cv::Vec3b>(i, j);
//                }
//            }
//
//            cameraIndex++;
//            camXidx++;
//        }
//        camYidx--;
//        camXidx = 0;
//    }
//    camYidx = 0;
//
//    // 画像の保存
//    std::string filename = "./images/lenna/v2-captured_image_zi" + std::to_string(static_cast<int>(tileZ)) + ".png";
//    cv::imwrite(filename, displayImage);
//
//    return 0;
//}

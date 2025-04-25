//// ピンホールカメラアレイを用いて表示画像を作成するプログラム
///*
//    ideal_v2からの派生
//*/
//
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//#include <Windows.h>
//
//int main() {
//
//
//    std::cout << "ICIP-ideal-v1" << std::endl;
//
//    int nphs[20] = { 4, 5, 6, 8, 10, 12, 15, 20, 24, 25, 30, 40, 50, 60, 75, 100, 120, 125, 150, 200 };
//
//    std::string WINNAME = "image";
//    cv::namedWindow(WINNAME);
//    HWND window = FindWindow(NULL, L"image");
//    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
//    SetWindowPos(window, NULL, 2560, 0, 3840, 2420, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
//
//    cv::Mat img_window = cv::Mat::zeros(cv::Size(3840, 2400), CV_8UC3);
//    img_window = cv::Scalar::all(255);
//
//    for (int idx_nph = 11; idx_nph < 12; idx_nph++) {
//
//        int nph = nphs[idx_nph];
//
//        std::cout << "number of pins:" << nph << std::endl;
//
//        double zo_min = 1000.0; // 最短視認距離(mm)
//        // ピクセルサイズの計算
//        const double coef = 1.0;
//        const double pixelSize = 13.4 / std::sqrt(3840 * 3840 + 2400 * 2400) * 25.4 / coef;
//        std::cout << "camera pixel size:" << pixelSize << std::endl;
//
//        // ディスプレイの設定
//        const int displayImageSize = 2400 * coef;
//        const double displayAreaSize = displayImageSize * pixelSize;
//        cv::Mat displayImage = cv::Mat::zeros(cv::Size(displayImageSize, displayImageSize), CV_8UC3);
//
//        // タイルの設定
//        //const double tileSize = displayAreaSize; // タイルのサイズ（mm）
//        const int tileResolution = 256; // タイルの解像度（ピクセル）
//        //const double tileZ = 1024.0; // タイルの位置（z座標、mm）
//        //const double tilePixelSize = tileSize / tileResolution; // ピクセルサイズ(mm)
//
//        // カメラのグリッド設定
//        const int gridSize = nph; // グリッドサイズ
//        const double gridSpacing = displayAreaSize / gridSize; // グリッド間隔（mm）
//        const double gridOriginOffset = -((gridSize - 1) * gridSpacing) / 2.0; // グリッドの中心を原点に合わせるためのオフセット
//
//        // カメラの設定
//        const double focalLength = 8.2; // レンズアレイの焦点距離 zo_min / (3 * nph - 1)
//        const double sensorSize = gridSpacing; // センサーサイズ（mm）
//        const int imageResolution = static_cast<int>(floor(displayImageSize / gridSize)); // 要素画像の解像度（ピクセル）
//
//        // 画像中心からのオフセット計算用
//        const double halfSensorSize = sensorSize / 2.0;
//        const double intv = sensorSize / pixelSize;
//
//        // タイル画像の読み込み
//        cv::Mat tileImage = cv::imread("./images/standard/Lenna.bmp");
//        if (tileImage.empty()) {
//            std::cerr << "タイル画像が見つかりません。" << std::endl;
//            return -1;
//        }
//        cv::resize(tileImage, tileImage, cv::Size(tileResolution, tileResolution));
//
//        for (double tileZ = 451.0; tileZ <= 451.0; tileZ *= 2) {
//
//            std::cout << "tileZ:" << tileZ << std::endl;
//
//            double tileSize = 230.0; // 拡大の場合 * (tileZ + zo_min) / zo_minを付ける
//            double tilePixelSize = tileSize / tileResolution;
//
//            // カメラ位置の計算
//            std::vector<cv::Point2d> cameraPositions;
//            for (int i = 0; i < gridSize; ++i)
//            {
//                for (int j = 0; j < gridSize; ++j)
//                {
//                    double camPosX = (gridOriginOffset + j * gridSpacing);
//                    double camPosY = (gridOriginOffset + i * gridSpacing);
//                    cameraPositions.emplace_back(camPosX, camPosY);
//                }
//            }
//
//            // ピクセルの位置を計算
//            std::vector<cv::Point2d> pixelPositions;
//            double offset = -((imageResolution - 1) * pixelSize) / 2.0;
//
//            for (int i = 0; i < imageResolution; ++i)
//            {
//                for (int j = 0; j < imageResolution; ++j)
//                {
//                    double x = (offset + j * pixelSize);  // x軸はそのまま
//                    double y = (offset + i * pixelSize);  // y軸の反転
//                    pixelPositions.emplace_back(x, y);
//                }
//            }
//
//            // 各カメラについて計算
//            int cameraIndex = 0;
//            int startu, startv;
//            for (int m = 0; m < gridSize; ++m) {
//
//                //startv = static_cast<int>(round(m * intv));
//
//                for (int n = 0; n < gridSize; ++n) {
//
//                    //startu = static_cast<int>(round(n * intv));
//
//                    // 出力画像の初期化
//                    cv::Mat outputImage(imageResolution, imageResolution, CV_8UC3, cv::Scalar(0, 0, 0));
//
//                    int pixelIndex = 0;
//                    const auto& cameraPos = cameraPositions[cameraIndex++];
//                    startu = static_cast<int>(std::round((cameraPos.x + displayAreaSize / 2 - halfSensorSize) / pixelSize));
//                    startv = static_cast<int>(std::round((cameraPos.y + displayAreaSize / 2 - halfSensorSize) / pixelSize));
//
//                    //std::cout << "cameraPos:" << cameraPos << std::endl;
//
//                    // 各ピクセルについて計算
//                    for (int i = 0; i < imageResolution; ++i) {
//                        for (int j = 0; j < imageResolution; ++j) {
//
//                            const auto& pixelPos = pixelPositions[pixelIndex++];
//                            //std::cout << "pixelPos:" << pixelPos << std::endl;
//
//                            // 1ピクセルから見えるタイル上の領域を計算
//                            // 投影線を通してピクセルからタイルへの射影を計算
//                            double scale = tileZ / focalLength; // z=0からz=tileZへの拡大率
//                            double viewX = pixelPos.x * scale + cameraPos.x;
//                            double viewY = pixelPos.y * scale + cameraPos.y;
//                            //std::cout << "viewX:" << viewX << ", viewY:" << viewY << std::endl;
//
//                            // タイル上での可視領域（ピンホールの開口を考慮）
//                            double pixelHalf = pixelSize / 2.0;
//                            double viewPixelHalf = pixelHalf * scale;
//
//                            double visibleAreaMinX = viewX - viewPixelHalf;
//                            double visibleAreaMaxX = viewX + viewPixelHalf;
//                            double visibleAreaMinY = viewY - viewPixelHalf;
//                            double visibleAreaMaxY = viewY + viewPixelHalf;
//
//                            // タイルの範囲内に制限
//                            visibleAreaMinX = (std::max)(-tileSize / 2.0, visibleAreaMinX);
//                            visibleAreaMaxX = (std::min)(tileSize / 2.0, visibleAreaMaxX);
//                            visibleAreaMinY = (std::max)(-tileSize / 2.0, visibleAreaMinY);
//                            visibleAreaMaxY = (std::min)(tileSize / 2.0, visibleAreaMaxY);
//                            //std::cout << "visible are min X:" << visibleAreaMinX << ", visible are max X:" << visibleAreaMaxX << ", visible are min Y:" << visibleAreaMinY << ", visible are max Y:" << visibleAreaMaxY << std::endl;
//                            //std::cout << "diff X:" << visibleAreaMaxX - visibleAreaMinX << ", diff Y:" << visibleAreaMaxY - visibleAreaMinY << std::endl;
//
//                            // 可視領域がタイル内に存在しない場合
//                            if (visibleAreaMinX >= visibleAreaMaxX || visibleAreaMinY >= visibleAreaMaxY)
//                            {
//                                // 黒色を設定
//                                outputImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                                continue;
//                            }
//
//                            // ピクセルインデックスの範囲を計算
//                            int xMinIdx = (std::max)(0, static_cast<int>(std::floor((visibleAreaMinX + tileSize / 2) / tilePixelSize)));
//                            int xMaxIdx = (std::min)(tileResolution - 1, static_cast<int>(std::floor((visibleAreaMaxX + tileSize / 2) / tilePixelSize)));
//                            int yMinIdx = (std::max)(0, static_cast<int>(std::floor((visibleAreaMinY + tileSize / 2) / tilePixelSize)));
//                            int yMaxIdx = (std::min)(tileResolution - 1, static_cast<int>(std::floor((visibleAreaMaxY + tileSize / 2) / tilePixelSize)));
//                            //std::cout << "X min index:" << xMinIdx << ", X max index:" << xMaxIdx << ", Y min index:" << yMinIdx << ", Y max index:" << yMaxIdx << std::endl;
//
//                            // 観察される色の加重平均を計算
//                            cv::Vec3d weighted_color(0, 0, 0);
//                            double total_area = 0.0;
//
//                            for (int xIdx = xMinIdx; xIdx <= xMaxIdx; ++xIdx)
//                            {
//                                // ピクセルのx座標範囲を計算
//                                double xPixelMin = xIdx * tilePixelSize - tileSize / 2;
//                                double xPixelMax = (xIdx + 1) * tilePixelSize - tileSize / 2;
//
//                                double xOverlap = (std::max)(0.0, (std::min)(visibleAreaMaxX, xPixelMax) - (std::max)(visibleAreaMinX, xPixelMin));
//                                //std::cout << "visible are max X:" << visibleAreaMaxX << ", x pixel max:" << xPixelMax << ", visible are min X:" << visibleAreaMinX << ", x pixel min:" << xPixelMin << std::endl;
//                                //std::cout << "x overlap:" << xOverlap << std::endl;
//
//                                if (xOverlap <= 0) continue;
//
//                                for (int yIdx = yMinIdx; yIdx <= yMaxIdx; ++yIdx)
//                                {
//                                    // ピクセルのy座標範囲を計算
//                                    double yPixelMin = yIdx * tilePixelSize - tileSize / 2;
//                                    double yPixelMax = (yIdx + 1) * tilePixelSize - tileSize / 2;
//
//                                    double yOverlap = (std::max)(0.0, (std::min)(visibleAreaMaxY, yPixelMax) - (std::max)(visibleAreaMinY, yPixelMin));
//                                    //std::cout << "visible are max Y:" << visibleAreaMaxY << ", y pixel max:" << yPixelMax << ", visible are min Y:" << visibleAreaMinY << ", y pixel min:" << yPixelMin << std::endl;
//                                    //std::cout << "y overlap:" << yOverlap << std::endl;
//
//                                    if (yOverlap <= 0) continue;
//
//                                    double overlap_area = xOverlap * yOverlap;
//                                    total_area += overlap_area;
//
//                                    // ピクセル値を取得し、加重平均に加算
//                                    cv::Vec3b pixel = tileImage.at<cv::Vec3b>(yIdx, xIdx);
//                                    weighted_color += cv::Vec3d(pixel) * overlap_area;
//                                }
//                            }
//
//                            cv::Vec3b final_color;
//                            if (total_area > 0)
//                            {
//                                weighted_color /= total_area;
//                                final_color = cv::Vec3b(
//                                    static_cast<uchar>(weighted_color[0]),
//                                    static_cast<uchar>(weighted_color[1]),
//                                    static_cast<uchar>(weighted_color[2])
//                                );
//                            }
//                            else
//                            {
//                                // 観察可能な領域がない場合は黒色
//                                final_color = cv::Vec3b(0, 0, 0);
//                            }
//
//                            // 出力画像に色を設定
//                            outputImage.at<cv::Vec3b>(i, j) = final_color;
//
//                        }
//                    }
//
//                    for (int i = 0; i < imageResolution; ++i) {
//                        for (int j = 0; j < imageResolution; ++j) {
//                            displayImage.at<cv::Vec3b>(startv + i, startu + j) = outputImage.at<cv::Vec3b>(i, j);
//                        }
//                    }
//                }
//            }
//
//
//            int shift_y = 895;
//            for (int i = shift_y; i < 2400; ++i) {
//                for (int j = 720; j < 3120; ++j) {
//                    img_window.at<cv::Vec3b>(i - shift_y, j)[0] = displayImage.at<cv::Vec3b>(i, j - 720)[0];
//                    img_window.at<cv::Vec3b>(i - shift_y, j)[1] = displayImage.at<cv::Vec3b>(i, j - 720)[1];
//                    img_window.at<cv::Vec3b>(i - shift_y, j)[2] = displayImage.at<cv::Vec3b>(i, j - 720)[2];
//                }
//            }
//
//            cv::imshow(WINNAME, img_window);
//            cv::waitKey(0);
//
//            //// 画像の保存
//            //std::string filename = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal/ideal-tileExpand_aerial_gridSize" + std::to_string(static_cast<int>(gridSize)) + "_zi" + std::to_string(static_cast<int>(tileZ)) + ".png";
//
//            //cv::imwrite(filename, displayImage);
//
//        }
//
//    }
//
//    return 0;
//}

//// ピンホールカメラアレイを用いて表示画像を作成するプログラム
///*
//    ideal_v2からの派生
//    タイルを3分割
//*/
//
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//
//int main() {
//
//    int nphs[20] = { 4, 5, 6, 8, 10, 12, 15, 20, 24, 25, 30, 40, 50, 60, 75, 100, 120, 125, 150, 200 };
//
//    for (int idx_nph = 0; idx_nph < 20; idx_nph++) {
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
//        const double focalLength = zo_min / (3 * nph - 1); // レンズアレイの焦点距離
//        const double sensorSize = gridSpacing; // センサーサイズ（mm）
//        const int imageResolution = static_cast<int>(floor(displayImageSize / gridSize)); // 要素画像の解像度（ピクセル）
//
//        // 画像中心からのオフセット計算用
//        const double halfSensorSize = sensorSize / 2.0;
//        const double intv = sensorSize / pixelSize;
//
//        // タイル画像の読み込み
//        cv::Mat tileImage = cv::imread("./images/standard/parrots.bmp");
//        if (tileImage.empty()) {
//            std::cerr << "タイル画像が見つかりません。" << std::endl;
//            return -1;
//        }
//
//        // タイル画像を縦に3分割
//        cv::Mat tileImageLeft = tileImage(cv::Rect(0, 0, tileImage.cols / 3, tileImage.rows)).clone();
//        cv::Mat tileImageCenter = tileImage(cv::Rect(tileImage.cols / 3, 0, tileImage.cols / 3, tileImage.rows)).clone();
//        cv::Mat tileImageRight = tileImage(cv::Rect(tileImage.cols * 2 / 3, 0, tileImage.cols / 3, tileImage.rows)).clone();
//
//        cv::resize(tileImageLeft, tileImageLeft, cv::Size(tileResolution / 3, tileResolution));
//
//        //cv::imshow("image", tileImageLeft);
//        //cv::waitKey(0);
//
//        cv::resize(tileImageCenter, tileImageCenter, cv::Size(tileResolution / 3, tileResolution));
//
//        //cv::imshow("image", tileImageCenter);
//        //cv::waitKey(0);
//
//        cv::resize(tileImageRight, tileImageRight, cv::Size(tileResolution / 3, tileResolution));
//
//        //cv::imshow("image", tileImageRight);
//        //cv::waitKey(0);
//
//        // 出力画像の初期化
//        cv::Mat outputImage(imageResolution, imageResolution, CV_8UC3, cv::Scalar(0, 0, 0));
//        cv::Mat tilePartialImage(tileResolution / 3, tileResolution, CV_8UC3, cv::Scalar(0, 0, 0));
//        for (double tileZ = 256; tileZ <= 8192; tileZ *= 2) {
//
//            std::cout << "tileZ:" << tileZ << std::endl;
//
//            double tileSize = displayAreaSize;
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
//            displayImage = cv::Scalar::all(0);
//            for (int k = 0; k < 1; ++k) {
//
//                cv::Mat currentDisplayImage = cv::Mat::zeros(cv::Size(displayImageSize, displayImageSize), CV_8UC3);
//                for (int m = 0; m < gridSize; ++m) {
//
//                    //startv = static_cast<int>(round(m * intv));
//
//                    for (int n = 0; n < gridSize; ++n) {
//
//                        double tileLeft, tileRight, currentTileZ;
//                        outputImage = cv::Scalar::all(0);
//                        //startu = static_cast<int>(round(n * intv));
//
//                        tilePartialImage = cv::Scalar::all(0);
//                        if (k == 0) {
//                            tilePartialImage = tileImageRight.clone();
//                            currentTileZ = tileZ * 2.0;
//                            tileLeft = tileSize / 6.0;
//                            tileRight = tileSize / 2.0;
//                        }
//                        else if (k == 1) {
//                            tilePartialImage = tileImageCenter.clone();
//                            currentTileZ = tileZ;
//                            tileLeft = -tileSize / 6.0;
//                            tileRight = tileSize / 6.0;
//                        }
//                        else {
//                            tilePartialImage = tileImageLeft.clone();
//                            currentTileZ = tileZ;
//                            tileLeft = -tileSize / 2.0;
//                            tileRight = -tileSize / 6.0;
//                        }
//
//                        //cv::imshow("image", tilePartialImage);
//                        //cv::waitKey(0);
//
//                        //// 出力画像の初期化
//                        //cv::Mat currentOutputImage(imageResolution, imageResolution, CV_8UC3, cv::Scalar(0, 0, 0));
//
//                        int pixelIndex = 0;
//                        const auto& cameraPos = cameraPositions[cameraIndex++];
//                        startu = static_cast<int>(std::round((cameraPos.x + displayAreaSize / 2 - halfSensorSize) / pixelSize));
//                        startv = static_cast<int>(std::round((cameraPos.y + displayAreaSize / 2 - halfSensorSize) / pixelSize));
//
//                        //std::cout << "cameraPos:" << cameraPos << std::endl;
//
//                        // 各ピクセルについて計算
//                        for (int i = 0; i < imageResolution; ++i) {
//                            for (int j = 0; j < imageResolution; ++j) {
//
//                                const auto& pixelPos = pixelPositions[pixelIndex++];
//                                //std::cout << "pixelPos:" << pixelPos << std::endl;
//
//                                // 1ピクセルから見えるタイル上の領域を計算
//                                // 投影線を通してピクセルからタイルへの射影を計算
//                                double scale = currentTileZ / focalLength; // z=0からz=currentTileZへの拡大率
//                                double viewX = pixelPos.x * scale + cameraPos.x;
//                                double viewY = pixelPos.y * scale + cameraPos.y;
//                                //std::cout << "viewX:" << viewX << ", viewY:" << viewY << std::endl;
//
//                                // タイル上での可視領域（ピンホールの開口を考慮）
//                                double pixelHalf = pixelSize / 2.0;
//                                double viewPixelHalf = pixelHalf * scale;
//
//                                double visibleAreaMinX = viewX - viewPixelHalf;
//                                double visibleAreaMaxX = viewX + viewPixelHalf;
//                                double visibleAreaMinY = viewY - viewPixelHalf;
//                                double visibleAreaMaxY = viewY + viewPixelHalf;
//
//                                // タイルの範囲内に制限
//                                visibleAreaMinX = std::max(tileLeft, visibleAreaMinX);
//                                visibleAreaMaxX = std::min(tileRight, visibleAreaMaxX);
//                                visibleAreaMinY = std::max(-tileSize / 2.0, visibleAreaMinY);
//                                visibleAreaMaxY = std::min(tileSize / 2.0, visibleAreaMaxY);
//                                //std::cout << "visible are min X:" << visibleAreaMinX << ", visible are max X:" << visibleAreaMaxX << ", visible are min Y:" << visibleAreaMinY << ", visible are max Y:" << visibleAreaMaxY << std::endl;
//                                //std::cout << "diff X:" << visibleAreaMaxX - visibleAreaMinX << ", diff Y:" << visibleAreaMaxY - visibleAreaMinY << std::endl;
//
//                                // 可視領域がタイル内に存在しない場合
//                                if (visibleAreaMinX >= visibleAreaMaxX || visibleAreaMinY >= visibleAreaMaxY)
//                                {
//                                    // 黒色を設定
//                                    outputImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
//                                    continue;
//                                }
//
//                                // ピクセルインデックスの範囲を計算
//                                int xMinIdx = std::max(0, static_cast<int>(std::floor((visibleAreaMinX + tileSize / 6.0) / tilePixelSize)));
//                                int xMaxIdx = std::min(tileResolution - 1, static_cast<int>(std::floor((visibleAreaMaxX + tileSize / 6.0) / tilePixelSize)));
//                                int yMinIdx = std::max(0, static_cast<int>(std::floor((visibleAreaMinY + tileSize / 2.0) / tilePixelSize)));
//                                int yMaxIdx = std::min(tileResolution - 1, static_cast<int>(std::floor((visibleAreaMaxY + tileSize / 2.0) / tilePixelSize)));
//                                //std::cout << "X min index:" << xMinIdx << ", X max index:" << xMaxIdx << ", Y min index:" << yMinIdx << ", Y max index:" << yMaxIdx << std::endl;
//
//                                // 観察される色の加重平均を計算
//                                cv::Vec3d weighted_color(0, 0, 0);
//                                double total_area = 0.0;
//
//                                for (int xIdx = xMinIdx; xIdx <= xMaxIdx; ++xIdx)
//                                {
//                                    // ピクセルのx座標範囲を計算
//                                    double xPixelMin = xIdx * tilePixelSize - tileSize / 6.0;
//                                    double xPixelMax = (xIdx + 1) * tilePixelSize - tileSize / 6.0;
//
//                                    double xOverlap = std::max(0.0, std::min(visibleAreaMaxX, xPixelMax) - std::max(visibleAreaMinX, xPixelMin));
//                                    //std::cout << "visible are max X:" << visibleAreaMaxX << ", x pixel max:" << xPixelMax << ", visible are min X:" << visibleAreaMinX << ", x pixel min:" << xPixelMin << std::endl;
//                                    //std::cout << "x overlap:" << xOverlap << std::endl;
//
//                                    if (xOverlap <= 0) continue;
//
//                                    for (int yIdx = yMinIdx; yIdx <= yMaxIdx; ++yIdx)
//                                    {
//                                        // ピクセルのy座標範囲を計算
//                                        double yPixelMin = yIdx * tilePixelSize - tileSize / 2;
//                                        double yPixelMax = (yIdx + 1) * tilePixelSize - tileSize / 2;
//
//                                        double yOverlap = std::max(0.0, std::min(visibleAreaMaxY, yPixelMax) - std::max(visibleAreaMinY, yPixelMin));
//                                        //std::cout << "visible are max Y:" << visibleAreaMaxY << ", y pixel max:" << yPixelMax << ", visible are min Y:" << visibleAreaMinY << ", y pixel min:" << yPixelMin << std::endl;
//                                        //std::cout << "y overlap:" << yOverlap << std::endl;
//
//                                        if (yOverlap <= 0) continue;
//
//                                        double overlap_area = xOverlap * yOverlap;
//                                        total_area += overlap_area;
//
//                                        // ピクセル値を取得し、加重平均に加算
//                                        cv::Vec3b pixel = tilePartialImage.at<cv::Vec3b>(yIdx, xIdx);
//                                        weighted_color += cv::Vec3d(pixel) * overlap_area;
//                                    }
//                                }
//
//                                cv::Vec3b final_color;
//                                if (total_area > 0)
//                                {
//                                    weighted_color /= total_area;
//                                    final_color = cv::Vec3b(
//                                        static_cast<uchar>(weighted_color[0]),
//                                        static_cast<uchar>(weighted_color[1]),
//                                        static_cast<uchar>(weighted_color[2])
//                                    );
//                                }
//                                else
//                                {
//                                    // 観察可能な領域がない場合は黒色
//                                    final_color = cv::Vec3b(0, 0, 0);
//                                }
//
//                                // 出力画像に色を設定
//                                outputImage.at<cv::Vec3b>(i, j) = final_color;
//
//                            }
//                        }
//
//                        for (int i = 0; i < imageResolution; ++i) {
//                            for (int j = 0; j < imageResolution; ++j) {
//                                currentDisplayImage.at<cv::Vec3b>(startv + i, startu + j) = outputImage.at<cv::Vec3b>(i, j);
//                            }
//                        }
//
//                        //outputImage = currentOutputImage.clone();
//
//                        //for (int i = 0; i < imageResolution; ++i) {
//                        //    for (int j = 0; j < imageResolution; ++j) {
//                        //        if (currentOutputImage.at<cv::Vec3b>(i, j)[0] > 0 && currentOutputImage.at<cv::Vec3b>(i, j)[1] > 0 && currentOutputImage.at<cv::Vec3b>(i, j)[2] > 0) {
//                        //            outputImage.at<cv::Vec3b>(i, j) = currentOutputImage.at<cv::Vec3b>(i, j);
//                        //        }
//                        //    }
//                        //}
//
//                        //cv::imshow("image", outputImage);
//                        //cv::waitKey(0);
//
//                    }
//                }
//
//                for (int i = 0; i < displayImageSize; ++i) {
//                    for (int j = 0; j < displayImageSize; ++j) {
//                        if (currentDisplayImage.at<cv::Vec3b>(i, j)[0] > 0 && currentDisplayImage.at<cv::Vec3b>(i, j)[1] > 0 && currentDisplayImage.at<cv::Vec3b>(i, j)[2] > 0)
//                        displayImage.at<cv::Vec3b>(i, j) = currentDisplayImage.at<cv::Vec3b>(i, j);
//                    }
//                }
//            }
//
//            // 画像の保存
//            std::string filename = "C:/Users/taw11/EvacuatedStorage/prop-reconstruction/ideal-v2/ideal-v2-tileNotExpand_parrots_gridSize" + std::to_string(static_cast<int>(gridSize)) + "_zi" + std::to_string(static_cast<int>(tileZ)) + ".png";
//
//            cv::imwrite(filename, displayImage);
//
//        }
//
//    }
//
//    return 0;
//}

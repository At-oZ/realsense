//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <Windows.h>
//#include <fstream>
//#include <vector>
//#include <string>
//
//// CSVファイルに配列データを書き込む関数（手動シフト量、列シフト量、行シフト量を保存）
//void saveAllArraysToCsv(const std::vector<int>& columnShiftsX, const std::vector<int>& columnShiftsY,
//    const std::vector<int>& rowShiftsX, const std::vector<int>& rowShiftsY,
//    int manualOffsetX, int manualOffsetY, const std::string& filename) {
//    std::ofstream csvFile(filename);
//
//    if (!csvFile.is_open()) {
//        std::cerr << "ファイルを開けませんでした: " << filename << std::endl;
//        return;
//    }
//
//    // 手動シフト量を最初に記録
//    csvFile << "ManualShift,X,Y" << std::endl;
//    csvFile << "," << manualOffsetX << "," << manualOffsetY << std::endl;
//
//    // 空行を挿入して区切る
//    csvFile << std::endl;
//
//    // 列シフト量のヘッダー行
//    csvFile << "Column,X_Shift,Y_Shift" << std::endl;
//
//    // 列シフト量のデータ行
//    for (size_t i = 0; i < columnShiftsX.size(); ++i) {
//        csvFile << i << "," << columnShiftsX[i] << "," << columnShiftsY[i] << std::endl;
//    }
//
//    // 空行を挿入して区切る
//    csvFile << std::endl;
//
//    // 行シフト量のヘッダー行
//    csvFile << "Row,X_Shift,Y_Shift" << std::endl;
//
//    // 行シフト量のデータ行
//    for (size_t i = 0; i < rowShiftsX.size(); ++i) {
//        csvFile << i << "," << rowShiftsX[i] << "," << rowShiftsY[i] << std::endl;
//    }
//
//    // 合計シフト量（手動シフト + 列シフト + 行シフト）のセクションを追加
//    csvFile << std::endl;
//    csvFile << "Position,Total_X_Shift,Total_Y_Shift" << std::endl;
//
//    // 各位置ごとの合計シフト量
//    for (size_t y = 0; y < rowShiftsY.size(); ++y) {
//        for (size_t x = 0; x < columnShiftsX.size(); ++x) {
//            int totalX = manualOffsetX + columnShiftsX[x] + rowShiftsX[y];
//            int totalY = manualOffsetY + columnShiftsY[x] + rowShiftsY[y];
//            csvFile << "(" << x << "," << y << ")," << totalX << "," << totalY << std::endl;
//        }
//    }
//
//    csvFile.close();
//    std::cout << "シフト量を " << filename << " に保存しました。" << std::endl;
//}
//
//int main()
//{
//    // パラメータ設定
//    const int elementSize = 60;           // 要素画像のサイズ (60x60ピクセル)
//    const int numElements = 40;           // 縦横の要素画像数
//    const int totalSize = elementSize * numElements; // 全体画像サイズ (2400x2400)
//
//    // 画像作成 (黒背景の画像)
//    cv::Mat baseImage(totalSize, totalSize, CV_8UC3, cv::Scalar(0, 0, 0));
//
//    // 各要素画像の中心に赤い2x2ピクセルを描画
//    for (int y = 0; y < numElements; y++) {
//        for (int x = 0; x < numElements; x++) {
//            // 要素画像の左上座標
//            int startX = x * elementSize;
//            int startY = y * elementSize;
//
//            // 要素画像の中心座標
//            int centerX = startX + elementSize / 2;
//            int centerY = startY + elementSize / 2;
//
//            // 中心の2x2ピクセルを赤色で描画
//            for (int dy = -1; dy <= 0; dy++) {
//                for (int dx = -1; dx <= 0; dx++) {
//                    baseImage.at<cv::Vec3b>(centerY + dy, centerX + dx) = cv::Vec3b(0, 0, 255); // BGR形式
//                }
//            }
//        }
//    }
//
//    std::string WINNAME = "image";
//    cv::namedWindow(WINNAME);
//    HWND window = FindWindow(NULL, L"image");
//    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
//
//    // 初期位置
//    int posX = 3280;
//    int posY = 0;
//    SetWindowPos(window, NULL, posX, posY, 3840, 2400, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
//
//    // 処理モード
//    enum Mode { MANUAL_SHIFT, COLUMN_SHIFT, ROW_SHIFT };
//    Mode currentMode = MANUAL_SHIFT;
//
//    // 現在処理中の列・行
//    int currentColumn = 0;
//    int currentRow = 0;
//
//    // 手動シフトのオフセット（画素単位でのシフト）
//    int manualOffsetX = 0;
//    int manualOffsetY = 0;
//
//    // 列・行シフトのオフセット
//    int columnOffsetX = 0;
//    int columnOffsetY = 0;
//    int rowOffsetX = 0;
//    int rowOffsetY = 0;
//
//    // 列・行シフトモードのシフト量を記録する配列（X方向とY方向）
//    std::vector<int> columnShiftsX(numElements, 0);
//    std::vector<int> columnShiftsY(numElements, 0);
//    std::vector<int> rowShiftsX(numElements, 0);
//    std::vector<int> rowShiftsY(numElements, 0);
//
//    // 現在の表示画像
//    cv::Mat displayImage = baseImage.clone();
//
//    // 画像表示
//    cv::imshow(WINNAME, displayImage);
//
//    std::cout << "初期モード: 手動シフトモード" << std::endl;
//    std::cout << "W,A,S,Dキーで画像を1pxずつ移動、Enterで列シフトモードに移行します。" << std::endl;
//
//    bool running = true;
//    while (running) {
//        int key = cv::waitKey(0);
//        bool imageUpdated = false;
//
//        if (currentMode == MANUAL_SHIFT) {
//            // 手動シフトモード
//            switch (key) {
//            case 27: // ESCキー
//                running = false;
//                break;
//            case 13: // Enterキー
//                // 手動シフト後の画像をベースとして列シフトモードに移行
//                currentMode = COLUMN_SHIFT;
//                currentColumn = 0;
//
//                // 列シフト用のオフセットをリセット
//                columnOffsetX = 0;
//                columnOffsetY = 0;
//
//                std::cout << "列シフトモードに移行します。列 " << currentColumn + 1 << "/" << numElements << " の調整を開始します。" << std::endl;
//                std::cout << "手動シフト量: X=" << manualOffsetX << ", Y=" << manualOffsetY << "px は維持されます。" << std::endl;
//                std::cout << "列シフトモードではX方向（左右）のみ移動できます。" << std::endl;
//                imageUpdated = true;
//                break;
//            case 119: // Wキー(上)
//                manualOffsetY--;
//                imageUpdated = true;
//                break;
//            case 115: // Sキー(下)
//                manualOffsetY++;
//                imageUpdated = true;
//                break;
//            case 97: // Aキー(左)
//                manualOffsetX--;
//                imageUpdated = true;
//                break;
//            case 100: // Dキー(右)
//                manualOffsetX++;
//                imageUpdated = true;
//                break;
//            default:
//                std::cout << "キーコード：" << key << std::endl;
//                break;
//            }
//        }
//        else if (currentMode == COLUMN_SHIFT) {
//            // 列シフトモード
//            switch (key) {
//            case 27: // ESCキー
//                running = false;
//                break;
//            case 13: // Enterキー
//                // 現在の列のシフト量を保存（列シフト量のみ）
//                columnShiftsX[currentColumn] = columnOffsetX;
//                columnShiftsY[currentColumn] = columnOffsetY; // Y方向は常に0になる
//                std::cout << "列 " << currentColumn + 1 << " のシフト量: X=" << columnOffsetX << "px を保存しました。" << std::endl;
//
//                // 次の列へ
//                currentColumn++;
//                columnOffsetX = 0; // X方向のオフセットをリセット
//
//                if (currentColumn >= numElements) {
//                    // すべての列の処理が完了、行シフトモードへ移行
//                    currentMode = ROW_SHIFT;
//                    currentRow = 0;
//                    rowOffsetX = 0;
//                    rowOffsetY = 0;
//                    std::cout << "すべての列の調整が完了しました。行シフトモードに移行します。" << std::endl;
//                    std::cout << "行 " << currentRow + 1 << "/" << numElements << " の調整を開始します。" << std::endl;
//                    std::cout << "行シフトモードではY方向（上下）のみ移動できます。" << std::endl;
//                }
//                else {
//                    std::cout << "列 " << currentColumn + 1 << "/" << numElements << " の調整を開始します。" << std::endl;
//                }
//                imageUpdated = true;
//                break;
//            case 119: // Wキー(上) - 列シフトモードでは無効
//                std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//                break;
//            case 115: // Sキー(下) - 列シフトモードでは無効
//                std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//                break;
//            case 97: // Aキー(左)
//                columnOffsetX--;
//                imageUpdated = true;
//                break;
//            case 100: // Dキー(右)
//                columnOffsetX++;
//                imageUpdated = true;
//                break;
//            default:
//                std::cout << "キーコード：" << key << std::endl;
//                break;
//            }
//        }
//        else if (currentMode == ROW_SHIFT) {
//            // 行シフトモード
//            switch (key) {
//            case 27: // ESCキー
//                running = false;
//                break;
//            case 13: // Enterキー
//                // 現在の行のシフト量を保存（行シフト量のみ）
//                rowShiftsX[currentRow] = rowOffsetX; // X方向は常に0になる
//                rowShiftsY[currentRow] = rowOffsetY;
//                std::cout << "行 " << currentRow + 1 << " のシフト量: Y=" << rowOffsetY << "px を保存しました。" << std::endl;
//
//                // 次の行へ
//                currentRow++;
//                rowOffsetY = 0; // Y方向のオフセットをリセット
//
//                if (currentRow >= numElements) {
//                    // すべての行の処理が完了
//                    std::cout << "すべての行の調整が完了しました。シフト量をCSVファイルに保存します。" << std::endl;
//                    saveAllArraysToCsv(columnShiftsX, columnShiftsY, rowShiftsX, rowShiftsY,
//                        manualOffsetX, manualOffsetY, "shift_data.csv");
//                    running = false;
//                }
//                else {
//                    std::cout << "行 " << currentRow + 1 << "/" << numElements << " の調整を開始します。" << std::endl;
//                }
//                imageUpdated = true;
//                break;
//            case 119: // Wキー(上)
//                rowOffsetY--;
//                imageUpdated = true;
//                break;
//            case 115: // Sキー(下)
//                rowOffsetY++;
//                imageUpdated = true;
//                break;
//            case 97: // Aキー(左) - 行シフトモードでは無効
//                std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//                break;
//            case 100: // Dキー(右) - 行シフトモードでは無効
//                std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//                break;
//            default:
//                std::cout << "キーコード：" << key << std::endl;
//                break;
//            }
//        }
//
//        // 画像更新
//        if (running && imageUpdated) {
//            // まず黒い背景を作成
//            cv::Mat tempImage(totalSize, totalSize, CV_8UC3, cv::Scalar(0, 0, 0));
//
//            if (currentMode == MANUAL_SHIFT) {
//                // 手動シフトモード: 全体画像をシフト
//                for (int y = 0; y < numElements; y++) {
//                    for (int x = 0; x < numElements; x++) {
//                        // 要素画像の中心座標（初期位置）
//                        int centerX = x * elementSize + elementSize / 2;
//                        int centerY = y * elementSize + elementSize / 2;
//
//                        // 手動シフトを適用
//                        int shiftedX = centerX + manualOffsetX;
//                        int shiftedY = centerY + manualOffsetY;
//
//                        // 中心の2x2ピクセルを赤色で描画
//                        for (int dy = -1; dy <= 0; dy++) {
//                            for (int dx = -1; dx <= 0; dx++) {
//                                int newX = shiftedX + dx;
//                                int newY = shiftedY + dy;
//                                // 画像範囲内かチェック
//                                if (newY >= 0 && newY < totalSize && newX >= 0 && newX < totalSize) {
//                                    tempImage.at<cv::Vec3b>(newY, newX) = cv::Vec3b(0, 0, 255);
//                                }
//                            }
//                        }
//                    }
//                }
//
//                std::cout << "手動シフト量: X=" << manualOffsetX << ", Y=" << manualOffsetY << std::endl;
//            }
//            else if (currentMode == COLUMN_SHIFT) {
//                // 列シフトモード: 列ごとに異なるシフト量を適用（X方向のみ）
//                for (int y = 0; y < numElements; y++) {
//                    for (int x = 0; x < numElements; x++) {
//                        // 要素画像の中心座標（初期位置）
//                        int centerX = x * elementSize + elementSize / 2;
//                        int centerY = y * elementSize + elementSize / 2;
//
//                        // まず手動シフトを適用
//                        int shiftedX = centerX + manualOffsetX;
//                        int shiftedY = centerY + manualOffsetY;
//
//                        // 列シフトの適用（X方向のみ）
//                        if (x == currentColumn) {
//                            // 現在の列には追加のX方向シフト量を適用
//                            shiftedX += columnOffsetX;
//                            // Y方向のシフトは無し
//                        }
//                        else if (x < currentColumn) {
//                            // 既に処理済みの列には保存されたX方向のシフト量を適用
//                            shiftedX += columnShiftsX[x];
//                            // Y方向のシフトは無し
//                        }
//                        // まだ処理していない列には追加シフトなし
//
//                        // 中心の2x2ピクセルを赤色で描画
//                        for (int dy = -1; dy <= 0; dy++) {
//                            for (int dx = -1; dx <= 0; dx++) {
//                                int newX = shiftedX + dx;
//                                int newY = shiftedY + dy;
//                                // 画像範囲内かチェック
//                                if (newY >= 0 && newY < totalSize && newX >= 0 && newX < totalSize) {
//                                    tempImage.at<cv::Vec3b>(newY, newX) = cv::Vec3b(0, 0, 255);
//                                }
//                            }
//                        }
//                    }
//                }
//
//                std::cout << "列 " << currentColumn + 1 << " のシフト量: X=" << columnOffsetX << "px" << std::endl;
//            }
//            else if (currentMode == ROW_SHIFT) {
//                // 行シフトモード: 行ごとに異なるシフト量を適用（Y方向のみ）
//                for (int y = 0; y < numElements; y++) {
//                    for (int x = 0; x < numElements; x++) {
//                        // 要素画像の中心座標（初期位置）
//                        int centerX = x * elementSize + elementSize / 2;
//                        int centerY = y * elementSize + elementSize / 2;
//
//                        // まず手動シフトを適用
//                        int shiftedX = centerX + manualOffsetX;
//                        int shiftedY = centerY + manualOffsetY;
//
//                        // 列シフトの適用
//                        shiftedX += columnShiftsX[x];
//
//                        // 行シフトの適用（Y方向のみ）
//                        if (y == currentRow) {
//                            // 現在の行には追加のY方向シフト量を適用
//                            shiftedY += rowOffsetY;
//                            // X方向のシフトは無し
//                        }
//                        else if (y < currentRow) {
//                            // 既に処理済みの行には保存されたY方向のシフト量を適用
//                            shiftedY += rowShiftsY[y];
//                            // X方向のシフトは無し
//                        }
//                        // まだ処理していない行には追加シフトなし
//
//                        // 中心の2x2ピクセルを赤色で描画
//                        for (int dy = -1; dy <= 0; dy++) {
//                            for (int dx = -1; dx <= 0; dx++) {
//                                int newX = shiftedX + dx;
//                                int newY = shiftedY + dy;
//                                // 画像範囲内かチェック
//                                if (newY >= 0 && newY < totalSize && newX >= 0 && newX < totalSize) {
//                                    tempImage.at<cv::Vec3b>(newY, newX) = cv::Vec3b(0, 0, 255);
//                                }
//                            }
//                        }
//                    }
//                }
//
//                std::cout << "行 " << currentRow + 1 << " のシフト量: Y=" << rowOffsetY << "px" << std::endl;
//            }
//
//            displayImage = tempImage;
//            cv::imshow(WINNAME, displayImage);
//            cv::imwrite("red-dots.png", displayImage);
//        }
//    }
//
//    return 0;
//}

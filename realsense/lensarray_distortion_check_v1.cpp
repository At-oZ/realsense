//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <Windows.h>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <Xinput.h>
//#include <cmath>
//
//// XInputのヘッダとライブラリをリンク
//#pragma comment(lib, "XInput.lib")
//
//// SMC-distortion-check_v1.cppと同様の画像シフト用関数
//void shiftImage(cv::Mat& image, int dx, int dy) {
//    cv::Mat result = cv::Mat::zeros(image.size(), image.type());
//    result.setTo(cv::Scalar::all(0)); // 背景を黒に設定
//
//    // シフト後の有効範囲を計算
//    int startX = (std::max)(0, dx);
//    int startY = (std::max)(0, dy);
//    int endX = (std::min)(image.cols, image.cols + dx);
//    int endY = (std::min)(image.rows, image.rows + dy);
//    int width = endX - startX;
//    int height = endY - startY;
//
//    if (width <= 0 || height <= 0) return; // 有効範囲がない場合は何もしない
//
//    // 元画像の対応する部分をコピー
//    cv::Rect srcRect(startX - dx, startY - dy, width, height);
//    cv::Rect dstRect(startX, startY, width, height);
//
//    image(srcRect).copyTo(result(dstRect));
//    result.copyTo(image);
//}
//
//// コントローラー入力のチェック関数
//bool ProcessXInput(int& offsetX, int& offsetY, bool& enterPressed, bool& escPressed,
//    bool columnModeXOnly, bool rowModeYOnly) {
//    // コントローラーの状態を取得
//    XINPUT_STATE state;
//    ZeroMemory(&state, sizeof(XINPUT_STATE));
//
//    // コントローラー0の状態を取得（最初に接続されたコントローラー）
//    DWORD result = XInputGetState(0, &state);
//
//    // コントローラーが接続されていない場合
//    if (result != ERROR_SUCCESS) {
//        return false;
//    }
//
//    // 十字キーの状態を取得
//    bool changed = false;
//    enterPressed = false;
//    escPressed = false;
//
//    // DPADの状態を確認
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
//        // 上キー - 行モードでもY方向移動可能
//        if (!columnModeXOnly) {
//            offsetY--;
//            std::cout << "上に移動しました。" << std::endl;
//            changed = true;
//        }
//        else {
//            std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//        }
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
//        // 下キー - 行モードでもY方向移動可能
//        if (!columnModeXOnly) {
//            offsetY++;
//            std::cout << "下に移動しました。" << std::endl;
//            changed = true;
//        }
//        else {
//            std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//        }
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
//        // 左キー - 列モードでもX方向移動可能
//        if (!rowModeYOnly) {
//            offsetX--;
//            std::cout << "左に移動しました。" << std::endl;
//            changed = true;
//        }
//        else {
//            std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//        }
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
//        // 右キー - 列モードでもX方向移動可能
//        if (!rowModeYOnly) {
//            offsetX++;
//            std::cout << "右に移動しました。" << std::endl;
//            changed = true;
//        }
//        else {
//            std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//        }
//    }
//
//    // Aボタン - Enterキーと同じ
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
//        static ULONGLONG lastAButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastAButtonPressTime > 300) {
//            enterPressed = true;
//            std::cout << "Enterキーが押されました（Aボタン）。" << std::endl;
//            lastAButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    // Bボタン または バックボタン - ESCキーと同じ
//    if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) || (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)) {
//        static ULONGLONG lastBackButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastBackButtonPressTime > 300) {
//            escPressed = true;
//            std::cout << "ESCキーが押されました（Bボタンまたはバックボタン）。" << std::endl;
//            lastBackButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    return changed;
//}
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
//    // 物理的なパラメータ設定
//    const double elementPhysicalSize = 4.525;    // 要素画像の物理的サイズ (mm)
//    const double pixelPitch = 0.075162683;       // モニターの画素ピッチ (mm)
//    const int numElements = 40;                  // 縦横の要素画像数
//
//    // 要素画像の解像度を計算（小数点以下を切り捨て）
//    const int baseElementSize = static_cast<int>(std::floor(elementPhysicalSize / pixelPitch));
//
//    // 誤差計算
//    const double actualSize = baseElementSize * pixelPitch;
//    const double sizeError = elementPhysicalSize - actualSize;
//    const double totalError = sizeError * numElements;
//
//    // バッファの計算（全体の誤差を補正するために必要なバッファのピクセル数）
//    const int totalBufferPixels = static_cast<int>(std::round(totalError / pixelPitch));
//
//    std::cout << "物理的サイズ: " << elementPhysicalSize << " mm" << std::endl;
//    std::cout << "画素ピッチ: " << pixelPitch << " mm" << std::endl;
//    std::cout << "要素画像の解像度: " << baseElementSize << " px" << std::endl;
//    std::cout << "誤差: " << sizeError << " mm/要素" << std::endl;
//    std::cout << "全体誤差: " << totalError << " mm" << std::endl;
//    std::cout << "必要バッファ: " << totalBufferPixels << " px" << std::endl;
//
//    // バッファの配分計算 (1pxずつ均等に分配)
//    std::vector<int> bufferDistribution(numElements, 0);
//    int remainingBuffers = totalBufferPixels;
//
//    // バッファを均等に配分
//    int interval = numElements / totalBufferPixels;
//    if (interval == 0) interval = 1; // 0除算防止
//
//    for (int i = 0; i < numElements && remainingBuffers > 0; i += interval) {
//        bufferDistribution[i] = 1;
//        remainingBuffers--;
//    }
//
//    // 残りのバッファを均等に配分（まだある場合）
//    int i = 0;
//    while (remainingBuffers > 0) {
//        if (bufferDistribution[i] == 0) {
//            bufferDistribution[i] = 1;
//            remainingBuffers--;
//        }
//        i = (i + 1) % numElements;
//    }
//
//    // 要素画像のサイズ (baseElementSize + バッファ)
//    std::vector<int> elementSizes(numElements);
//    for (int i = 0; i < numElements; i++) {
//        elementSizes[i] = baseElementSize + bufferDistribution[i];
//    }
//
//    // 全体画像サイズの計算（各要素サイズの合計）
//    int totalWidth = 0;
//    int totalHeight = 0;
//    for (int i = 0; i < numElements; i++) {
//        totalWidth += elementSizes[i];
//        totalHeight += elementSizes[i];
//    }
//
//    // 画像作成 (黒背景の画像)
//    cv::Mat baseImage(totalHeight, totalWidth, CV_8UC3, cv::Scalar(0, 0, 0));
//
//    // 各要素画像の中心に赤い2x2ピクセルを描画
//    int yOffset = 0;
//    for (int y = 0; y < numElements; y++) {
//        int xOffset = 0;
//        for (int x = 0; x < numElements; x++) {
//            // 要素画像の左上座標
//            int startX = xOffset;
//            int startY = yOffset;
//
//            // 要素画像の中心座標
//            int centerX = startX + elementSizes[x] / 2;
//            int centerY = startY + elementSizes[y] / 2;
//
//            // 中心の2x2ピクセルを赤色で描画
//            for (int dy = -1; dy <= 0; dy++) {
//                for (int dx = -1; dx <= 0; dx++) {
//                    baseImage.at<cv::Vec3b>(centerY + dy, centerX + dx) = cv::Vec3b(0, 0, 255); // BGR形式
//                }
//            }
//
//            // 次の要素へのオフセットを更新
//            xOffset += elementSizes[x];
//        }
//        // 次の行へのオフセットを更新
//        yOffset += elementSizes[y];
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
//    SetWindowPos(window, NULL, posX, posY, totalWidth, totalHeight, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
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
//    // 現在の表示画像とオリジナル画像
//    cv::Mat displayImage = baseImage.clone();
//    cv::Mat originalImage = baseImage.clone();  // 元画像のバックアップ
//
//    // 画像表示
//    cv::imshow(WINNAME, displayImage);
//
//    std::cout << "初期モード: 手動シフトモード" << std::endl;
//    std::cout << "W,A,S,Dキーまたはコントローラー十字キーで画像を1pxずつ移動、Enterまたはコントローラーのボタンで列シフトモードに移行します。" << std::endl;
//
//    // コントローラーのポーリング間隔 (ms)
//    const int controllerPollInterval = 50;
//    ULONGLONG lastPollTime = 0;
//
//    bool running = true;
//    while (running) {
//        // キーボード入力をチェック（非ブロッキング）
//        int key = cv::waitKey(1);
//        bool imageUpdated = false;
//        bool enterPressed = false;
//        bool escPressed = false;
//
//        // コントローラー入力チェック
//        ULONGLONG currentTime = GetTickCount64();
//        if (currentTime - lastPollTime > controllerPollInterval) {
//            bool columnModeXOnly = (currentMode == COLUMN_SHIFT);
//            bool rowModeYOnly = (currentMode == ROW_SHIFT);
//
//            // 適切なオフセットへの参照を渡す
//            int* offsetXRef = nullptr;
//            int* offsetYRef = nullptr;
//
//            switch (currentMode) {
//            case MANUAL_SHIFT:
//                offsetXRef = &manualOffsetX;
//                offsetYRef = &manualOffsetY;
//                break;
//            case COLUMN_SHIFT:
//                offsetXRef = &columnOffsetX;
//                offsetYRef = &columnOffsetY;
//                break;
//            case ROW_SHIFT:
//                offsetXRef = &rowOffsetX;
//                offsetYRef = &rowOffsetY;
//                break;
//            }
//
//            if (offsetXRef && offsetYRef) {
//                bool changed = ProcessXInput(*offsetXRef, *offsetYRef, enterPressed, escPressed, columnModeXOnly, rowModeYOnly);
//                if (changed) {
//                    imageUpdated = true;
//                }
//            }
//
//            lastPollTime = currentTime;
//        }
//
//        if (key == -1 && !imageUpdated && !enterPressed && !escPressed) {
//            // キー入力がなく、コントローラーによる変更もない場合は待機
//            Sleep(10);
//            continue;
//        }
//
//        // キーボード入力の処理
//        if (key != -1) {
//            if (currentMode == MANUAL_SHIFT) {
//                // 手動シフトモード
//                switch (key) {
//                case 27: // ESCキー
//                    running = false;
//                    break;
//                case 13: // Enterキー
//                    enterPressed = true;
//                    break;
//                case 119: // Wキー(上)
//                    manualOffsetY--;
//                    imageUpdated = true;
//                    break;
//                case 115: // Sキー(下)
//                    manualOffsetY++;
//                    imageUpdated = true;
//                    break;
//                case 97: // Aキー(左)
//                    manualOffsetX--;
//                    imageUpdated = true;
//                    break;
//                case 100: // Dキー(右)
//                    manualOffsetX++;
//                    imageUpdated = true;
//                    break;
//                default:
//                    std::cout << "キーコード：" << key << std::endl;
//                    break;
//                }
//            }
//            else if (currentMode == COLUMN_SHIFT) {
//                // 列シフトモード
//                switch (key) {
//                case 27: // ESCキー
//                    running = false;
//                    break;
//                case 13: // Enterキー
//                    enterPressed = true;
//                    break;
//                case 119: // Wキー(上) - 列シフトモードでは無効
//                    std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//                    break;
//                case 115: // Sキー(下) - 列シフトモードでは無効
//                    std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//                    break;
//                case 97: // Aキー(左)
//                    columnOffsetX--;
//                    imageUpdated = true;
//                    break;
//                case 100: // Dキー(右)
//                    columnOffsetX++;
//                    imageUpdated = true;
//                    break;
//                default:
//                    std::cout << "キーコード：" << key << std::endl;
//                    break;
//                }
//            }
//            else if (currentMode == ROW_SHIFT) {
//                // 行シフトモード
//                switch (key) {
//                case 27: // ESCキー
//                    running = false;
//                    break;
//                case 13: // Enterキー
//                    enterPressed = true;
//                    break;
//                case 119: // Wキー(上)
//                    rowOffsetY--;
//                    imageUpdated = true;
//                    break;
//                case 115: // Sキー(下)
//                    rowOffsetY++;
//                    imageUpdated = true;
//                    break;
//                case 97: // Aキー(左) - 行シフトモードでは無効
//                    std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//                    break;
//                case 100: // Dキー(右) - 行シフトモードでは無効
//                    std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//                    break;
//                default:
//                    std::cout << "キーコード：" << key << std::endl;
//                    break;
//                }
//            }
//        }
//
//        // ESCキーが押された場合
//        if (escPressed) {
//            running = false;
//        }
//
//        // Enterキーが押された場合のモード切替処理
//        if (enterPressed) {
//            if (currentMode == MANUAL_SHIFT) {
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
//            }
//            else if (currentMode == COLUMN_SHIFT) {
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
//            }
//            else if (currentMode == ROW_SHIFT) {
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
//            }
//        }
//
//        // 画像更新
//        if (running && imageUpdated) {
//            if (currentMode == MANUAL_SHIFT) {
//                // 手動シフトモード: 全体画像を平行移動 (SMC-distortion-check_v1.cppと同様)
//                displayImage = originalImage.clone();  // 元画像からコピー
//
//                // シフト量が0でなければ画像全体をシフト
//                if (manualOffsetX != 0 || manualOffsetY != 0) {
//                    // SMC-distortion-check_v1.cppと同様のシフト処理を使用
//                    shiftImage(displayImage, manualOffsetX, manualOffsetY);
//                }
//
//                std::cout << "手動シフト量: X=" << manualOffsetX << ", Y=" << manualOffsetY << std::endl;
//            }
//            else if (currentMode == COLUMN_SHIFT) {
//                // 列シフトモード: 列ごとに異なるシフト量を適用（X方向のみ）
//                cv::Mat tempImage = cv::Mat::zeros(totalHeight, totalWidth, CV_8UC3);
//
//                int yOffset = 0;
//                for (int y = 0; y < numElements; y++) {
//                    int xOffset = 0;
//                    for (int x = 0; x < numElements; x++) {
//                        // 要素画像の中心座標（初期位置）
//                        int centerX = xOffset + elementSizes[x] / 2;
//                        int centerY = yOffset + elementSizes[y] / 2;
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
//                                if (newY >= 0 && newY < totalHeight && newX >= 0 && newX < totalWidth) {
//                                    tempImage.at<cv::Vec3b>(newY, newX) = cv::Vec3b(0, 0, 255);
//                                }
//                            }
//                        }
//
//                        // 次の要素へのオフセットを更新
//                        xOffset += elementSizes[x];
//                    }
//                    // 次の行へのオフセットを更新
//                    yOffset += elementSizes[y];
//                }
//
//                displayImage = tempImage;
//                std::cout << "列 " << currentColumn + 1 << " のシフト量: X=" << columnOffsetX << "px" << std::endl;
//            }
//            else if (currentMode == ROW_SHIFT) {
//                // 行シフトモード: 行ごとに異なるシフト量を適用（Y方向のみ）
//                cv::Mat tempImage = cv::Mat::zeros(totalHeight, totalWidth, CV_8UC3);
//
//                int yOffset = 0;
//                for (int y = 0; y < numElements; y++) {
//                    int xOffset = 0;
//                    for (int x = 0; x < numElements; x++) {
//                        // 要素画像の中心座標（初期位置）
//                        int centerX = xOffset + elementSizes[x] / 2;
//                        int centerY = yOffset + elementSizes[y] / 2;
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
//                                if (newY >= 0 && newY < totalHeight && newX >= 0 && newX < totalWidth) {
//                                    tempImage.at<cv::Vec3b>(newY, newX) = cv::Vec3b(0, 0, 255);
//                                }
//                            }
//                        }
//
//                        // 次の要素へのオフセットを更新
//                        xOffset += elementSizes[x];
//                    }
//                    // 次の行へのオフセットを更新
//                    yOffset += elementSizes[y];
//                }
//
//                displayImage = tempImage;
//                std::cout << "行 " << currentRow + 1 << " のシフト量: Y=" << rowOffsetY << "px" << std::endl;
//            }
//
//            cv::imshow(WINNAME, displayImage);
//            cv::imwrite("red-dots.png", displayImage);
//        }
//    }
//
//    return 0;
//}

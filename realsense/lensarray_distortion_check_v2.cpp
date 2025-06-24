//// ピンホールカメラアレイを用いて表示画像を作成するプログラム
///*
//    ideal_v2からの派生
//*/
//
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//#include <Windows.h>
//#include <Xinput.h>
//#include <fstream>
//#include <string>
//
//// XInputのヘッダとライブラリをリンク
//#pragma comment(lib, "XInput.lib")
//
//// 処理モード
//enum Mode { MANUAL_SHIFT, COLUMN_SHIFT, ROW_SHIFT };
//
//// コントローラー入力のチェック関数（拡張版）
//bool ProcessXInput(int& x_shift, int& y_shift, int shift_amount, bool& continue_key_loop, bool& running,
//    bool& enter_pressed, bool column_mode_x_only = false, bool row_mode_y_only = false) {
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
//    // enterPressedは関数の外から渡されるため、ここでfalseにリセットしないこと
//    // enter_pressed = false;
//    // 
//    // DPADの状態を確認
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
//        static ULONGLONG lastUButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastUButtonPressTime > 700) {
//            // 上キー - 上移動（列シフトモード以外）
//            if (!column_mode_x_only) {
//                y_shift -= shift_amount;
//                std::cout << "上に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                changed = true;
//            }
//            else {
//                std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//            }
//        }
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
//        static ULONGLONG lastDButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastDButtonPressTime > 700) {
//            // 下キー - 下移動（列シフトモード以外）
//            if (!column_mode_x_only) {
//                y_shift += shift_amount;
//                std::cout << "下に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                changed = true;
//            }
//            else {
//                std::cout << "列シフトモードではY方向への移動はできません。" << std::endl;
//            }
//        }
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
//        static ULONGLONG lastLButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastLButtonPressTime > 700) {
//            // 左キー - 左移動（行シフトモード以外）
//            if (!row_mode_y_only) {
//                x_shift -= shift_amount;
//                std::cout << "左に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                changed = true;
//            }
//            else {
//                std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//            }
//        }
//    }
//
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
//        static ULONGLONG lastRButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastRButtonPressTime > 700) {
//            // 右キー - 右移動（行シフトモード以外）
//            if (!row_mode_y_only) {
//                x_shift += shift_amount;
//                std::cout << "右に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                changed = true;
//            }
//            else {
//                std::cout << "行シフトモードではX方向への移動はできません。" << std::endl;
//            }
//        }
//    }
//
//    // Aボタン - 確定ボタン（Enterキーと同じ）
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
//        static ULONGLONG lastAButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastAButtonPressTime > 700) {
//            enter_pressed = true; // このフラグを明示的にtrueに設定
//            std::cout << "Enterキーが押されました（Aボタン）。" << std::endl;
//            lastAButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    // Bボタン - リセット（Rキーと同じ）
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
//        static ULONGLONG lastBButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastBButtonPressTime > 700) {
//            x_shift = 0;
//            y_shift = 0;
//            std::cout << "位置をリセットしました。" << std::endl;
//            lastBButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    // バックボタン（またはビューボタン） - 終了（ESCと同じ）
//    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
//        static ULONGLONG lastBackButtonPressTime = 0;
//        ULONGLONG currentTime = GetTickCount64();
//
//        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
//        if (currentTime - lastBackButtonPressTime > 700) {
//            continue_key_loop = false;
//            running = false;
//            lastBackButtonPressTime = currentTime;
//            changed = true;
//        }
//    }
//
//    return changed;
//}
//
//// 画像シフト用関数
//void shiftImage(cv::Mat& image, int dx, int dy) {
//    cv::Mat result = cv::Mat::zeros(image.size(), image.type());
//    result.setTo(cv::Scalar::all(255)); // 背景を白に設定
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
//int main() {
//    std::cout << "lensarray-distortion-check-v2" << std::endl;
//
//    int nphs[20] = { 4, 5, 6, 8, 10, 12, 15, 20, 24, 25, 30, 40, 50, 60, 75, 100, 120, 125, 150, 200 };
//
//    std::string WINNAME = "image";
//    cv::namedWindow(WINNAME);
//    HWND window = FindWindow(NULL, L"image");
//    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
//    SetWindowPos(window, NULL, 2560, 0, 3840, 2420, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);
//
//    cv::Mat img_window = cv::Mat::zeros(cv::Size(3840, 2420), CV_8UC3);
//    img_window = cv::Scalar::all(255);
//
//    // 画像シフト用の変数
//    cv::Mat original_img_window;
//    int x_shift = 0, y_shift = 0;
//
//    // シフト量設定用の変数
//    int shift_amount = 1;
//    bool input_mode = false;
//    std::string input_buffer = "";
//
//    // パラメータの固定値を設定
//    double focalLength = 13.0; // 固定値
//    double tileSize = 203.0;   // 固定値
//    double tileZ = 500;        // 固定値
//
//    int nph_idx = 11; // 初期値 (nphs[11] = 40)
//
//    // コントローラーのポーリング間隔 (ms)
//    const int controllerPollInterval = 50;
//    ULONGLONG lastPollTime = 0;
//
//    // モニターの画素ピッチを設定（mm）
//    const double pixelPitch = 0.075162683;
//
//    // 要素画像間の物理的間隔（mm）
//    const double elementSpacing = 181.6 / 40.0;
//
//    // 処理モード変数
//    Mode currentMode = MANUAL_SHIFT;
//
//    // 列・行インデックスと対応するシフト量
//    int currentColumn = 0;
//    int currentRow = 0;
//    int columnOffsetX = 0;
//    int columnOffsetY = 0;
//    int rowOffsetX = 0;
//    int rowOffsetY = 0;
//
//    bool running = true;
//    while (running) {
//        int nph = nphs[nph_idx];
//        const int gridSize = nph;
//
//        // 列・行シフトのための配列を初期化（最初のループでのみ）
//        std::vector<int> columnShiftsX(gridSize, 0);
//        std::vector<int> columnShiftsY(gridSize, 0);
//        std::vector<int> rowShiftsX(gridSize, 0);
//        std::vector<int> rowShiftsY(gridSize, 0);
//
//        std::cout << "number of pins:" << nph << std::endl;
//
//        double zo_min = 1000.0; // 最短視認距離(mm)
//
//        // ディスプレイの設定
//        const int displayImageSize = 2400;
//
//        // 黒い背景に変更
//        cv::Mat displayImage = cv::Mat::zeros(cv::Size(displayImageSize, displayImageSize), CV_8UC3);
//
//        // 要素画像の理論上のピクセルサイズを計算
//        const double elementPixels = elementSpacing / pixelPitch;
//        std::cout << "理論上の要素画像サイズ: " << elementPixels << " px" << std::endl;
//
//        // 要素画像の実際のピクセルサイズ（小数点以下を切り捨て）
//        const int baseElementSize = static_cast<int>(floor(elementPixels));
//        std::cout << "基本要素画像サイズ: " << baseElementSize << " px" << std::endl;
//
//        // 全体の物理サイズを計算
//        const double totalPhysicalSize = elementSpacing * gridSize;
//
//        // 全体の理想的なピクセルサイズ
//        const double totalIdealPixels = totalPhysicalSize / pixelPitch;
//
//        // 要素画像とバッファを合わせた全体のピクセルサイズ
//        int totalActualPixels = baseElementSize * gridSize;
//
//        // バッファが必要な位置を計算
//        std::vector<bool> needBufferH(gridSize, false);
//        std::vector<bool> needBufferV(gridSize, false);
//
//        // バッファが必要な総数を計算
//        int totalBuffersNeeded = static_cast<int>(round(totalIdealPixels)) - totalActualPixels;
//        std::cout << "必要なバッファ総数: " << totalBuffersNeeded << " px" << std::endl;
//
//        // バッファの配置（等間隔に近くなるように）
//        if (totalBuffersNeeded > 0) {
//            double idealSpacing = static_cast<double>(gridSize) / totalBuffersNeeded;
//            for (int i = 0; i < totalBuffersNeeded; i++) {
//                int pos = static_cast<int>(round(i * idealSpacing));
//                if (pos < gridSize - 1) {
//                    needBufferH[pos] = true;
//                    needBufferV[pos] = true;
//                }
//            }
//        }
//
//        // 各要素画像の開始位置を計算
//        std::vector<int> elementStartPosH(gridSize);
//        std::vector<int> elementStartPosV(gridSize);
//
//        int currentPos = 0;
//        for (int i = 0; i < gridSize; i++) {
//            elementStartPosH[i] = currentPos;
//            currentPos += baseElementSize;
//            if (i < gridSize - 1 && needBufferH[i]) {
//                currentPos += 1; // バッファを追加
//            }
//        }
//
//        currentPos = 0;
//        for (int i = 0; i < gridSize; i++) {
//            elementStartPosV[i] = currentPos;
//            currentPos += baseElementSize;
//            if (i < gridSize - 1 && needBufferV[i]) {
//                currentPos += 1; // バッファを追加
//            }
//        }
//
//        // 最終的な表示サイズ
//        const int finalDisplaySize = currentPos;
//        std::cout << "最終的な表示サイズ: " << finalDisplaySize << " px" << std::endl;
//
//        // 必要に応じてdisplayImageのサイズを調整
//        if (finalDisplaySize > displayImageSize) {
//            displayImage = cv::Mat::zeros(cv::Size(finalDisplaySize, finalDisplaySize), CV_8UC3);
//        }
//
//        // 各カメラ(要素画像)について処理 - 初期画像作成部分
//        for (int i = 0; i < gridSize; i++) {
//            for (int j = 0; j < gridSize; j++) {
//                int startX = elementStartPosH[j];
//                int startY = elementStartPosV[i];
//
//                // 要素画像の中央座標を計算
//                // 偶数サイズの要素画像の場合、中央は厳密には存在しないため、
//                // 左上寄りの座標を中央とみなすように調整
//                int centerX = startX + baseElementSize / 2;
//                int centerY = startY + baseElementSize / 2;
//
//                // 横線を描画（中央の白線 - 1px幅に変更）
//                // 線の左端が基準点になるように調整
//                int lineStartX = startX;
//                int lineEndX = startX + baseElementSize;
//
//                // 中央の横線を描画 - 1px幅
//                for (int x = lineStartX; x < lineEndX; x++) {
//                    // 1px幅の線に変更
//                    if (x >= 0 && x < displayImage.cols && centerY >= 0 && centerY < displayImage.rows) {
//                        displayImage.at<cv::Vec3b>(centerY, x) = cv::Vec3b(255, 255, 255); // 白色
//                    }
//                }
//
//                // 縦線を描画（中央の白線 - 1px幅に変更）
//                // 線の上端が基準点になるように調整
//                int lineStartY = startY;
//                int lineEndY = startY + baseElementSize;
//
//                // 中央の縦線を描画 - 1px幅
//                for (int y = lineStartY; y < lineEndY; y++) {
//                    // 1px幅の線に変更
//                    if (y >= 0 && y < displayImage.rows && centerX >= 0 && centerX < displayImage.cols) {
//                        displayImage.at<cv::Vec3b>(y, centerX) = cv::Vec3b(255, 255, 255); // 白色
//                    }
//                }
//            }
//        }
//
//
//        // 中央に配置するためのオフセットを計算
//        int offsetX = (displayImageSize > finalDisplaySize) ? (displayImageSize - finalDisplaySize) / 2 : 0;
//        int offsetY = (displayImageSize > finalDisplaySize) ? (displayImageSize - finalDisplaySize) / 2 : 0;
//
//        // メイン表示ウィンドウに描画
//        img_window = cv::Scalar::all(255); // 白い背景
//
//        // 描画領域の計算
//        int copy_width = (std::max)(finalDisplaySize, displayImageSize);
//        int copy_height = (std::max)(finalDisplaySize, displayImageSize);
//
//        for (int i = 0; i < copy_height; i++) {
//            for (int j = 0; j < copy_width; j++) {
//                int target_i = i + offsetY;
//                int target_j = j + 720 + offsetX; // 横方向にセンタリング
//
//                if (target_i >= 0 && target_i < img_window.rows &&
//                    target_j >= 0 && target_j < img_window.cols) {
//                    img_window.at<cv::Vec3b>(target_i, target_j) = displayImage.at<cv::Vec3b>(i, j);
//                }
//            }
//        }
//
//        // ここで基本画像を作成完了
//        // 各モードによる処理を開始
//
//        bool continue_key_loop = true;
//        std::string last_input_buffer = "";
//
//        // 最初のモードは手動シフトモード
//        if (currentMode == MANUAL_SHIFT) {
//            std::cout << "初期モード: 手動シフトモード" << std::endl;
//            std::cout << "W,A,S,Dキーまたはコントローラー十字キーで画像を" << shift_amount << "pxずつ移動、Enterで列シフトモードに移行します。" << std::endl;
//        }
//
//        // オリジナルの画像を保存（手動シフト適用前）
//        original_img_window = img_window.clone();
//        cv::Mat base_image = img_window.clone(); // 基本画像の保存
//
//        // モード選択処理とキー入力ループ
//        while (continue_key_loop) {
//            cv::Mat display_img;
//            bool imageUpdated = false;
//            bool enterPressed = false;
//
//            // 現在のモードに応じた画像処理
//            if (currentMode == MANUAL_SHIFT) {
//                // 手動シフトモード - 全体画像をシフト
//                display_img = original_img_window.clone();
//                if (x_shift != 0 || y_shift != 0) {
//                    shiftImage(display_img, x_shift, y_shift);
//                }
//            }
//            // 列シフトモード: 特定の列の縦線のみをシフト
//            else if (currentMode == COLUMN_SHIFT) {
//                // 手動シフトのみを適用した画像を作成
//                display_img = original_img_window.clone();
//                if (x_shift != 0 || y_shift != 0) {
//                    shiftImage(display_img, x_shift, y_shift);
//                }
//
//                // 各要素画像を処理
//                for (int i = 0; i < gridSize; i++) {
//                    for (int j = 0; j < gridSize; j++) {
//                        int startX = elementStartPosH[j] + 720 + offsetX;
//                        int startY = elementStartPosV[i] + offsetY;
//                        int centerX = startX + baseElementSize / 2;
//                        int centerY = startY + baseElementSize / 2;
//
//                        // 手動シフト適用
//                        startX += x_shift;
//                        centerX += x_shift;
//                        startY += y_shift;
//                        centerY += y_shift;
//
//                        // 列シフトモード: 現在の列に対する処理
//                        if (j == currentColumn) {
//                            // 元の縦線を消去（黒の背景色で上書き）
//                            for (int y = startY; y < startY + baseElementSize; y++) {
//                                // 1px幅の線を消去
//                                if (y >= 0 && y < display_img.rows && centerX >= 0 && centerX < display_img.cols) {
//                                    // 元の縦線位置を黒（背景色）で塗りつぶす
//                                    display_img.at<cv::Vec3b>(y, centerX) = cv::Vec3b(0, 0, 0);
//                                }
//                            }
//
//                            // シフト位置に赤い縦線を描画（元の位置+オフセット）- 1px幅
//                            int shiftedX = centerX + columnOffsetX;
//                            for (int y = startY; y < startY + baseElementSize; y++) {
//                                if (y >= 0 && y < display_img.rows && shiftedX >= 0 && shiftedX < display_img.cols) {
//                                    display_img.at<cv::Vec3b>(y, shiftedX) = cv::Vec3b(0, 0, 255); // 赤色
//                                }
//                            }
//                        }
//                        // 調整済みの列に対する処理
//                        else if (j < currentColumn) {
//                            // 元の縦線を消去（黒の背景色で塗りつぶす）
//                            for (int y = startY; y < startY + baseElementSize; y++) {
//                                if (y >= 0 && y < display_img.rows && centerX >= 0 && centerX < display_img.cols) {
//                                    display_img.at<cv::Vec3b>(y, centerX) = cv::Vec3b(0, 0, 0);
//                                }
//                            }
//
//                            // シフト位置に白い縦線を描画 - 1px幅
//                            int shiftedX = centerX + columnShiftsX[j];
//                            for (int y = startY; y < startY + baseElementSize; y++) {
//                                if (y >= 0 && y < display_img.rows && shiftedX >= 0 && shiftedX < display_img.cols) {
//                                    display_img.at<cv::Vec3b>(y, shiftedX) = cv::Vec3b(255, 255, 255); // 白色
//                                }
//                            }
//                        }
//                    }
//                }
//
//                // モード表示
//                std::string modeText = "列シフトモード: 列 " + std::to_string(currentColumn + 1) + "/" + std::to_string(gridSize);
//                cv::putText(display_img, modeText, cv::Point(50, 50),
//                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
//            }
//            // 行シフトモード: 特定の行の横線のみをシフト
//            else if (currentMode == ROW_SHIFT) {
//                // 手動シフトのみを適用した画像を作成
//                display_img = original_img_window.clone();
//                if (x_shift != 0 || y_shift != 0) {
//                    shiftImage(display_img, x_shift, y_shift);
//                }
//
//                // 各要素画像を処理
//                for (int i = 0; i < gridSize; i++) {
//                    for (int j = 0; j < gridSize; j++) {
//                        int startX = elementStartPosH[j] + 720 + offsetX;
//                        int startY = elementStartPosV[i] + offsetY;
//                        int centerX = startX + baseElementSize / 2;
//                        int centerY = startY + baseElementSize / 2;
//
//                        // 手動シフト適用
//                        startX += x_shift;
//                        centerX += x_shift;
//                        startY += y_shift;
//                        centerY += y_shift;
//
//                        // 要素画像の範囲を明確にする
//                        int elementWidth = baseElementSize;
//                        int elementHeight = baseElementSize;
//
//                        // 現在の行に対する処理
//                        if (i == currentRow) {
//                            // 元の横線を消去（黒の背景色で上書き）
//                            for (int x = startX; x < startX + elementWidth; x++) {
//                                // 1px幅の線を消去
//                                if (x >= 0 && x < display_img.cols && centerY >= 0 && centerY < display_img.rows) {
//                                    // 元の横線位置を黒（背景色）で塗りつぶす
//                                    display_img.at<cv::Vec3b>(centerY, x) = cv::Vec3b(0, 0, 0);
//                                }
//                            }
//
//                            // シフト位置に赤い横線を描画 - 1px幅
//                            // rowOffsetYは初期値が0なので、最初は元の位置に描画される
//                            int shiftedY = centerY + rowOffsetY;
//                            for (int x = startX; x < startX + elementWidth; x++) {
//                                if (x >= 0 && x < display_img.cols && shiftedY >= 0 && shiftedY < display_img.rows) {
//                                    display_img.at<cv::Vec3b>(shiftedY, x) = cv::Vec3b(0, 0, 255); // 赤色
//                                }
//                            }
//                        }
//                        // 調整済みの行に対する処理
//                        else if (i < currentRow) {
//                            // 元の横線を消去（黒の背景色で塗りつぶす）
//                            for (int x = startX; x < startX + elementWidth; x++) {
//                                if (x >= 0 && x < display_img.cols && centerY >= 0 && centerY < display_img.rows) {
//                                    display_img.at<cv::Vec3b>(centerY, x) = cv::Vec3b(0, 0, 0);
//                                }
//                            }
//
//                            // シフト位置に白い横線を描画 - 1px幅
//                            int shiftedY = centerY + rowShiftsY[i];
//                            for (int x = startX; x < startX + elementWidth; x++) {
//                                if (x >= 0 && x < display_img.cols && shiftedY >= 0 && shiftedY < display_img.rows) {
//                                    display_img.at<cv::Vec3b>(shiftedY, x) = cv::Vec3b(255, 255, 255); // 白色
//                                }
//                            }
//                        }
//                    }
//                }
//
//                // モード表示
//                std::string modeText = "行シフトモード: 行 " + std::to_string(currentRow + 1) + "/" + std::to_string(gridSize);
//                cv::putText(display_img, modeText, cv::Point(50, 50),
//                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
//            }
//
//
//
//            // 入力モード中の表示
//            if (input_mode && last_input_buffer != input_buffer) {
//                // シフト量入力モードの表示
//                std::cout << "シフト量入力モード: " << input_buffer << "_" << std::endl;
//                std::string display_text = "シフト量: " + input_buffer + "_";
//                cv::putText(display_img, display_text, cv::Point(50, 100),
//                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
//
//                last_input_buffer = input_buffer;
//            }
//
//            // 画像を表示
//            cv::imshow(WINNAME, display_img);
//
//            // 入力モード中はコントローラー入力を無視
//            if (!input_mode) {
//                // 現在の時間を取得
//                ULONGLONG currentTime = GetTickCount64();
//
//                // そして、メインループの中で、以下のように修正:
//                // 一定間隔でコントローラー入力をチェック
//                if (currentTime - lastPollTime > controllerPollInterval) {
//                    // コントローラー入力の処理
//                    bool controller_changed = false;
//
//                    // モードに応じた制限
//                    bool columnModeXOnly = (currentMode == COLUMN_SHIFT);
//                    bool rowModeYOnly = (currentMode == ROW_SHIFT);
//
//                    // 適切な変数への参照
//                    int* targetX = &x_shift;
//                    int* targetY = &y_shift;
//
//                    if (currentMode == COLUMN_SHIFT) {
//                        targetX = &columnOffsetX;
//                    }
//                    else if (currentMode == ROW_SHIFT) {
//                        targetY = &rowOffsetY;
//                    }
//
//                    // enterPressedをfalseに初期化してから渡す
//                    enterPressed = false;
//
//                    controller_changed = ProcessXInput(*targetX, *targetY, shift_amount,
//                        continue_key_loop, running,
//                        enterPressed, columnModeXOnly, rowModeYOnly);
//
//                    //std::cout << enterPressed << std::endl;
//
//                    // 入力が変更された場合、または特別なボタンが押された場合
//                    if (controller_changed || enterPressed) {
//                        imageUpdated = true;
//                    }
//
//                    lastPollTime = currentTime;
//                }
//            }
//
//            // キーボード入力をチェック (非ブロッキング)
//            int key = cv::waitKey(1);
//
//            // キーが押されていない場合
//            if (key == -1 && !enterPressed) {
//                // 短い待機時間を挿入してCPU使用率を抑制
//                Sleep(10);
//                continue;
//            }
//
//            // 入力モード中の処理
//            if (input_mode) {
//                if (key >= '0' && key <= '9' || key == '.') {
//                    // 数字キーまたはドットが押されたら入力バッファに追加
//                    input_buffer += (char)key;
//                    imageUpdated = true;
//                }
//                else if (key == 8) { // バックスペース
//                    // バッファから1文字削除
//                    if (!input_buffer.empty()) {
//                        input_buffer.pop_back();
//                        imageUpdated = true;
//                    }
//                }
//                else if (key == 13) { // Enter
//                    // 入力確定、シフト量を設定
//                    if (!input_buffer.empty()) {
//                        shift_amount = std::stoi(input_buffer);
//                        std::cout << "シフト量を " << shift_amount << " に設定しました。" << std::endl;
//                    }
//                    input_mode = false;
//                    input_buffer = "";
//                    imageUpdated = true;
//                }
//                else if (key == 27) { // ESC
//                    // 入力モードをキャンセル
//                    input_mode = false;
//                    input_buffer = "";
//                    imageUpdated = true;
//                }
//                continue;
//            }
//
//            // Enterキーの処理（モード切替）
//            if (key == 13 || enterPressed) {
//                enterPressed = true;
//            }
//
//            // モード別キー処理
//            if (currentMode == MANUAL_SHIFT) {
//                switch (key) {
//                case 'w': // 上へ移動
//                case 'W':
//                    y_shift -= shift_amount;
//                    std::cout << "上に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 's': // 下へ移動
//                case 'S':
//                    y_shift += shift_amount;
//                    std::cout << "下に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'a': // 左へ移動
//                case 'A':
//                    x_shift -= shift_amount;
//                    std::cout << "左に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'd': // 右へ移動
//                case 'D':
//                    x_shift += shift_amount;
//                    std::cout << "右に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'r': // リセット
//                case 'R':
//                    x_shift = 0;
//                    y_shift = 0;
//                    std::cout << "位置をリセットしました。" << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'i': // シフト量設定モード
//                case 'I':
//                    input_mode = true;
//                    input_buffer = "";
//                    std::cout << "シフト量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 27: // ESC: 終了
//                    continue_key_loop = false;
//                    running = false;
//                    break;
//                }
//            }
//            else if (currentMode == COLUMN_SHIFT) {
//                switch (key) {
//                case 'a': // 左へ移動
//                case 'A':
//                    columnOffsetX -= shift_amount;
//                    std::cout << "列 " << currentColumn + 1 << " を左に " << shift_amount << " px 移動しました。シフト量: " << columnOffsetX << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'd': // 右へ移動
//                case 'D':
//                    columnOffsetX += shift_amount;
//                    std::cout << "列 " << currentColumn + 1 << " を右に " << shift_amount << " px 移動しました。シフト量: " << columnOffsetX << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'r': // リセット
//                case 'R':
//                    columnOffsetX = 0;
//                    std::cout << "列 " << currentColumn + 1 << " のシフト量をリセットしました。" << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'i': // シフト量設定モード
//                case 'I':
//                    input_mode = true;
//                    input_buffer = "";
//                    std::cout << "シフト量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 27: // ESC: 終了
//                    continue_key_loop = false;
//                    running = false;
//                    break;
//                }
//            }
//            else if (currentMode == ROW_SHIFT) {
//                switch (key) {
//                case 'w': // 上へ移動
//                case 'W':
//                    rowOffsetY -= shift_amount;
//                    std::cout << "行 " << currentRow + 1 << " を上に " << shift_amount << " px 移動しました。シフト量: " << rowOffsetY << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 's': // 下へ移動
//                case 'S':
//                    rowOffsetY += shift_amount;
//                    std::cout << "行 " << currentRow + 1 << " を下に " << shift_amount << " px 移動しました。シフト量: " << rowOffsetY << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'r': // リセット
//                case 'R':
//                    rowOffsetY = 0;
//                    std::cout << "行 " << currentRow + 1 << " のシフト量をリセットしました。" << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 'i': // シフト量設定モード
//                case 'I':
//                    input_mode = true;
//                    input_buffer = "";
//                    std::cout << "シフト量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
//                    imageUpdated = true;
//                    break;
//
//                case 27: // ESC: 終了
//                    continue_key_loop = false;
//                    running = false;
//                    break;
//                }
//            }
//
//            // Enterキーによるモード切替処理
//            if (enterPressed) {
//                if (currentMode == MANUAL_SHIFT) {
//                    // 手動シフトモード → 列シフトモード
//                    currentMode = COLUMN_SHIFT;
//                    currentColumn = 0;
//                    columnOffsetX = 0;
//                    columnOffsetY = 0;
//                    std::cout << "列シフトモードに移行します。列 " << currentColumn + 1 << "/" << gridSize << " の調整を開始します。" << std::endl;
//                    std::cout << "列シフトモードではA/Dキーで左右に移動できます。" << std::endl;
//                    imageUpdated = true;
//                }
//                else if (currentMode == COLUMN_SHIFT) {
//                    // 現在の列のシフト量を保存
//                    columnShiftsX[currentColumn] = columnOffsetX;
//                    columnShiftsY[currentColumn] = 0; // Y方向は変更なし
//                    std::cout << "列 " << currentColumn + 1 << " のシフト量: X=" << columnOffsetX << "px を保存しました。" << std::endl;
//
//                    // 次の列へ
//                    currentColumn++;
//                    columnOffsetX = 0;
//
//                    // 列シフトモードから行シフトモードへの移行部分を修正
//                    if (currentColumn >= gridSize) {
//                        // すべての列の処理が完了→行シフトモードへ
//
//                        // 新しい基準画像を作成するために、最初からやり直す
//                        // まずオリジナルの未処理画像のクローンを作成
//                        cv::Mat adjusted_img = cv::Mat::zeros(img_window.size(), img_window.type());
//                        adjusted_img = cv::Scalar::all(255); // 白い背景
//
//                        // 黒い背景の要素画像群を再描画
//                        cv::Mat new_display_image = cv::Mat::zeros(cv::Size(displayImageSize, displayImageSize), CV_8UC3);
//
//                        // バッファサイズなどが変わっていないか確認し、再描画
//                        if (finalDisplaySize > displayImageSize) {
//                            new_display_image = cv::Mat::zeros(cv::Size(finalDisplaySize, finalDisplaySize), CV_8UC3);
//                        }
//
//                        // 各要素画像を再描画（列シフトの結果を適用）
//                        for (int i = 0; i < gridSize; i++) {
//                            for (int j = 0; j < gridSize; j++) {
//                                int startX = elementStartPosH[j];
//                                int startY = elementStartPosV[i];
//                                int centerX = startX + baseElementSize / 2;
//                                int centerY = startY + baseElementSize / 2;
//
//                                // 横線を描画（中央の白い横線）
//                                int lineStartX = startX;
//                                int lineEndX = startX + baseElementSize;
//
//                                //// 中央の横線を描画
//                                //for (int x = lineStartX; x < lineEndX; x++) {
//                                //    int y1 = centerY - 1;
//                                //    int y2 = centerY;
//
//                                //    if (y1 >= 0 && y1 < new_display_image.rows && x >= 0 && x < new_display_image.cols) {
//                                //        new_display_image.at<cv::Vec3b>(y1, x) = cv::Vec3b(255, 255, 255); // 白色
//                                //    }
//                                //    if (y2 >= 0 && y2 < new_display_image.rows && x >= 0 && x < new_display_image.cols) {
//                                //        new_display_image.at<cv::Vec3b>(y2, x) = cv::Vec3b(255, 255, 255); // 白色
//                                //    }
//                                //}
//
//                                // 列シフトを適用した縦線を描画 - 1px幅
//                                int shiftedX = centerX + columnShiftsX[j];
//                                for (int y = startY; y < startY + baseElementSize; y++) {
//                                    if (y >= 0 && y < new_display_image.rows && shiftedX >= 0 && shiftedX < new_display_image.cols) {
//                                        new_display_image.at<cv::Vec3b>(y, shiftedX) = cv::Vec3b(255, 255, 255); // 白色
//                                    }
//                                }
//
//                                // 中央の横線を描画 - 1px幅
//                                for (int x = lineStartX; x < lineEndX; x++) {
//                                    if (x >= 0 && x < new_display_image.cols && centerY >= 0 && centerY < new_display_image.rows) {
//                                        new_display_image.at<cv::Vec3b>(centerY, x) = cv::Vec3b(255, 255, 255); // 白色
//                                    }
//                                }
//                            }
//                        }
//
//                        // メイン表示ウィンドウに描画
//                        adjusted_img = cv::Scalar::all(255); // 白い背景
//
//                        for (int i = 0; i < copy_height; i++) {
//                            for (int j = 0; j < copy_width; j++) {
//                                int target_i = i + offsetY;
//                                int target_j = j + 720 + offsetX; // 横方向にセンタリング
//
//                                if (target_i >= 0 && target_i < adjusted_img.rows &&
//                                    target_j >= 0 && target_j < adjusted_img.cols) {
//                                    adjusted_img.at<cv::Vec3b>(target_i, target_j) = new_display_image.at<cv::Vec3b>(i, j);
//                                }
//                            }
//                        }
//
//                        // 調整済みの画像を新たな基準画像として設定
//                        original_img_window = adjusted_img.clone();
//
//                        //// 手動シフトを適用（基準画像に対してシフト）
//                        //if (x_shift != 0 || y_shift != 0) {
//                        //    shiftImage(original_img_window, x_shift, y_shift);
//                        //}
//
//                        // 行シフトモードに移行
//                        currentMode = ROW_SHIFT;
//                        currentRow = 0;
//                        rowOffsetX = 0;
//                        rowOffsetY = 0;
//                        std::cout << "すべての列の調整が完了しました。行シフトモードに移行します。" << std::endl;
//                        std::cout << "行 " << currentRow + 1 << "/" << gridSize << " の調整を開始します。" << std::endl;
//                        std::cout << "行シフトモードではW/Sキーで上下に移動できます。" << std::endl;
//                    }
//                    else {
//                        std::cout << "列 " << currentColumn + 1 << "/" << gridSize << " の調整を開始します。" << std::endl;
//                    }
//                    imageUpdated = true;
//                }
//                else if (currentMode == ROW_SHIFT) {
//                    // 現在の行のシフト量を保存
//                    rowShiftsX[currentRow] = 0; // X方向は変更なし
//                    rowShiftsY[currentRow] = rowOffsetY;
//                    std::cout << "行 " << currentRow + 1 << " のシフト量: Y=" << rowOffsetY << "px を保存しました。" << std::endl;
//
//                    // 次の行へ
//                    currentRow++;
//                    rowOffsetY = 0;
//
//                    // 行シフトモードが完了した後の処理を修正します
//                    if (currentRow >= gridSize) {
//                        // すべての行の処理が完了→調整済み画像を作成して保存
//                        std::cout << "すべての行の調整が完了しました。調整済み画像を作成します。" << std::endl;
//
//                        // 要素画像群のみを含む最終的な調整済み画像を作成
//                        // 最終的なディスプレイサイズを計算
//                        int finalWidth = (elementStartPosH[gridSize - 1] + baseElementSize) + 2 * offsetX; // 最後の要素の終わり位置 + オフセット
//                        int finalHeight = (elementStartPosV[gridSize - 1] + baseElementSize) + 2 * offsetY;
//
//                        // 要素画像群のみを含む画像（黒背景）
//                        cv::Mat final_display_image = cv::Mat::zeros(cv::Size(finalWidth, finalHeight), CV_8UC3);
//
//                        // 各要素画像を再描画（黒い背景、白い線）
//                        for (int i = 0; i < gridSize; i++) {
//                            for (int j = 0; j < gridSize; j++) {
//                                int startX = elementStartPosH[j] + offsetX;
//                                int startY = elementStartPosV[i] + offsetY;
//                                int centerX = startX + baseElementSize / 2;
//                                int centerY = startY + baseElementSize / 2;
//
//                                // 各要素画像の黒背景部分を描画
//                                cv::Rect elementRect(startX, startY, baseElementSize, baseElementSize);
//                                cv::rectangle(final_display_image, elementRect, cv::Scalar(0, 0, 0), -1); // -1は塗りつぶし
//
//                                // 列シフトを適用した縦線を描画（線幅1pxに変更）
//                                int shiftedX = centerX + columnShiftsX[j];
//                                cv::line(final_display_image,
//                                    cv::Point(shiftedX, startY),
//                                    cv::Point(shiftedX, startY + baseElementSize - 1),
//                                    cv::Scalar(255, 255, 255), 1);  // 線幅を1pxに変更
//
//                                // 行シフトを適用した横線を描画（線幅1pxに変更）
//                                int shiftedY = centerY + rowShiftsY[i];
//                                cv::line(final_display_image,
//                                    cv::Point(startX, shiftedY),
//                                    cv::Point(startX + baseElementSize - 1, shiftedY),
//                                    cv::Scalar(255, 255, 255), 1);  // 線幅を1pxに変更
//                            }
//                        }
//
//                        // 画像を保存
//                        std::string filename = "adjusted_grid_" + std::to_string(gridSize) + ".png";
//                        cv::imwrite(filename, final_display_image);
//                        std::cout << "調整済み画像を " << filename << " に保存しました。" << std::endl;
//
//                        // CSVファイルに保存
//                        saveAllArraysToCsv(columnShiftsX, columnShiftsY, rowShiftsX, rowShiftsY,
//                            x_shift, y_shift, "shift_data_v2.csv");
//
//                        // 手動シフトモードに戻る
//                        currentMode = MANUAL_SHIFT;
//                        std::cout << "手動シフトモードに戻ります。" << std::endl;
//                    }
//                    else {
//                        std::cout << "行 " << currentRow + 1 << "/" << gridSize << " の調整を開始します。" << std::endl;
//                    }
//
//
//                    imageUpdated = true;
//                }
//            }
//
//
//        }
//    }
//
//    return 0;
//}

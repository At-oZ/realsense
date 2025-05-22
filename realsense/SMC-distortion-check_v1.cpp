// ピンホールカメラアレイを用いて表示画像を作成するプログラム
/*
    ideal_v2からの派生
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <Windows.h>
#include <Xinput.h>

// XInputのヘッダとライブラリをリンク
#pragma comment(lib, "XInput.lib")

// コントローラー入力のチェック関数
bool ProcessXInput(int& x_shift, int& y_shift, int shift_amount,
    bool& focal_length_mode, double& focalLength, double focal_length_step,
    bool& tile_size_mode, double& tileSize, double tile_size_step,
    bool& tile_z_mode, double& tileZ, double tile_z_step,
    bool& continue_key_loop, bool& running) {
    // コントローラーの状態を取得
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // コントローラー0の状態を取得（最初に接続されたコントローラー）
    DWORD result = XInputGetState(0, &state);

    // コントローラーが接続されていない場合
    if (result != ERROR_SUCCESS) {
        return false;
    }

    // 十字キーの状態を取得
    bool changed = false;

    // DPADの状態を確認
    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
        // 上キー - モードに応じた処理
        if (focal_length_mode) {
            // ギャップ調整モードの場合はギャップを増加
            focalLength += focal_length_step;
            std::cout << "ギャップを増加: " << focalLength << " mm" << std::endl;
            continue_key_loop = false;
            changed = true;
        }
        else if (tile_size_mode) {
            // タイルサイズ調整モードの場合はタイルサイズを増加
            tileSize += tile_size_step;
            std::cout << "タイルサイズを増加: " << tileSize << std::endl;
            continue_key_loop = false;
            changed = true;
        }
        else if (tile_z_mode) {
            // タイル位置調整モードの場合はタイル位置を増加
            tileZ += tile_z_step;
            std::cout << "タイル位置を増加: " << tileZ << " mm" << std::endl;
            continue_key_loop = false;
            changed = true;
        }
        else {
            // 通常モードの場合は上移動
            y_shift -= shift_amount;
            std::cout << "上に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
            changed = true;
        }
    }

    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
        // 下キー - モードに応じた処理
        if (focal_length_mode) {
            // ギャップ調整モードの場合はギャップを減少
            if (focalLength > focal_length_step) {
                focalLength -= focal_length_step;
                std::cout << "ギャップを減少: " << focalLength << " mm" << std::endl;
            }
            else {
                std::cout << "ギャップが最小値に達しています" << std::endl;
            }
            continue_key_loop = false;
            changed = true;
        }
        else if (tile_size_mode) {
            // タイルサイズ調整モードの場合はタイルサイズを減少
            if (tileSize > tile_size_step) {
                tileSize -= tile_size_step;
                std::cout << "タイルサイズを減少: " << tileSize << std::endl;
            }
            else {
                std::cout << "タイルサイズが最小値に達しています" << std::endl;
            }
            continue_key_loop = false;
            changed = true;
        }
        else if (tile_z_mode) {
            // タイル位置調整モードの場合はタイル位置を減少
            if (tileZ > tile_z_step) {
                tileZ -= tile_z_step;
                std::cout << "タイル位置を減少: " << tileZ << " mm" << std::endl;
            }
            else {
                std::cout << "タイル位置が最小値に達しています" << std::endl;
            }
            continue_key_loop = false;
            changed = true;
        }
        else {
            // 通常モードの場合は下移動
            y_shift += shift_amount;
            std::cout << "下に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
            changed = true;
        }
    }

    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
        // 左キー - 左移動（調整モード中は無効）
        if (!focal_length_mode && !tile_size_mode && !tile_z_mode) {
            x_shift -= shift_amount;
            std::cout << "左に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
            changed = true;
        }
    }

    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
        // 右キー - 右移動（調整モード中は無効）
        if (!focal_length_mode && !tile_size_mode && !tile_z_mode) {
            x_shift += shift_amount;
            std::cout << "右に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
            changed = true;
        }
    }

    // Aボタン - ギャップ調整モードの切り替え（Fキーと同じ）
    if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
        static ULONGLONG lastAButtonPressTime = 0;
        ULONGLONG currentTime = GetTickCount64();

        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
        if (currentTime - lastAButtonPressTime > 300) {
            focal_length_mode = !focal_length_mode;
            if (focal_length_mode) {
                tile_size_mode = false; // タイルサイズモードを終了
                tile_z_mode = false;    // タイル位置モードを終了
                std::cout << "ギャップ調整モードに切り替えました。十字キー上下でギャップを調整します。現在値: " << focalLength << " mm (ステップ: " << focal_length_step << " mm)" << std::endl;
            }
            else {
                std::cout << "ギャップ調整モードを終了します。" << std::endl;
            }
            lastAButtonPressTime = currentTime;
            changed = true;
        }
    }

    // Xボタン - タイルサイズ調整モードの切り替え（Tキーと同じ）
    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
        static ULONGLONG lastXButtonPressTime = 0;
        ULONGLONG currentTime = GetTickCount64();

        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
        if (currentTime - lastXButtonPressTime > 300) {
            tile_size_mode = !tile_size_mode;
            if (tile_size_mode) {
                focal_length_mode = false; // ギャップ調整モードを終了
                tile_z_mode = false;       // タイル位置モードを終了
                std::cout << "タイルサイズ調整モードに切り替えました。十字キー上下でタイルサイズを調整します。現在値: " << tileSize << " (ステップ: " << tile_size_step << ")" << std::endl;
            }
            else {
                std::cout << "タイルサイズ調整モードを終了します。" << std::endl;
            }
            lastXButtonPressTime = currentTime;
            changed = true;
        }
    }

    // Yボタン - タイル位置調整モードの切り替え（Zキーと同じ）
    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) {
        static ULONGLONG lastYButtonPressTime = 0;
        ULONGLONG currentTime = GetTickCount64();

        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
        if (currentTime - lastYButtonPressTime > 300) {
            tile_z_mode = !tile_z_mode;
            if (tile_z_mode) {
                focal_length_mode = false; // ギャップ調整モードを終了
                tile_size_mode = false;    // タイルサイズモードを終了
                std::cout << "タイル位置調整モードに切り替えました。十字キー上下でタイル位置を調整します。現在値: " << tileZ << " mm (ステップ: " << tile_z_step << " mm)" << std::endl;
            }
            else {
                std::cout << "タイル位置調整モードを終了します。" << std::endl;
            }
            lastYButtonPressTime = currentTime;
            changed = true;
        }
    }

    // Bボタン - リセット（Rキーと同じ）
    if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
        static ULONGLONG lastBButtonPressTime = 0;
        ULONGLONG currentTime = GetTickCount64();

        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
        if (currentTime - lastBButtonPressTime > 300) {
            x_shift = 0;
            y_shift = 0;
            std::cout << "位置をリセットしました。" << std::endl;
            lastBButtonPressTime = currentTime;
            changed = true;
        }
    }

    // バックボタン（またはビューボタン） - 終了（ESCと同じ）
    if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
        static ULONGLONG lastBackButtonPressTime = 0;
        ULONGLONG currentTime = GetTickCount64();

        // ボタン連打防止 (300ミリ秒以内の連続押下を無視)
        if (currentTime - lastBackButtonPressTime > 300) {
            focal_length_mode = false;
            tile_size_mode = false;
            tile_z_mode = false;
            continue_key_loop = false;
            running = false;
            lastBackButtonPressTime = currentTime;
            changed = true;
        }
    }

    return changed;
}

// 画像シフト用関数
void shiftImage(cv::Mat& image, int dx, int dy) {
    cv::Mat result = cv::Mat::zeros(image.size(), image.type());
    result.setTo(cv::Scalar::all(255)); // 背景を白に設定

    // シフト後の有効範囲を計算
    int startX = (std::max)(0, dx);
    int startY = (std::max)(0, dy);
    int endX = (std::min)(image.cols, image.cols + dx);
    int endY = (std::min)(image.rows, image.rows + dy);
    int width = endX - startX;
    int height = endY - startY;

    if (width <= 0 || height <= 0) return; // 有効範囲がない場合は何もしない

    // 元画像の対応する部分をコピー
    cv::Rect srcRect(startX - dx, startY - dy, width, height);
    cv::Rect dstRect(startX, startY, width, height);

    image(srcRect).copyTo(result(dstRect));
    result.copyTo(image);
}

int main() {
    std::cout << "ICIP-ideal-v1" << std::endl;

    int nphs[20] = { 4, 5, 6, 8, 10, 12, 15, 20, 24, 25, 30, 40, 50, 60, 75, 100, 120, 125, 150, 200 };

    std::string WINNAME = "image";
    cv::namedWindow(WINNAME);
    HWND window = FindWindow(NULL, L"image");
    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP);
    SetWindowPos(window, NULL, 2560, 0, 3840, 2420, SWP_DRAWFRAME | SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    cv::Mat img_window = cv::Mat::zeros(cv::Size(3840, 2400), CV_8UC3);
    img_window = cv::Scalar::all(255);

    // 画像シフト用の変数
    cv::Mat original_img_window;
    int x_shift = 0, y_shift = 0;

    // シフト量設定用の変数
    int shift_amount = 1200;
    bool input_mode = false;
    std::string input_buffer = "";

    // ギャップ調整モード用の変数
    bool focal_length_mode = false;
    double focalLength = 13.0; // 初期値
    double focal_length_step = 0.1; // 増減ステップの初期値
    bool focal_length_input_mode = false; // ギャップ増減量入力モード

    // タイルサイズ調整モード用の変数
    bool tile_size_mode = false;
    double tileSize = 203.0; // 初期値
    double tile_size_step = 1.0; // 増減ステップの初期値
    bool tile_size_input_mode = false; // タイルサイズ増減量入力モード

    // タイル位置調整モード用の変数
    bool tile_z_mode = false;
    double tileZ = 500; // 初期値
    double tile_z_step = 10.0; // 増減ステップの初期値（1mm）
    bool tile_z_input_mode = false; // タイル位置増減量入力モード

    int nph_idx = 11; // 初期値 (nphs[11] = 40)

    // コントローラーのポーリング間隔 (ms)
    const int controllerPollInterval = 50;
    ULONGLONG lastPollTime = 0;

    bool running = true;
    while (running) {
        int nph = nphs[nph_idx];

        std::cout << "number of pins:" << nph << std::endl;

        double zo_min = 1000.0; // 最短視認距離(mm)
        // ピクセルサイズの計算
        const double coef = 1.0;
        const double pixelSize = 13.4 / std::sqrt(3840 * 3840 + 2400 * 2400) * 25.4 / coef;
        std::cout << "camera pixel size:" << pixelSize << std::endl;

        // ディスプレイの設定
        const int displayImageSize = 2400 * coef;
        const double displayAreaSize = displayImageSize * pixelSize;
        cv::Mat displayImage = cv::Mat::zeros(cv::Size(displayImageSize, displayImageSize), CV_8UC3);

        // タイルの設定
        const int tileResolution = 256; // タイルの解像度（ピクセル）

        // カメラのグリッド設定
        const int gridSize = nph; // グリッドサイズ
        const double gridSpacing = displayAreaSize / gridSize; // グリッド間隔（mm）
        const double gridOriginOffset = -((gridSize - 1) * gridSpacing) / 2.0; // グリッドの中心を原点に合わせるためのオフセット

        // カメラの設定
        // ギャップ調整用にfocalLengthを変数化
        std::cout << "Focal length (gap): " << focalLength << " mm (調整ステップ: " << focal_length_step << " mm)" << std::endl;
        std::cout << "Tile size: " << tileSize << " (調整ステップ: " << tile_size_step << ")" << std::endl;
        std::cout << "Tile Z: " << tileZ << " mm (調整ステップ: " << tile_z_step << " mm)" << std::endl;

        const double sensorSize = gridSpacing; // センサーサイズ（mm）
        const int imageResolution = static_cast<int>(floor(displayImageSize / gridSize)); // 要素画像の解像度（ピクセル）

        // 画像中心からのオフセット計算用
        const double halfSensorSize = sensorSize / 2.0;
        const double intv = sensorSize / pixelSize;

        // 現在のシフト位置を保存
        int stored_x_shift = x_shift;
        int stored_y_shift = y_shift;

        // タイル画像の読み込み
        cv::Mat tileImage = cv::imread("./images/standard/Lenna.bmp");
        if (tileImage.empty()) {
            std::cerr << "タイル画像が見つかりません。" << std::endl;
            return -1;
        }
        cv::resize(tileImage, tileImage, cv::Size(tileResolution, tileResolution));

        std::cout << "tileZ:" << tileZ << std::endl;

        double tilePixelSize = tileSize / tileResolution;

        // カメラ位置の計算
        std::vector<cv::Point2d> cameraPositions;
        for (int i = 0; i < gridSize; ++i)
        {
            for (int j = 0; j < gridSize; ++j)
            {
                double camPosX = (gridOriginOffset + j * gridSpacing);
                double camPosY = (gridOriginOffset + i * gridSpacing);
                cameraPositions.emplace_back(camPosX, camPosY);
            }
        }

        // ピクセルの位置を計算
        std::vector<cv::Point2d> pixelPositions;
        double offset = -((imageResolution - 1) * pixelSize) / 2.0;

        for (int i = 0; i < imageResolution; ++i)
        {
            for (int j = 0; j < imageResolution; ++j)
            {
                double x = (offset + j * pixelSize);  // x軸はそのまま
                double y = (offset + i * pixelSize);  // y軸の反転
                pixelPositions.emplace_back(x, y);
            }
        }

        // 各カメラについて計算
        int cameraIndex = 0;
        int startu, startv;
        for (int m = 0; m < gridSize; ++m) {
            for (int n = 0; n < gridSize; ++n) {
                // 出力画像の初期化
                cv::Mat outputImage(imageResolution, imageResolution, CV_8UC3, cv::Scalar(0, 0, 0));

                int pixelIndex = 0;
                const auto& cameraPos = cameraPositions[cameraIndex++];
                startu = static_cast<int>(std::round((cameraPos.x + displayAreaSize / 2 - halfSensorSize) / pixelSize));
                startv = static_cast<int>(std::round((cameraPos.y + displayAreaSize / 2 - halfSensorSize) / pixelSize));

                // 各ピクセルについて計算
                for (int i = 0; i < imageResolution; ++i) {
                    for (int j = 0; j < imageResolution; ++j) {
                        const auto& pixelPos = pixelPositions[pixelIndex++];

                        // 1ピクセルから見えるタイル上の領域を計算
                        // 投影線を通してピクセルからタイルへの射影を計算
                        double scale = tileZ / focalLength; // z=0からz=tileZへの拡大率
                        double viewX = pixelPos.x * scale + cameraPos.x;
                        double viewY = pixelPos.y * scale + cameraPos.y;

                        // タイル上での可視領域（ピンホールの開口を考慮）
                        double pixelHalf = pixelSize / 2.0;
                        double viewPixelHalf = pixelHalf * scale;

                        double visibleAreaMinX = viewX - viewPixelHalf;
                        double visibleAreaMaxX = viewX + viewPixelHalf;
                        double visibleAreaMinY = viewY - viewPixelHalf;
                        double visibleAreaMaxY = viewY + viewPixelHalf;

                        // タイルの範囲内に制限
                        visibleAreaMinX = (std::max)(-tileSize / 2.0, visibleAreaMinX);
                        visibleAreaMaxX = (std::min)(tileSize / 2.0, visibleAreaMaxX);
                        visibleAreaMinY = (std::max)(-tileSize / 2.0, visibleAreaMinY);
                        visibleAreaMaxY = (std::min)(tileSize / 2.0, visibleAreaMaxY);

                        // 可視領域がタイル内に存在しない場合
                        if (visibleAreaMinX >= visibleAreaMaxX || visibleAreaMinY >= visibleAreaMaxY)
                        {
                            // 黒色を設定
                            outputImage.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                            continue;
                        }

                        // ピクセルインデックスの範囲を計算
                        int xMinIdx = (std::max)(0, static_cast<int>(std::floor((visibleAreaMinX + tileSize / 2) / tilePixelSize)));
                        int xMaxIdx = (std::min)(tileResolution - 1, static_cast<int>(std::floor((visibleAreaMaxX + tileSize / 2) / tilePixelSize)));
                        int yMinIdx = (std::max)(0, static_cast<int>(std::floor((visibleAreaMinY + tileSize / 2) / tilePixelSize)));
                        int yMaxIdx = (std::min)(tileResolution - 1, static_cast<int>(std::floor((visibleAreaMaxY + tileSize / 2) / tilePixelSize)));

                        // 観察される色の加重平均を計算
                        cv::Vec3d weighted_color(0, 0, 0);
                        double total_area = 0.0;

                        for (int xIdx = xMinIdx; xIdx <= xMaxIdx; ++xIdx)
                        {
                            // ピクセルのx座標範囲を計算
                            double xPixelMin = xIdx * tilePixelSize - tileSize / 2;
                            double xPixelMax = (xIdx + 1) * tilePixelSize - tileSize / 2;

                            double xOverlap = (std::max)(0.0, (std::min)(visibleAreaMaxX, xPixelMax) - (std::max)(visibleAreaMinX, xPixelMin));
                            if (xOverlap <= 0) continue;

                            for (int yIdx = yMinIdx; yIdx <= yMaxIdx; ++yIdx)
                            {
                                // ピクセルのy座標範囲を計算
                                double yPixelMin = yIdx * tilePixelSize - tileSize / 2;
                                double yPixelMax = (yIdx + 1) * tilePixelSize - tileSize / 2;

                                double yOverlap = (std::max)(0.0, (std::min)(visibleAreaMaxY, yPixelMax) - (std::max)(visibleAreaMinY, yPixelMin));
                                if (yOverlap <= 0) continue;

                                double overlap_area = xOverlap * yOverlap;
                                total_area += overlap_area;

                                // ピクセル値を取得し、加重平均に加算
                                cv::Vec3b pixel = tileImage.at<cv::Vec3b>(yIdx, xIdx);
                                weighted_color += cv::Vec3d(pixel) * overlap_area;
                            }
                        }

                        cv::Vec3b final_color;
                        if (total_area > 0)
                        {
                            weighted_color /= total_area;
                            final_color = cv::Vec3b(
                                static_cast<uchar>(weighted_color[0]),
                                static_cast<uchar>(weighted_color[1]),
                                static_cast<uchar>(weighted_color[2])
                            );
                        }
                        else
                        {
                            // 観察可能な領域がない場合は黒色
                            final_color = cv::Vec3b(0, 0, 0);
                        }

                        // 出力画像に色を設定
                        outputImage.at<cv::Vec3b>(i, j) = final_color;
                    }
                }

                for (int i = 0; i < imageResolution; ++i) {
                    for (int j = 0; j < imageResolution; ++j) {
                        displayImage.at<cv::Vec3b>(startv + i, startu + j) = outputImage.at<cv::Vec3b>(i, j);
                    }
                }
            }
        }

        int shift_y = 0;
        for (int i = shift_y; i < 2400; ++i) {
            for (int j = 720; j < 3120; ++j) {
                img_window.at<cv::Vec3b>(i - shift_y, j)[0] = displayImage.at<cv::Vec3b>(i, j - 720)[0];
                img_window.at<cv::Vec3b>(i - shift_y, j)[1] = displayImage.at<cv::Vec3b>(i, j - 720)[1];
                img_window.at<cv::Vec3b>(i - shift_y, j)[2] = displayImage.at<cv::Vec3b>(i, j - 720)[2];
            }
        }

        cv::imwrite("./SMC-distortion-check.png", displayImage);

        // オリジナルの画像を保存
        original_img_window = img_window.clone();

        // シフト位置を復元して適用
        x_shift = stored_x_shift;
        y_shift = stored_y_shift;
        if (x_shift != 0 || y_shift != 0) {
            shiftImage(img_window, x_shift, y_shift);
            std::cout << "シフト位置: x=" << x_shift << ", y=" << y_shift << std::endl;
        }

        // 画像を表示
        cv::imshow(WINNAME, img_window);

        // キー入力ループ
        bool continue_key_loop = true;
        double prev_focal_length = focalLength;
        double prev_tile_size = tileSize;
        double prev_tile_z = tileZ;
        bool display_updated = false;

        // モード表示用フラグ
        bool initial_mode_display = true;
        std::string last_input_buffer = "";

        int key;
        while (continue_key_loop) {
            // 入力モード表示用の画像を作成
            cv::Mat display_img = img_window.clone();

            // モード切替時または入力バッファ変更時に画面を更新
            if ((input_mode || focal_length_input_mode || tile_size_input_mode || tile_z_input_mode) && last_input_buffer != input_buffer) {
                cv::Mat display_img = img_window.clone();

                if (input_mode) {
                    // シフト量入力モードの表示
                    std::cout << "シフト量入力モード: " << input_buffer << "_" << std::endl;
                    std::string display_text = "シフト量: " + input_buffer + "_";
                    cv::putText(display_img, display_text, cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2);
                }
                else if (focal_length_input_mode) {
                    // ギャップ増減量入力モードの表示
                    std::cout << "ギャップ増減量入力モード: " << input_buffer << "_" << std::endl;
                    std::string display_text = "ギャップ増減量: " + input_buffer + "_";
                    cv::putText(display_img, display_text, cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
                }
                else if (tile_size_input_mode) {
                    // タイルサイズ増減量入力モードの表示
                    std::cout << "タイルサイズ増減量入力モード: " << input_buffer << "_" << std::endl;
                    std::string display_text = "タイルサイズ増減量: " + input_buffer + "_";
                    cv::putText(display_img, display_text, cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0), 2);
                }
                else if (tile_z_input_mode) {
                    // タイル位置増減量入力モードの表示
                    std::cout << "タイル位置増減量入力モード: " << input_buffer << "_" << std::endl;
                    std::string display_text = "タイル位置増減量: " + input_buffer + "_";
                    cv::putText(display_img, display_text, cv::Point(50, 50),
                        cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);
                }

                cv::imshow(WINNAME, display_img);
                last_input_buffer = input_buffer;
                display_updated = true;
                initial_mode_display = false;
            }
            else if (focal_length_mode && initial_mode_display) {
                // ギャップ調整モードの初期表示
                cv::Mat display_img = img_window.clone();
                std::string display_text = "ギャップ調整モード: " + std::to_string(focalLength) + " mm (ステップ: " + std::to_string(focal_length_step) + " mm)";
                cv::putText(display_img, display_text, cv::Point(50, 50),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

                cv::imshow(WINNAME, display_img);
                display_updated = true;
                initial_mode_display = false;
            }
            else if (tile_size_mode && initial_mode_display) {
                // タイルサイズ調整モードの初期表示
                cv::Mat display_img = img_window.clone();
                std::string display_text = "タイルサイズ調整モード: " + std::to_string(tileSize) + " (ステップ: " + std::to_string(tile_size_step) + ")";
                cv::putText(display_img, display_text, cv::Point(50, 50),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0), 2);

                cv::imshow(WINNAME, display_img);
                display_updated = true;
                initial_mode_display = false;
            }
            else if (tile_z_mode && initial_mode_display) {
                // タイル位置調整モードの初期表示
                cv::Mat display_img = img_window.clone();
                std::string display_text = "タイル位置調整モード: " + std::to_string(tileZ) + " mm (ステップ: " + std::to_string(tile_z_step) + " mm)";
                cv::putText(display_img, display_text, cv::Point(50, 50),
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);

                cv::imshow(WINNAME, display_img);
                display_updated = true;
                initial_mode_display = false;
            }

            // モード切替検出
            if (!input_mode && !focal_length_input_mode && !tile_size_input_mode && !tile_z_input_mode
                && !focal_length_mode && !tile_size_mode && !tile_z_mode) {
                // 全モード終了時にフラグをリセット
                initial_mode_display = true;
            }

            // 入力モード中はコントローラー入力を無視
            if (!input_mode && !focal_length_input_mode && !tile_size_input_mode && !tile_z_input_mode) {
                // 現在の時間を取得
                ULONGLONG currentTime = GetTickCount64();

                // 一定間隔でコントローラー入力をチェック
                if (currentTime - lastPollTime > controllerPollInterval) {
                    // コントローラー入力の処理
                    bool controller_changed = ProcessXInput(x_shift, y_shift, shift_amount,
                        focal_length_mode, focalLength, focal_length_step,
                        tile_size_mode, tileSize, tile_size_step,
                        tile_z_mode, tileZ, tile_z_step,
                        continue_key_loop, running);

                    // 入力が変更された場合、画像を更新
                    if (controller_changed && !continue_key_loop) {
                        // 値が変わった場合は処理を抜ける
                        break;
                    }
                    else if (controller_changed) {
                        img_window = original_img_window.clone();
                        shiftImage(img_window, x_shift, y_shift);
                        cv::imshow(WINNAME, img_window);
                    }

                    lastPollTime = currentTime;
                }
            }

            // キーボード入力をチェック (非ブロッキング)
            key = cv::waitKey(1);

            // キーが押されていない場合
            if (key == -1) {
                // 短い待機時間を挿入してCPU使用率を抑制
                Sleep(10);
                continue;
            }

            // 入力モード中の処理
            if (input_mode) {
                if (key >= '0' && key <= '9' || key == '.') {
                    // 数字キーまたはドットが押されたら入力バッファに追加
                    input_buffer += (char)key;
                }
                else if (key == 8) { // バックスペース
                    // バッファから1文字削除
                    if (!input_buffer.empty()) {
                        input_buffer.pop_back();
                    }
                }
                else if (key == 13) { // Enter
                    // 入力確定、シフト量を設定
                    if (!input_buffer.empty()) {
                        shift_amount = std::stoi(input_buffer);
                        std::cout << "シフト量を " << shift_amount << " に設定しました。" << std::endl;
                    }
                    input_mode = false;
                    input_buffer = "";
                }
                else if (key == 27) { // ESC
                    // 入力モードをキャンセル
                    input_mode = false;
                    input_buffer = "";
                }
                continue;
            }

            // ギャップ増減量入力モード中の処理
            if (focal_length_input_mode) {
                if (key >= '0' && key <= '9' || key == '.') {
                    // 数字キーまたはドットが押されたら入力バッファに追加
                    input_buffer += (char)key;
                }
                else if (key == 8) { // バックスペース
                    // バッファから1文字削除
                    if (!input_buffer.empty()) {
                        input_buffer.pop_back();
                    }
                }
                else if (key == 13) { // Enter
                    // 入力確定、ギャップ増減量を設定
                    if (!input_buffer.empty()) {
                        focal_length_step = std::stod(input_buffer);
                        std::cout << "ギャップ増減量を " << focal_length_step << " mm に設定しました。" << std::endl;
                    }
                    focal_length_input_mode = false;
                    input_buffer = "";
                }
                else if (key == 27) { // ESC
                    // 入力モードをキャンセル
                    focal_length_input_mode = false;
                    input_buffer = "";
                }
                continue;
            }

            // タイルサイズ増減量入力モード中の処理
            if (tile_size_input_mode) {
                if (key >= '0' && key <= '9' || key == '.') {
                    // 数字キーまたはドットが押されたら入力バッファに追加
                    input_buffer += (char)key;
                }
                else if (key == 8) { // バックスペース
                    // バッファから1文字削除
                    if (!input_buffer.empty()) {
                        input_buffer.pop_back();
                    }
                }
                else if (key == 13) { // Enter
                    // 入力確定、タイルサイズ増減量を設定
                    if (!input_buffer.empty()) {
                        tile_size_step = std::stod(input_buffer);
                        std::cout << "タイルサイズ増減量を " << tile_size_step << " に設定しました。" << std::endl;
                    }
                    tile_size_input_mode = false;
                    input_buffer = "";
                }
                else if (key == 27) { // ESC
                    // 入力モードをキャンセル
                    tile_size_input_mode = false;
                    input_buffer = "";
                }
                continue;
            }

            // タイル位置増減量入力モード中の処理
            if (tile_z_input_mode) {
                if (key >= '0' && key <= '9' || key == '.') {
                    // 数字キーまたはドットが押されたら入力バッファに追加
                    input_buffer += (char)key;
                }
                else if (key == 8) { // バックスペース
                    // バッファから1文字削除
                    if (!input_buffer.empty()) {
                        input_buffer.pop_back();
                    }
                }
                else if (key == 13) { // Enter
                    // 入力確定、タイル位置増減量を設定
                    if (!input_buffer.empty()) {
                        tile_z_step = std::stod(input_buffer);
                        std::cout << "タイル位置増減量を " << tile_z_step << " mm に設定しました。" << std::endl;
                    }
                    tile_z_input_mode = false;
                    input_buffer = "";
                }
                else if (key == 27) { // ESC
                    // 入力モードをキャンセル
                    tile_z_input_mode = false;
                    input_buffer = "";
                }
                continue;
            }

            // ギャップ調整モード中の処理
            if (focal_length_mode) {
                switch (key) {
                case 'w': // ギャップを増加
                case 'W':
                    focalLength += focal_length_step;
                    std::cout << "ギャップを増加: " << focalLength << " mm" << std::endl;
                    // 再処理のため、ループを抜ける
                    continue_key_loop = false;
                    break;

                case 's': // ギャップを減少
                case 'S':
                    if (focalLength > focal_length_step) { // 0より小さくならないように
                        focalLength -= focal_length_step;
                        std::cout << "ギャップを減少: " << focalLength << " mm" << std::endl;
                    }
                    else {
                        std::cout << "ギャップが最小値に達しています" << std::endl;
                    }
                    // 再処理のため、ループを抜ける
                    continue_key_loop = false;
                    break;

                case 'i': // ギャップ増減量設定モード
                case 'I':
                    focal_length_input_mode = true;
                    input_buffer = "";
                    std::cout << "ギャップ増減量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
                    break;

                case 'f': // ギャップ調整モードを終了
                case 'F':
                    focal_length_mode = false;
                    std::cout << "ギャップ調整モードを終了します。" << std::endl;
                    // 値が変わっていたら再処理
                    if (prev_focal_length != focalLength) {
                        continue_key_loop = false;
                    }
                    break;

                case 'l': // モード切替
                case 'L':
                    focal_length_mode = false;
                    tile_size_mode = true;
                    std::cout << "タイルサイズ調整モードに切り替えました。W/Sキーでタイルサイズを調整します。現在値: " << tileSize << " (ステップ: " << tile_size_step << ")" << std::endl;
                    initial_mode_display = true; // 表示を更新するためのフラグを設定
                    break;

                case 'z': // タイル位置調整モードに切り替え
                case 'Z':
                    focal_length_mode = false;
                    tile_z_mode = true;
                    std::cout << "タイル位置調整モードに切り替えました。W/Sキーでタイル位置を調整します。現在値: " << tileZ << " mm (ステップ: " << tile_z_step << " mm)" << std::endl;
                    initial_mode_display = true; // 表示を更新するためのフラグを設定
                    break;

                case 27: // ESC: 終了
                    focal_length_mode = false;
                    continue_key_loop = false;
                    running = false;
                    break;

                default:
                    break;
                }
                continue;
            }

            // タイルサイズ調整モード中の処理
            if (tile_size_mode) {
                switch (key) {
                case 'w': // タイルサイズを増加
                case 'W':
                    tileSize += tile_size_step;
                    std::cout << "タイルサイズを増加: " << tileSize << std::endl;
                    // 再処理のため、ループを抜ける
                    continue_key_loop = false;
                    break;

                case 's': // タイルサイズを減少
                case 'S':
                    if (tileSize > tile_size_step) { // 0より小さくならないように
                        tileSize -= tile_size_step;
                        std::cout << "タイルサイズを減少: " << tileSize << std::endl;
                    }
                    else {
                        std::cout << "タイルサイズが最小値に達しています" << std::endl;
                    }
                    // 再処理のため、ループを抜ける
                    continue_key_loop = false;
                    break;

                case 'i': // タイルサイズ増減量設定モード
                case 'I':
                    tile_size_input_mode = true;
                    input_buffer = "";
                    std::cout << "タイルサイズ増減量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
                    break;

                case 't': // タイルサイズ調整モードを終了
                case 'T':
                    tile_size_mode = false;
                    std::cout << "タイルサイズ調整モードを終了します。" << std::endl;
                    // 値が変わっていたら再処理
                    if (prev_tile_size != tileSize) {
                        continue_key_loop = false;
                    }
                    break;

                case 'l': // モード切替
                case 'L':
                    tile_size_mode = false;
                    tile_z_mode = true;
                    std::cout << "タイル位置調整モードに切り替えました。W/Sキーでタイル位置を調整します。現在値: " << tileZ << " mm (ステップ: " << tile_z_step << " mm)" << std::endl;
                    initial_mode_display = true; // 表示を更新するためのフラグを設定
                    break;

                case 'z': // タイル位置調整モードに切り替え
                case 'Z':
                    tile_size_mode = false;
                    tile_z_mode = true;
                    std::cout << "タイル位置調整モードに切り替えました。W/Sキーでタイル位置を調整します。現在値: " << tileZ << " mm (ステップ: " << tile_z_step << " mm)" << std::endl;
                    initial_mode_display = true; // 表示を更新するためのフラグを設定
                    break;

                case 27: // ESC: 終了
                    tile_size_mode = false;
                    continue_key_loop = false;
                    running = false;
                    break;

                default:
                    break;
                }
                continue;
            }

            // タイル位置調整モード中の処理
            if (tile_z_mode) {
                switch (key) {
                case 'w': // タイル位置を増加
                case 'W':
                    tileZ += tile_z_step;
                    std::cout << "タイル位置を増加: " << tileZ << " mm" << std::endl;
                    // 再処理のため、ループを抜ける
                    continue_key_loop = false;
                    break;

                case 's': // タイル位置を減少
                case 'S':
                    if (tileZ > tile_z_step) { // 0より小さくならないように
                        tileZ -= tile_z_step;
                        std::cout << "タイル位置を減少: " << tileZ << " mm" << std::endl;
                    }
                    else {
                        std::cout << "タイル位置が最小値に達しています" << std::endl;
                    }
                    // 再処理のため、ループを抜ける
                    continue_key_loop = false;
                    break;

                case 'i': // タイル位置増減量設定モード
                case 'I':
                    tile_z_input_mode = true;
                    input_buffer = "";
                    std::cout << "タイル位置増減量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
                    break;

                case 'z': // タイル位置調整モードを終了
                case 'Z':
                    tile_z_mode = false;
                    std::cout << "タイル位置調整モードを終了します。" << std::endl;
                    // 値が変わっていたら再処理
                    if (prev_tile_z != tileZ) {
                        continue_key_loop = false;
                    }
                    break;

                case 'l': // モード切替
                case 'L':
                    tile_z_mode = false;
                    focal_length_mode = true;
                    std::cout << "ギャップ調整モードに切り替えました。W/Sキーでギャップを調整します。現在値: " << focalLength << " mm (ステップ: " << focal_length_step << " mm)" << std::endl;
                    initial_mode_display = true; // 表示を更新するためのフラグを設定
                    break;

                case 27: // ESC: 終了
                    tile_z_mode = false;
                    continue_key_loop = false;
                    running = false;
                    break;

                default:
                    break;
                }
                continue;
            }

            // 通常モード中の処理
            switch (key) {
            case 'w': // 上へ移動
            case 'W':
                y_shift -= shift_amount;
                std::cout << "上に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
                img_window = original_img_window.clone();
                shiftImage(img_window, x_shift, y_shift);
                cv::imshow(WINNAME, img_window);
                break;

            case 's': // 下へ移動
            case 'S':
                y_shift += shift_amount;
                std::cout << "下に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
                img_window = original_img_window.clone();
                shiftImage(img_window, x_shift, y_shift);
                cv::imshow(WINNAME, img_window);
                break;

            case 'a': // 左へ移動
            case 'A':
                x_shift -= shift_amount;
                std::cout << "左に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
                img_window = original_img_window.clone();
                shiftImage(img_window, x_shift, y_shift);
                cv::imshow(WINNAME, img_window);
                break;

            case 'd': // 右へ移動
            case 'D':
                x_shift += shift_amount;
                std::cout << "右に " << shift_amount << " px 移動しました。現在の位置: x=" << x_shift << ", y=" << y_shift << std::endl;
                img_window = original_img_window.clone();
                shiftImage(img_window, x_shift, y_shift);
                cv::imshow(WINNAME, img_window);
                break;

            case 'r': // リセット
            case 'R':
                x_shift = 0;
                y_shift = 0;
                std::cout << "位置をリセットしました。" << std::endl;
                img_window = original_img_window.clone();
                cv::imshow(WINNAME, img_window);
                break;

            case 'i': // シフト量設定モード
            case 'I':
                input_mode = true;
                input_buffer = "";
                std::cout << "シフト量入力モードに切り替えました。数字キーで入力し、Enterで確定します。" << std::endl;
                break;

            case 'f': // ギャップ調整モード
            case 'F':
                focal_length_mode = true;
                tile_size_mode = false; // タイルサイズモードを終了
                tile_z_mode = false;    // タイル位置モードを終了
                std::cout << "ギャップ調整モードに切り替えました。W/Sキーでギャップを調整します。現在値: " << focalLength << " mm (ステップ: " << focal_length_step << " mm)" << std::endl;
                // モード表示の更新
                display_img = img_window.clone();
                cv::putText(display_img, "ギャップ調整モード: " + std::to_string(focalLength) + " mm (ステップ: " + std::to_string(focal_length_step) + " mm)",
                    cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
                cv::imshow(WINNAME, display_img);
                break;

            case 't': // タイルサイズ調整モード
            case 'T':
                tile_size_mode = true;
                focal_length_mode = false; // ギャップ調整モードを終了
                tile_z_mode = false;       // タイル位置モードを終了
                std::cout << "タイルサイズ調整モードに切り替えました。W/Sキーでタイルサイズを調整します。現在値: " << tileSize << " (ステップ: " << tile_size_step << ")" << std::endl;
                // モード表示の更新
                display_img = img_window.clone();
                cv::putText(display_img, "タイルサイズ調整モード: " + std::to_string(tileSize) + " (ステップ: " + std::to_string(tile_size_step) + ")",
                    cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0), 2);
                cv::imshow(WINNAME, display_img);
                break;

            case 'z': // タイル位置調整モード
            case 'Z':
                tile_z_mode = true;
                focal_length_mode = false; // ギャップ調整モードを終了
                tile_size_mode = false;    // タイルサイズモードを終了
                std::cout << "タイル位置調整モードに切り替えました。W/Sキーでタイル位置を調整します。現在値: " << tileZ << " mm (ステップ: " << tile_z_step << " mm)" << std::endl;
                // モード表示の更新
                display_img = img_window.clone();
                cv::putText(display_img, "タイル位置調整モード: " + std::to_string(tileZ) + " mm (ステップ: " + std::to_string(tile_z_step) + " mm)",
                    cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);
                cv::imshow(WINNAME, display_img);
                break;

            case 27: // ESC: 終了
                continue_key_loop = false;
                running = false;
                break;

            default:
                break;
            }
        }

        // 値が変更された場合のみループを継続（ESCキーのときは除く）
        if (!running && (prev_focal_length != focalLength || prev_tile_size != tileSize || prev_tile_z != tileZ)) {
            running = true;
        }

        // モード変更時にフラグをリセット
        if (key == 'i' || key == 'I' || key == 'f' || key == 'F' || key == 't' || key == 'T' || key == 'z' || key == 'Z') {
            initial_mode_display = true;
        }
    }

    return 0;
}

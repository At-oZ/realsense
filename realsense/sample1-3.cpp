//#include <iostream>
//#include <vector>
//#include <chrono>
//
//int main() {
//    const int width = 2261, height = 2261, channels = 3;
//
//    // 3次元ベクトルで画像データを表現
//    std::vector<std::vector<std::vector<int>>> A(height, std::vector<std::vector<int>>(width, std::vector<int>(channels)));
//
//    // 計測開始時間
//    auto start = std::chrono::high_resolution_clock::now();
//
//    // 指定された計算で各ピクセルの値を設定
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            A[i][j][0] = (i * width + j) % 256; // 赤
//            A[i][j][1] = ((i * width + j) * 100) % 256; // 緑
//            A[i][j][2] = ((i * width + j) * 200) % 256; // 青
//        }
//    }
//
//    // 計測終了時間
//    auto finish = std::chrono::high_resolution_clock::now();
//
//    // 経過時間を出力
//    std::chrono::duration<double> elapsed = finish - start;
//    std::cout << "Time to fill the array: " << elapsed.count() << " seconds" << std::endl;
//
//    // 必要に応じて画像データを操作または出力
//    // 例: 画像の一部のピクセル値を表示
//    std::cout << "Pixel at (0,0): (" << A[0][0][0] << ", " << A[0][0][1] << ", " << A[0][0][2] << ")" << std::endl;
//
//    return 0;
//}

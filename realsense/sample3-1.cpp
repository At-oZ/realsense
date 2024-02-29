//#include <algorithm> // std::sort
//#include <vector>
//#include <iostream>
//#include <chrono>
//#include <random>
//
//int main() {
//    //// 二次元配列の定義（例として4x3の配列）
//    //std::vector<std::vector<int>> arr = {
//    //    {3, 2, 5},
//    //    {1, 4, 4},
//    //    {2, 3, 3},
//    //    {4, 1, 2}
//    //};
//
//    const int rows = 307200; // 行数
//    const int cols = 6;      // 列数
//    std::vector<std::vector<double>> arr(rows, std::vector<double>(cols));
//
//    // 乱数生成器の設定
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0.0, 1.0);
//
//    // 配列に乱数を格納
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            arr[i][j] = dis(gen);
//        }
//    }
//
//    // 計測開始時間
//    auto start = std::chrono::high_resolution_clock::now();
//
//    // ソートする列のインデックス（0から始まる）
//    int columnIndex = 1; // 例：2番目の列に基づいて並べ替え
//
//    // カスタム比較関数
//    auto compare = [columnIndex](const std::vector<double>& a, const std::vector<double>& b) {
//        return a[columnIndex] < b[columnIndex];
//    };
//
//    // 特定の列に基づいて全体を並べ替え
//    std::sort(arr.begin(), arr.end(), compare);
//
//    //// 結果の表示
//    //for (const auto& row : arr) {
//    //    for (int val : row) {
//    //        std::cout << val << " ";
//    //    }
//    //    std::cout << std::endl;
//    //}
//
//    // 計測終了時間
//    auto finish = std::chrono::high_resolution_clock::now();
//
//    // 経過時間を出力
//    std::chrono::duration<double> elapsed = finish - start;
//    std::cout << "Time to fill the array: " << elapsed.count() << " seconds" << std::endl;
//
//    return 0;
//}

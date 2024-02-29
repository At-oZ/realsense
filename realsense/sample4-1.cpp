//#include <iostream>
//#include <vector>
//#include <algorithm>
//#include <iterator>
//
//int main() {
//    // 二次元配列をstd::vectorで定義
//    std::vector<std::vector<int>> A = {
//        {1, 0, 3},
//        {4, 5, 6},
//        {7, 0, 9},
//        {10, 11, 12}
//    };
//
//    //// 条件に一致する行を削除する新たな二次元配列Bを作成
//    //std::vector<std::vector<int>> B;
//
//    //// 2列目が0でない行だけをBにコピー
//    //std::copy_if(A.begin(), A.end(), std::back_inserter(B), [](const std::vector<int>& row) {
//    //    return row[1] != 0; // 2列目が0ではない行を選択
//    //    });
//
//    //// 結果を表示
//    //for (const auto& row : B) {
//    //    for (int elem : row) {
//    //        std::cout << elem << " ";
//    //    }
//    //    std::cout << std::endl;
//    //}
//
//    decltype(A)::iterator lastA = std::end(A);
//
//    std::cout << lastA << " " << std::endl;
//
//    return 0;
//}

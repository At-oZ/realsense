//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//// 二次元配列の一列目の値に基づいて行を比較する関数
//int compareRows(vector<int>& row1, vector<int>& row2) {
//    return row1[0] < row2[0];
//}
//
//// パーティション関数
//int partition(vector<vector<int>>& A, int low, int high) {
//    vector<int> pivot = A[high];    // 基準点を配列の最後に設定
//    int i = low - 1;
//
//    for (int j = low; j <= high - 1; j++) {
//        // もし現在の要素がピボットより小さい場合、iをインクリメントし、その要素をiの位置に交換
//        if (compareRows(A[j], pivot)) {
//            i++;
//            swap(A[i], A[j]);
//        }
//    }
//    swap(A[i + 1], A[high]);  // ピボットを正しい位置に置く
//    return (i + 1);
//}
//
//// クイックソート関数
//void quickSort(vector<vector<int>>& A, int low, int high) {
//    if (low < high) {
//        // パーティションインデックスを取得する。A[p] は現在正しい場所にあります
//        int pi = partition(A, low, high);
//
//        // 別々に前半部と後半部をソートする
//        quickSort(A, low, pi - 1);
//        quickSort(A, pi + 1, high);
//    }
//}
//
////// 二次元配列の一列目の値に基づいて行を比較する関数
////int compareRows(vector<int>& row1, vector<int>& row2) {
////    return row1[0] < row2[0];
////}
////
////// パーティション関数
////int partition(vector<int>& A, int low, int high) {
////    vector<int> pivot = A[high];    // 基準点を配列の最後に設定
////    int i = low - 1;
////
////    for (int j = low; j <= high - 1; j++) {
////        // もし現在の要素がピボットより小さい場合、iをインクリメントし、その要素をiの位置に交換
////        if (compareRows(A[j], pivot)) {
////            i++;
////            swap(A[i], A[j]);
////        }
////    }
////    swap(A[i + 1], A[high]);  // ピボットを正しい位置に置く
////    return (i + 1);
////}
////
////// クイックソート関数
////void quickSort(vector<int>& A, int low, int high) {
////    if (low < high) {
////        // パーティションインデックスを取得する。A[p] は現在正しい場所にあります
////        int pi = partition(A, low, high);
////
////        // 別々に前半部と後半部をソートする
////        quickSort(A, low, pi - 1);
////        quickSort(A, pi + 1, high);
////    }
////}
//
//int main() {
//    //vector<vector<int>> A = { {4, 2, 3}, {1, 5, 6}, {7, 8, 9}, {2, 5, 2}, {1, 7, 8}, {9, 6, 4}, {10, 20, 2}, {32, 52, 2}, {15, 2, 19} };
//    //vector<int> A = { 4, 2, 3, 1, 5, 6, 7, 8, 9, 2, 5, 2, 1, 7, 8, 9, 6, 4, 10, 20, 2, 32, 52, 2, 15, 2, 19 };
//    vector<vector<int>> A(20, vector<int>(5));
//    for (int i = 0; i < 20; i++) {
//        for (int j = 0; j < 5; j++) {
//            A[i][j] = rand() % 100;
//        }
//    }
//    int n = A.size();
//
//    cout << "ソート前の配列:" << endl;
//    // ソート後の配列を出力
//    for (int i = 0; i < n; i++) {
//        for (int j = 0; j < A[i].size(); j++) {
//            cout << A[i][j] << " ";
//        }
//        cout << "\n";
//    }
//
//    quickSort(A, 0, n - 1);
//
//    cout << "ソート後の配列:" << endl;
//    // ソート後の配列を出力
//    for (int i = 0; i < n; i++) {
//        for (int j = 0; j < A[i].size(); j++) {
//            cout << A[i][j] << " ";
//        }
//        cout << "\n";
//    }
//
//    return 0;
//}

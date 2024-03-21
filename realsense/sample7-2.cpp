//#include <iostream>
//#include <vector>
//using namespace std;
//
//// パーティション関数: ブロックの最後の要素を基準にして、ブロックをソート
//int partition(vector<int>& A, int low, int high) {
//    int pivot = A[high * 5 - 1];  // ブロックの最後の要素
//    int i = low - 1;
//
//    for (int j = low + 1; j < high - 1; j++) {
//        //std::cout << "j:" << j << std::endl;
//        if (A[j * 5 - 1] < pivot) {
//            i++;
//            // ブロック全体を交換
//            for (int k = 0; k < 5; k++) {
//                swap(A[i * 5 + k], A[j * 5 + k]);
//            }
//        }
//    }
//    //// ブロック全体を交換
//    //for (int k = 0; k < 5; k++) {
//    //    swap(A[(i + 1) * 5 + k], A[high * 5 + k]);
//    //}
//    return i + 1;
//}
//
//// クイックソート関数
//void quickSort(vector<int>& A, int low, int high) {
//    if (low < high) {
//        int pi = partition(A, low, high);
//        quickSort(A, low, pi - 1);
//        quickSort(A, pi + 1, high);
//    }
//}
//
//int main() {
//    // 要素数100の配列を用意
//    vector<int> A(100);
//    // 配列を初期化
//    for (int i = 0; i < 100; i++) {
//        A[i] = rand() % 100;  // 乱数で配列を埋める
//    }
//
//    cout << "ソート前の配列:" << endl;
//    for (int i = 0; i < 100; i++) {
//        cout << A[i] << " ";
//        if ((i + 1) % 5 == 0) cout << endl;  // ブロックごとに改行
//    }
//
//    // 配列をブロック単位でクイックソート
//    quickSort(A, 0, 20);  // 20ブロックがあるため、highは19となる
//
//    cout << "\nソート後の配列:" << endl;
//    for (int i = 0; i < 100; i++) {
//        cout << A[i] << " ";
//        if ((i + 1) % 5 == 0) cout << endl;  // ブロックごとに改行
//    }
//
//    return 0;
//}

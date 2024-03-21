//#include <iostream>
//#include <vector>
//
//using namespace std;
//
//int medianOfThree(int* A, int low, int high, int m, int l) {
//    int mid = low + (high - low) / 2;
//    if (A[mid * m + l] < A[low * m + l]) swap(A[mid * m + l], A[low * m + l]);
//    if (A[high * m + l] < A[low * m + l]) swap(A[high * m + l], A[low * m + l]);
//    if (A[mid * m + l] < A[high * m + l]) swap(A[mid * m + l], A[high * m + l]);
//    // ここで、highの位置の要素を中央値として選択します
//    return high;
//}
//
//// パーティション関数
//int partition(int* A, int low, int high, int m, int l) {
//    int pivotIndex = medianOfThree(A, low, high, m, l);
//    int pivot = A[pivotIndex * m + l];
//    int i = low - 1;
//    int tmp;
//    for (int j = low; j <= high - 1; j++) {
//        // もし現在の要素がピボットより小さい場合、iをインクリメントし、その要素をiの位置に交換
//        if (A[j * m + l] < pivot) {
//            i++;
//            for (int k = 0; k < m; k++) {
//                tmp = A[i * m + k];
//                A[i * m + k] = A[j * m + k];
//                A[j * m + k] = tmp;
//            }
//        }
//    }
//    for (int k = 0; k < m; k++) {
//        tmp = A[(i + 1) * m + k];
//        A[int(high / 1000) * m + k] = A[(i + 1) * m + k];
//        A[(i + 1) * m + k] = tmp;
//    }
//    return (i + 1);
//}
//
//// クイックソート関数
//void quickSort(int* A, int low, int high, int m, int l) {
//    if (low < high) {
//        // パーティションインデックスを取得する。A[p] は現在正しい場所にあります
//        int pi = partition(A, low, high, m, l);
//
//        // 別々に前半部と後半部をソートする
//        quickSort(A, low, pi - 1, m, l);
//        quickSort(A, pi + 1, high, m, l);
//    }
//}
//
//int main() {
//
//    int n = 1000000, m = 5, l = 0;
//
//    int* A;
//    A = (int*)malloc(sizeof(int) * n * m);
//
//    for (int i = 0; i < n * m; i++) {
//        A[i] = rand() % 100;
//    }
//
//    //cout << "ソート前の配列:" << endl;
//    //for (int i = 0; i < n * m; i++) {
//    //    cout << A[i] << " ";
//    //    if (i % m == 4) {
//    //        cout << endl;
//    //    }
//    //}
//
//    quickSort(A, 0, n - 1, m, l);
//
//    //cout << "ソート後の配列:" << endl;
//    //for (int i = 0; i < n * m; i++) {
//    //    cout << A[i] << " ";
//    //    if (i % m == 4) {
//    //        cout << endl;
//    //    }
//    //}
//
//    free(A);
//    return 0;
//}

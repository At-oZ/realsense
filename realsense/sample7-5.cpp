//#include <iostream>
//#include <vector>
//#include <stack>
//using namespace std;
//
//// パーティション関数は変わりません
//int partition(int* arr, int low, int high, int m) {
//    int pivot = arr[high * m];
//    int i = low - 1;
//    for (int j = low; j <= high - 1; j++) {
//        if (arr[j * m] < pivot) {
//            i++;
//            for (int k = 0; k < m; k++) {
//                swap(arr[i * m + k], arr[j * m + k]);
//            }
//        }
//    }
//    for (int k = 0; k < m; k++) {
//        swap(arr[(i + 1) * m + k], arr[high * m + k]);
//    }
//    return i + 1;
//}
//
//// 反復的なクイックソート関数
//void iterativeQuickSort(int* arr, int l, int h, int m) {
//    // 分割すべき範囲を保持するためのスタック
//    stack<int> stack;
//
//    // 初期範囲をプッシュ
//    stack.push(l);
//    stack.push(h);
//
//    // スタックが空になるまでループ
//    while (!stack.empty()) {
//        // 範囲をポップ
//        h = stack.top();
//        stack.pop();
//        l = stack.top();
//        stack.pop();
//
//        // パーティションを実行
//        int p = partition(arr, l, h, m);
//
//        // 左側の要素があればスタックにプッシュ
//        if (p - 1 > l) {
//            stack.push(l);
//            stack.push(p - 1);
//        }
//
//        // 右側の要素があればスタックにプッシュ
//        if (p + 1 < h) {
//            stack.push(p + 1);
//            stack.push(h);
//        }
//    }
//}
//
//// メイン関数
//int main() {
//
//    int n = 300000, m = 6;
//    int* arr;
//    arr = (int*)malloc(sizeof(int) * n * m);
//
//    clock_t cpu_time;
//    double start, end;
//
//    for (int i = 0; i < n * m; i++) {
//        arr[i] = rand() % 100;
//    }
//
//    cpu_time = clock();
//    start = double(cpu_time) / double(CLOCKS_PER_SEC);
//    iterativeQuickSort(arr, 0, n - 1, m);
//    cpu_time = clock();
//    end = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//    cout << "time:" << end - start << endl;
//    //cout << "Sorted array: \n";
//    //for (int i = 0; i < n; i++) {
//    //    for (int j = 0; j < m; j++) {
//    //        cout << arr[i * m + j] << " ";
//    //    }
//    //    cout << endl;
//    //}
//    free(arr);
//    return 0;
//}

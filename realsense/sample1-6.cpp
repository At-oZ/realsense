//#include <iostream>
//#include <vector>
//#include <chrono>
//
//// malloc < new << vector の順で速い
//int main(int argc, char* argv[]) {
//
//    const int x = 2261, y = 2261, z = 3;
//    int*** A;
//
//    //A = new int[x * y * z];
//    //A = new int**[x];
//    //for (int i = 0; i < x; i++) {
//    //    A[i] = new int* [y];
//    //    for (int j = 0; j < y; j++) {
//    //        A[i][j] = new int[z];
//    //    }
//    //}
//
//    //A = (int*)malloc(sizeof(int) * x * y * z);
//    A = (int***)malloc(sizeof(int**) * x);
//    for (int i = 0; i < x; i++) {
//        A[i] = (int**)malloc(sizeof(int*) * y);
//        for (int j = 0; j < y; j++) {
//            A[i][j] = (int*)malloc(sizeof(int) * z);
//        }
//    }
//
//    //int A[x * y * z];
//
//    //std::vector<int> A(x * y * z);
//
//    std::chrono::duration<double> elapsed;
//
//    double sum = 0;
//    for (int k = 0; k < 1000; k++) {
//
//        // 計測開始時間
//        auto start = std::chrono::high_resolution_clock::now();
//
//        // 3重のforループを使用して配列の各要素に100を代入
//        for (int i = 0; i < x; i++) {
//            for (int j = 0; j < y; j++) {
//                for (int k = 0; k < z; k++) {
//                    A[i][j][z] = 100;
//                }
//            }
//        }
//        // 計測終了時間
//        auto finish = std::chrono::high_resolution_clock::now();
//
//        elapsed = finish - start;
//
//        sum += elapsed.count();
//
//    }
//
//    //double sum = 0;
//    //for (int k = 0; k < 1000; k++) {
//
//    //    // 計測開始時間
//    //    auto start = std::chrono::high_resolution_clock::now();
//
//    //    // 2重のforループを使用して配列の各要素に100を代入
//    //    for (int i = 0; i < x; i++) {
//    //        for (int j = 0; j < y; j++) {
//    //            A[i * 2261 * 3 + j * 3 + 0] = 100;
//    //            A[i * 2261 * 3 + j * 3 + 1] = 100;
//    //            A[i * 2261 * 3 + j * 3 + 2] = 100;
//    //        }
//    //    }
//    //    // 計測終了時間
//    //    auto finish = std::chrono::high_resolution_clock::now();
//
//    //    elapsed = finish - start;
//
//    //    sum += elapsed.count();
//
//    //}
//
//    // 経過時間を出力
//    std::cout << "Average Time to fill the array: " << sum / 1000 << " seconds" << std::endl;
//
//    // 必要に応じて画像データを操作または出力
//    // 例: 画像の一部のピクセル値を表示
//    //std::cout << "Pixel at (0,0): (" << A[0] << ", " << A[1] << ", " << A[2] << ")" << std::endl;
//    std::cout << "Pixel at (0,0): (" << A[0][0][0] << ", " << A[0][0][1] << ", " << A[0][0][2] << ")" << std::endl;
//
//    //for (int i = 0; i < x; i++) {
//    //    for (int j = 0; j < y; j++) {
//    //        delete[] A[i][j];
//    //    }
//    //    delete[] A[i];
//    //}
//    //delete[] A;
//
//    for (int i = 0; i < x; i++) {
//        for (int j = 0; j < y; j++) {
//            free(A[i][j]);
//        }
//        free(A[i]);
//    }
//    free(A);
//
//    return 0;
//}

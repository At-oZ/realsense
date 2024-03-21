//#include <iostream>
//#include <vector>
//#include <chrono>
//#include <time.h>
//
//// malloc < new << vector の順で速い
//int main(int argc, char* argv[]) {
//
//    double start, end;
//    clock_t cpu_time;
//
//    const int x = 2261, y = 2261, z = 3;
//
//    int*** A;
//    //A = (int***)malloc(sizeof(int**) * x);
//    //for (int i = 0; i < x; i++) {
//    //    A[i] = (int**)malloc(sizeof(int*) * y);
//    //    for (int j = 0; j < y; j++) {
//    //        A[i][j] = (int*)malloc(sizeof(int) * z);
//    //    }
//    //}
//    A = new int**[x];
//    for (int i = 0; i < x; i++) {
//        A[i] = new int* [y];
//        for (int j = 0; j < y; j++) {
//            A[i][j] = new int[z];
//        }
//    }
//    //std::vector<std::vector<std::vector<int>>> A(x, std::vector<std::vector<int>>(y, std::vector<int>(z)));
//
//    //std::chrono::duration<double> elapsed;
//
//    //double sum = 0;
//    //for (int l = 0; l < 100; l++) {
//
//    //    // 計測開始時間
//    //    //auto start = std::chrono::high_resolution_clock::now();
//    //    cpu_time = clock();
//    //    start = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//    //    //// 3重のforループを使用して配列の各要素に100を代入
//    //    //for (int i = 0; i < x; i++) {
//    //    //    for (int j = 0; j < y; j++) {
//    //    //        for (int k = 0; k < z; k++) {
//    //    //            A[i][j][k] = 100;
//    //    //        }
//    //    //    }
//    //    //}
//
//    //    // 2重のforループを使用して配列の各要素に100を代入
//    //    for (int i = 0; i < x; i++) {
//    //        for (int j = 0; j < y; j += 7) {
//    //            A[i][j][0] = 100;
//    //            A[i][j][1] = 100;
//    //            A[i][j][2] = 100;
//    //            A[i][j + 1][0] = 100;
//    //            A[i][j + 1][1] = 100;
//    //            A[i][j + 1][2] = 100;
//    //            A[i][j + 2][0] = 100;
//    //            A[i][j + 2][1] = 100;
//    //            A[i][j + 2][2] = 100;
//    //            A[i][j + 3][0] = 100;
//    //            A[i][j + 3][1] = 100;
//    //            A[i][j + 3][2] = 100;
//    //            A[i][j + 4][0] = 100;
//    //            A[i][j + 4][1] = 100;
//    //            A[i][j + 4][2] = 100;
//    //            A[i][j + 5][0] = 100;
//    //            A[i][j + 5][1] = 100;
//    //            A[i][j + 5][2] = 100;
//    //            A[i][j + 6][0] = 100;
//    //            A[i][j + 6][1] = 100;
//    //            A[i][j + 6][2] = 100;
//
//    //        }
//    //    }
//
//    //    //// 1重のforループを使用して配列の各要素に100を代入
//    //    //int i, j;
//    //    //for (int k = 0; k < x * y; k++) {
//
//    //    //    i = int(k / y);
//    //    //    j = k % y;
//
//    //    //    A[i][j][0] = 100;
//    //    //    A[i][j][1] = 100;
//    //    //    A[i][j][2] = 100;
//    //    //}
//
//    //    // 計測終了時間
//    //    //auto finish = std::chrono::high_resolution_clock::now();
//    //    cpu_time = clock();
//    //    end = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//    //    //elapsed = finish - start;
//
//    //    //sum += elapsed.count();
//    //    sum += end - start;
//
//    //}
//
//    //// 経過時間を出力
//    //std::cout << "Average Time to fill the array: " << sum / 100 << " seconds" << std::endl;
//
//    //// 必要に応じて画像データを操作または出力
//    //// 例: 画像の一部のピクセル値を表示
//    //std::cout << "Pixel at (0,0): (" << A[0][0][0] << ", " << A[0][0][1] << ", " << A[0][0][2] << ")" << std::endl;
//
//
//
//    //int* A;
//    //A = (int*)malloc(sizeof(int) * x * y * z);
//    //A = new int[x * y * z];
//    //std::vector<int> A(x * y * z);
//
//
//    //double sum = 0;
//    //for (int k = 0; k < 100; k++) {
//
//    //    // 計測開始時間
//    //    //auto start = std::chrono::high_resolution_clock::now();
//    //    cpu_time = clock();
//    //    start = double(cpu_time) / double(CLOCKS_PER_SEC);
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
//    //    //auto finish = std::chrono::high_resolution_clock::now();
//    //    cpu_time = clock();
//    //    end = double(cpu_time) / double(CLOCKS_PER_SEC);
//
//    //    //elapsed = finish - start;
//
//    //    //sum += elapsed.count();
//    //    sum += end - start;
//
//    //}
//
//    ////経過時間を出力
//    //std::cout << "Average Time to fill the array: " << sum / 100 << " seconds" << std::endl;
//
//    ////必要に応じて画像データを操作または出力
//    ////例: 画像の一部のピクセル値を表示
//    //std::cout << "Pixel at (0,0): (" << A[0] << ", " << A[1] << ", " << A[2] << ")" << std::endl;
//
//
//    //for (int i = 0; i < x; i++) {
//    //    for (int j = 0; j < y; j++) {
//    //        free(A[i][j]);
//    //    }
//    //    free(A[i]);
//    //}
//    //free(A);
//    for (int i = 0; i < x; i++) {
//        for (int j = 0; j < y; j++) {
//            delete[] A[i][j];
//        }
//        delete[] A[i];
//    }
//    delete[] A;
//
//    return 0;
//}

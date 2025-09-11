//#include <iostream>
//#include <vector>
//#include <thread>
//#include <numeric> // std::iota
//#include <chrono>
//
//// 各スレッドが実行する関数
//// 特定の範囲 [start, end) の配列要素を計算する
//void worker_task(std::vector<float>& result, const std::vector<float>& a, const std::vector<float>& b, size_t start, size_t end) {
//    // このループがSIMD化の対象となる
//    // ループ内の処理は独立しており、単純な演算であるため、
//    // コンパイラがSIMD命令を生成する可能性が非常に高い。
//    for (size_t i = start; i < end; ++i) {
//        result[i] = a[i] * b[i] + 5.0f;
//    }
//}
//
//int main() {
//    const size_t data_size = 1000 * 1000 * 10; // 1000万要素
//    std::vector<float> result(data_size);
//    std::vector<float> a(data_size);
//    std::vector<float> b(data_size);
//
//    // テストデータの準備
//    std::iota(a.begin(), a.end(), 1.0f);
//    std::iota(b.begin(), b.end(), 0.5f);
//
//    // 利用可能なハードウェアスレッド数を取得
//    const unsigned int num_threads = std::thread::hardware_concurrency();
//    std::cout << "Using " << num_threads << " threads." << std::endl;
//
//    // スレッドを管理するためのvector
//    std::vector<std::thread> threads;
//
//    // 各スレッドに割り当てるデータ量を計算
//    const size_t chunk_size = data_size / num_threads;
//
//	auto start_time = std::chrono::high_resolution_clock::now();
//
//    // タスクを分割してスレッドを起動
//    for (unsigned int i = 0; i < num_threads; ++i) {
//        const size_t start = i * chunk_size;
//        // 最後のスレッドは残りの要素をすべて担当
//        const size_t end = (i == num_threads - 1) ? data_size : start + chunk_size;
//
//        threads.emplace_back(worker_task, std::ref(result), std::ref(a), std::ref(b), start, end);
//    }
//
//    // すべてのスレッドが終了するのを待つ
//    for (auto& t : threads) {
//        t.join();
//    }
//
//	auto end_time = std::chrono::high_resolution_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
//	std::cout << "Processing time: " << duration << " ms" << std::endl;
//    std::cout << "Processing finished." << std::endl;
//
//    return 0;
//}
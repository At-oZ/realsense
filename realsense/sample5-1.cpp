//#include <iostream>
//#include <chrono>
//
//using namespace std;
//using namespace std::chrono;
//
//int main() {
//    const long long num_iterations = 1000000000; // 1 billion iterations for precision
//
//    // Benchmark for int
//    auto start_int = high_resolution_clock::now();
//    for (int i = 0; i < num_iterations; i++) {}
//    auto stop_int = high_resolution_clock::now();
//    auto duration_int = duration_cast<microseconds>(stop_int - start_int);
//
//    // Benchmark for double
//    auto start_double = high_resolution_clock::now();
//    for (double j = 0; j < num_iterations; j++) {}
//    auto stop_double = high_resolution_clock::now();
//    auto duration_double = duration_cast<microseconds>(stop_double - start_double);
//
//    cout << "Time taken by int loop: "
//        << duration_int.count() << " microseconds" << endl;
//    cout << "Time taken by double loop: "
//        << duration_double.count() << " microseconds" << endl;
//
//    return 0;
//}
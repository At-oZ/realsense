//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <string>
//#include <algorithm>
//
//// CSVファイルを読み込み、各行をベクターとして返す関数
//std::vector<std::vector<double>> readCSV(const std::string& filename) {
//    std::vector<std::vector<double>> data;
//    std::ifstream file(filename);
//    std::string line;
//
//    while (std::getline(file, line)) {
//        std::vector<double> row;
//        std::stringstream ss(line);
//        std::string cell;
//
//        while (std::getline(ss, cell, ',')) {
//            row.push_back(std::stod(cell));
//        }
//        data.push_back(row);
//    }
//
//    file.close();
//    return data;
//}
//
//// CSVデータをファイルに書き込む関数
//void writeCSV(const std::string& filename, const std::vector<std::vector<double>>& data) {
//    std::ofstream file(filename);
//
//    for (const auto& row : data) {
//        for (size_t i = 0; i < row.size(); ++i) {
//            file << row[i];
//            if (i < row.size() - 1) {
//                file << ",";
//            }
//        }
//        file << "\n";
//    }
//
//    file.close();
//}
//
//// 特定の列の値を書き換える関数
//void modifyColumn(std::vector<std::vector<double>>& data) {
//    for (size_t i = 0; i < data.size(); ++i) {
//        if (i % 960 == 0) {
//            data[i][5] = 255;
//            data[i][4] = 0;
//            data[i][3] = 0;
//        }
//        else {
//            data[i][5] = static_cast<int>(floor(i / (256 * 256)));
//            data[i][4] = static_cast<int>(floor((i - data[i][5] * 256 * 256) / 256));
//            data[i][3] = i - data[i][4] * 256 - data[i][5] * 256 * 256;
//        }
//
//    }
//}
//
//// 配列をソートする関数
//void sortByColumn(std::vector<std::vector<double>>& array, int rows, int cols, int column) {
//
//    std::sort(array.begin(), array.end(), [column](const std::vector<double>& a, const std::vector<double>& b) {
//        return a[column] < b[column];
//        });
//}
//
//int main() {
//    // 入力ファイル名と出力ファイル名
//    std::string inputFilename = "output_v3.csv";
//    std::string outputFilename = "output_v3a.csv";
//
//    // CSVファイルを読み込む
//    auto data = readCSV(inputFilename);
//
//    // 特定の列（例: 1列目）でソート
//    int columnToSort = 2;
//    sortByColumn(data, 480*640, 6, columnToSort);
//
//    //// 特定の列の値を書き換える
//    //modifyColumn(data);
//
//    // 結果をCSVファイルに書き込む
//    writeCSV(outputFilename, data);
//
//    std::cout << "CSVファイルの更新が完了しました。" << std::endl;
//    return 0;
//}

//// ピンホールを通して画像を観察したときの観察画像の作成
///*
//    一つのピンホールに対してその中の色を計算
//*/
// 
//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//int main()
//{
//    // 画像を読み込む
//    cv::Mat image = cv::imread("./images/lenna.bmp");
//
//    if (image.empty())
//    {
//        std::cout << "画像を開くことができませんでした。\n";
//        return -1;
//    }
//
//    // 画像のサイズとピクセルサイズを計算
//    const double tile_size_mm = 100.0;  // タイルのサイズ（mm）
//    const int image_size_px = 256;      // 画像の解像度（ピクセル）
//    const double pixel_size_mm = tile_size_mm / image_size_px; // ピクセルサイズ（mm）
//
//    // 可視領域の範囲を計算
//    const double visible_area_min = -0.22; // mm
//    const double visible_area_max = 0.22;  // mm
//
//    // ピクセルインデックスの範囲を計算
//    int i_min = std::max(0, static_cast<int>(std::floor((visible_area_min + tile_size_mm / 2) / pixel_size_mm)));
//    int i_max = std::min(image_size_px - 1, static_cast<int>(std::floor((visible_area_max + tile_size_mm / 2) / pixel_size_mm)));
//    int j_min = std::max(0, static_cast<int>(std::floor((tile_size_mm / 2 - visible_area_max) / pixel_size_mm)));
//    int j_max = std::min(image_size_px - 1, static_cast<int>(std::floor((tile_size_mm / 2 - visible_area_min) / pixel_size_mm)));
//
//    // 観察される色の加重平均を計算
//    cv::Vec3d weighted_color(0, 0, 0);
//    double total_area = 0.0;
//
//    for (int i = i_min; i <= i_max; ++i)
//    {
//        // ピクセルのx座標範囲を計算
//        double x_pixel_min = i * pixel_size_mm - tile_size_mm / 2;
//        double x_pixel_max = (i + 1) * pixel_size_mm - tile_size_mm / 2;
//
//        double x_overlap = std::max(0.0, std::min(visible_area_max, x_pixel_max) - std::max(visible_area_min, x_pixel_min));
//
//        if (x_overlap <= 0) continue;
//
//        for (int j = j_min; j <= j_max; ++j)
//        {
//            // ピクセルのy座標範囲を計算
//            double y_pixel_min = tile_size_mm / 2 - (j + 1) * pixel_size_mm;
//            double y_pixel_max = tile_size_mm / 2 - j * pixel_size_mm;
//
//            double y_overlap = std::max(0.0, std::min(visible_area_max, y_pixel_max) - std::max(visible_area_min, y_pixel_min));
//
//            if (y_overlap <= 0) continue;
//
//            double overlap_area = x_overlap * y_overlap;
//            total_area += overlap_area;
//
//            // ピクセル値を取得し、加重平均に加算
//            cv::Vec3b pixel = image.at<cv::Vec3b>(j, i);
//            weighted_color += cv::Vec3d(pixel) * overlap_area;
//        }
//    }
//
//    if (total_area > 0)
//    {
//        weighted_color /= total_area;
//        std::cout << "観察される色 (B, G, R): (" << weighted_color[0] << ", " << weighted_color[1] << ", " << weighted_color[2] << ")\n";
//    }
//    else
//    {
//        std::cout << "観察可能な領域がありません。\n";
//    }
//
//    return 0;
//}

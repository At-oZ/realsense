#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
	// 画像の読み込み
	cv::Mat img1 = cv::imread("C:/ForStudy/reconstruction/OpenGL-normal-v1/OpenGL-normal-v1-grid_f8.4034_subsize360.78_zi1000_2.png");
	cv::Mat img2 = cv::imread("C:/ForStudy/reconstruction/IE-prop-normal-v1/prop-normal-v1-grid_f8.4034_subsize360.78_zi1000.png");
	if (img1.empty() || img2.empty()) {
		std::cerr << "画像の読み込みに失敗しました。" << std::endl;
		return -1;
	}
	// 画像のサイズが異なる場合、同じサイズにリサイズ
	if (img1.size() != img2.size()) {
		cv::resize(img2, img2, img1.size());
	}
	// 画像の差分を計算
	cv::Mat diff;
	cv::absdiff(img1, img2, diff);
	// 差分画像を強調表示するためにスケーリング
	cv::Mat diff_scaled;
	cv::normalize(diff, diff_scaled, 0, 255, cv::NORM_MINMAX);
	// 差分画像を表示
	cv::imshow("Difference Image", diff_scaled);
	cv::waitKey(0);
	// 差分画像を保存
	cv::imwrite("C:/ForStudy/reconstruction/diff/diff_prop-OpenGL_grid_f8.4034_subsize360.78_zi1000.png", diff_scaled);
	return 0;
}
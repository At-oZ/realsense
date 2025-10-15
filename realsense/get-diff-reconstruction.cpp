//#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <limits>
//#include <cmath>
//
//int main() {
//	// 画像の読み込み
//	cv::Mat img1 = cv::imread("D:/ForStudy/reconstruction/OpenGL-scratch-normal-v4-2-2/OpenGL-scratch-normal-v4-2-2-grid_f8.4034_subsize234.51_zi300.png");
//	//cv::Mat img2 = cv::imread("D:/ForStudy/reconstruction/OpenGL-normal-v1/OpenGL-normal-v1-grid_f8.4034_subsize252.55_zi400.png");
//	//cv::Mat img2 = cv::imread("D:/ForStudy/reconstruction/IE-prop-normal-v1/prop-normal-v1-grid_f8.4034_subsize252.55_zi400.png");
//	cv::Mat img2 = cv::imread("D:/ForStudy/reconstruction/PCSJ2025-prop-normal-v1/prop-normal-v1-grid_f8.4034_subsize234.51_zi300.png");
//	if (img1.empty() || img2.empty()) {
//		std::cerr << "画像の読み込みに失敗しました。" << std::endl;
//		return -1;
//	}
//
//	// 画像のサイズが異なる場合、同じサイズにリサイズ
//	if (img1.size() != img2.size()) {
//		cv::resize(img2, img2, img1.size());
//	}
//
//	// 画像の差分を計算
//	cv::Mat diff;
//	cv::absdiff(img1, img2, diff);
//
//	// PSNRを計算
//	double maxI = 255.0;
//	switch (img1.depth()) {
//	case CV_8U:  maxI = 255.0;   break;
//	case CV_16U: maxI = 65535.0; break;
//	case CV_32F: maxI = 1.0;     break;
//	default:     maxI = 255.0;   break;
//	}
//	cv::Mat diff32f;
//	diff.convertTo(diff32f, CV_32F);
//	cv::Scalar ssePerCh = cv::sum(diff32f.mul(diff32f));
//
//	const int ch = img1.channels();
//	double sse = 0.0;
//	for (int c = 0; c < ch; ++c) sse += ssePerCh[c];
//
//	const double denom = static_cast<double>(ch) * static_cast<double>(img1.total());
//	const double mse = (denom > 0.0) ? (sse / denom) : 0.0;
//	const double psnr = (mse <= 1e-15) ? std::numeric_limits<double>::infinity()
//		: 10.0 * std::log10((maxI * maxI) / mse);
//
//	std::cout << "MSE: " << mse << std::endl;
//	if (std::isinf(psnr)) {
//		std::cout << "PSNR: INF dB" << std::endl;
//	} else {
//		std::cout << "PSNR: " << psnr << " dB" << std::endl;
//	}
//
//	// 参考: チャネル別PSNR（B,G,R[,A]）
//	static const char* chName[4] = { "B", "G", "R", "A" };
//	for (int c = 0; c < ch && c < 4; ++c) {
//		const double mseC = (img1.total() > 0) ? (ssePerCh[c] / static_cast<double>(img1.total())) : 0.0;
//		const double psnrC = (mseC <= 1e-15) ? std::numeric_limits<double>::infinity()
//			: 10.0 * std::log10((maxI * maxI) / mseC);
//		if (std::isinf(psnrC)) {
//			std::cout << "PSNR(" << chName[c] << "): INF dB" << std::endl;
//		} else {
//			std::cout << "PSNR(" << chName[c] << "): " << psnrC << " dB" << std::endl;
//		}
//	}
//
//	// 差分画像を強調表示するためにスケーリング
//	cv::Mat diff_scaled;
//	cv::normalize(diff, diff_scaled, 0, 255, cv::NORM_MINMAX);
//
//	// 差分画像を表示
//	cv::imshow("Difference Image", diff_scaled);
//	cv::waitKey(0);
//
//	// 差分画像を保存
//	//cv::imwrite("D:/ForStudy/reconstruction/diff/diff_OpenGL-GPT_scratch_grid-normal-v1-grid_f8.4034_subsize252.55_zi400.png", diff_scaled);
//	//cv::imwrite("D:/ForStudy/reconstruction/diff/diff_prop-OpenGL_grid-normal-v4-2-2-grid_f8.4034_subsize252.55_zi400.png", diff_scaled);
//	return 0;
//}
//#include <opencv2/opencv.hpp>
//#include <cmath>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//
//using namespace std;
//
//int load(int start, int end) {
//
//	double wp = 1.25;
//	double ld = 13.4 * 25.4;
//	double pw = 3840.0;
//	double ph = 2400.0;
//	double pd = ld / sqrt(pw * pw + ph * ph);
//	double intv = wp / pd;
//
//	cv::Mat img_display = cv::Mat::zeros(cv::Size(pw, ph), CV_8UC3);
//	int startu, startv;
//	int NUMZEROS = 3;
//	string f, paddedf;
//	ostringstream stream;
//
//	for (int frame = start; frame < end; frame++) {
//
//		cout << "frame:" << frame << endl;
//
//		f = to_string(frame);
//		paddedf = string(NUMZEROS - f.length(), '0') + f;
//
//		for (int i = 0; i < 136; i++) {
//			for (int j = 0; j < 136; j++) {
//
//				startv = static_cast<int>(floor(i * intv));
//				startu = static_cast<int>(floor(j * intv));
//				//cout << startu << "=" << startv << endl;
//
//				stream.str("");
//				stream.clear();
//				stream << "D:/Users/taw11/projects/code/images/movie/simulation/E/g4_wp1.25_zi800_pd0.075_Wi200/frame" << paddedf << "/g4_wp1.25_zi800_pd0.075_Wi200_" << i << "=" << j << "_frame" << paddedf << ".png";
//				cv::String filenamein = stream.str();
//
//				//cout << filenamein << endl;
//				cv::Mat elemimg = cv::imread(filenamein);
//
//				//cout << "ok1" << endl;
//
//				for (int v = 0; v < 16; v++) {
//					for (int u = 0; u < 16; u++) {
//						img_display.at<cv::Vec3b>(startv + v, startu + u)[0] = elemimg.at<cv::Vec3b>(v, u)[0];
//						img_display.at<cv::Vec3b>(startv + v, startu + u)[1] = elemimg.at<cv::Vec3b>(v, u)[1];
//						img_display.at<cv::Vec3b>(startv + v, startu + u)[2] = elemimg.at<cv::Vec3b>(v, u)[2];
//					}
//				}
//
//				//elemimg.release();
//			}
//		}
//
//		stream.str("");
//		stream.clear();
//		stream << "D:/Users/taw11/projects/code/images/movie/simulation/E/g4_wp1.25_zi800_pd0.075_Wi200/frame" << paddedf << "/img_display_frame" << paddedf << ".png";
//		cv::String filenameout = stream.str();
//
//		cv::imwrite(filenameout, img_display);
//	}
//
//	return 0;
//}
//
//
//int main(int argc, char* argv[])
//{
//
//	const int numThreads = 8;
//    vector<thread> threads;
//    int rowsPerThread = 300 / numThreads;
//
//    int startRow, endRow;
//    for (int i = 0; i < numThreads; i++) {
//        startRow = i * rowsPerThread;
//        endRow = (i == numThreads - 1) ? 300 : (i + 1) * rowsPerThread;
//        threads.emplace_back(load, startRow, endRow);
//        //(int WIDTH, int HEIGHT, int Wpin, int N, int D, double g, double wp, double inv_wp, double intv, int starty, int endy, double* uo, double* vo, rs2::vertex * verts, cv::Mat color_image, cv::Mat img_display, double* val_z);
//    }
//
//    for (auto& t : threads) {
//        t.join();
//    }
//
//
//	return 0;
//}

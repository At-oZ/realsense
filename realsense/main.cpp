//#include <librealsense2/rs.hpp>
//#include <opencv2/opencv.hpp>
//#include <iostream>
//
//int main(int argc, char* argv[]) try
//{
//    int WIDTH = 640;
//    int HEIGHT = 480;
//    int FPS = 30;
//    rs2::config config;
//    config.enable_stream(RS2_STREAM_COLOR, WIDTH, HEIGHT, RS2_FORMAT_BGR8, FPS);
//    config.enable_stream(RS2_STREAM_DEPTH, WIDTH, HEIGHT, RS2_FORMAT_Z16, FPS);
//    rs2::pipeline pipe;
//    pipe.start(config);
//
//    rs2::colorizer color_map;
//    rs2::align align(RS2_STREAM_COLOR);
//
//    for (int i = 0; i < 3; i++)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        cv::waitKey(10);
//    }
//
//    while (true)
//    {
//        rs2::frameset frames = pipe.wait_for_frames();
//        auto aligned_frames = align.process(frames);
//        rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
//        rs2::video_frame depth_frame = aligned_frames.get_depth_frame().apply_filter(color_map);
//
//        cv::Mat color_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
//        cv::Mat depth_image(cv::Size(WIDTH, HEIGHT), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
//
//        cv::Mat images(cv::Size(2 * WIDTH, HEIGHT), CV_8UC3);
//        cv::Mat color_positon(images, cv::Rect(0, 0, WIDTH, HEIGHT));
//        color_image.copyTo(color_positon);
//        cv::Mat depth_positon(images, cv::Rect(WIDTH, 0, WIDTH, HEIGHT));
//        depth_image.copyTo(depth_positon);
//        cv::imshow("images", images);
//
//        if (cv::waitKey(10) == 27)  // ESCƒL[‚ÅI—¹
//        {
//            cv::destroyAllWindows;
//            break;
//        }
//    }
//    pipe.stop();
//
//    return EXIT_SUCCESS;
//}
//catch (const rs2::error& e)
//{
//    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
//    return EXIT_FAILURE;
//}
//catch (const std::exception& e)
//{
//    std::cerr << e.what() << std::endl;
//    return EXIT_FAILURE;
//}

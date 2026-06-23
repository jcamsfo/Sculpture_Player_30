#pragma once  // sdl2_local.h


#include <opencv2/opencv.hpp>
#include <string>


void Update_And_Show_Main_Displays_sdl2(std::array<cv::Mat, 4> &Main_Display);

bool Update_And_Show_Main_Displays_sdl2_2(std::array<cv::Mat, 4> &Main_Display);

bool sdl2_multi_imshow(const std::string &window_name, const cv::Mat &image);




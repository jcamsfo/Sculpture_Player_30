#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "process_params.h"



// Gaussian blur with odd kernel; no-op if k<=1. Works on 8U or 32F.
void gauss_blur_inplace(cv::Mat &img_8u_or_32f3, int k);

// Fused non-spatial math. Expects CV_32FC3 BGR in [0..255], in-place.
void process_pixels(cv::Mat &bgr32f, const ProcessParams &local_params);

void process_pixels_dst(cv::Mat &ImageIn_F, const std::vector<int> &local_params);

void process_image_with_shift_float(const cv::Mat& src,
                                    cv::Mat& dst,
                                    const std::vector<int>& p);

void h_shift_u8(const cv::Mat& src, cv::Mat& dst, int shift_px);
void h_shift_float(const cv::Mat &src, cv::Mat &dst, int shift_px);


// ---- Stateful fast path (buffer reuse) ----
class Image_Processor
{
public:
  Image_Processor(int rows, int cols); // preallocs internal buffers
cv::Mat &Process_Image(const cv::Mat &ImageIn_U, const ProcessParams &local_params);

private:
  cv::Mat ShiftedTemp;       // reused 8U tmp for shifting
  cv::Mat Image_Processed_F; // reused 32F working buffer
};

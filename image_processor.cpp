#include "image_processor.h"
#include <vector>
#include "globals.h"
#include "measure2.h"
#include "process_params.h"



// for downstream LED Correction
void h_shift_float(const cv::Mat &src, cv::Mat &dst, int shift_px)
{
  CV_Assert(src.type() == CV_32FC3);
  CV_Assert(!src.empty());

  const int W = src.cols;
  const int H = src.rows;

  int s = shift_px % W;
  if (s < 0)
    s += W;

  dst.create(src.size(), src.type());

  CV_Assert(src.data != dst.data); // no in-place shifting

  if (s == 0)
  {
    src.copyTo(dst);
    return;
  }

  src(cv::Rect(W - s, 0, s, H))
      .copyTo(dst(cv::Rect(0, 0, s, H)));

  src(cv::Rect(0, 0, W - s, H))
      .copyTo(dst(cv::Rect(s, 0, W - s, H)));
}

// For player class
void h_shift_u8(const cv::Mat &src, cv::Mat &dst, int shift_px)
{
  CV_Assert(src.type() == CV_8UC3);

  if (src.empty())
  {
    dst.release();
    return;
  }

  const int W = src.cols;
  const int H = src.rows;

  int s = shift_px % W;
  if (s < 0)
    s += W;

  if (s == 0)
  {
    if (src.data != dst.data)
      src.copyTo(dst);
    return;
  }

  // Check alias BEFORE dst.create()
  const bool alias = (src.data == dst.data);

  dst.create(src.size(), src.type());

  cv::Mat safe_src;
  const cv::Mat &read_src =
      alias ? (src.copyTo(safe_src), safe_src) : src;

  read_src(cv::Rect(W - s, 0, s, H))
      .copyTo(dst(cv::Rect(0, 0, s, H)));

  read_src(cv::Rect(0, 0, W - s, H))
      .copyTo(dst(cv::Rect(s, 0, W - s, H)));
}

// -------- Blur (unchanged API, tiny guard) --------
void gauss_blur_inplace(cv::Mat &ImageInOut_F, int k)
{
  int ks = (k >= 3) ? (k | 1) : 1;
  if (ks <= 1 || ImageInOut_F.empty())
    return;
  cv::GaussianBlur(ImageInOut_F, ImageInOut_F, cv::Size(ks, ks), 0);
}


  // LEAK TESTING  
// -------- Fused non-spatial (with inv255 + clamp) --------
void process_pixels(cv::Mat &ImageIn_F, const ProcessParams &local_params)
{

  CV_Assert(ImageIn_F.type() == CV_32FC3);

  const bool invert = local_params.Gain < 0;
  const float gain_abs = std::abs(local_params.Gain) / 100.0f;
  const float black_add = 2.0f * static_cast<float>(local_params.Black_Level);
  const float t = static_cast<float>(local_params.Image_Gamma) / 100.0f;
  const float hue = static_cast<float>(local_params.Color_Hue) * (float)CV_PI / 180.0f;
  const float cg = static_cast<float>(local_params.Color_Gain) / 100.0f;
  const float ccos = cg * std::cos(hue);
  const float ssin = cg * std::sin(hue);
  const float kGY_RY = -0.5093f;
  const float kGY_BY = -0.1942f;
  const float inv255 = 1.0f / 255.0f;

  cv::parallel_for_(cv::Range(0, ImageIn_F.rows), [&](const cv::Range &r)
                    {
    for (int y = r.start; y < r.end; ++y) {
      cv::Vec3f* row = ImageIn_F.ptr<cv::Vec3f>(y);
      for (int x = 0; x < ImageIn_F.cols; ++x) {
        float B=row[x][0], G=row[x][1], R=row[x][2];

        if (invert){ R=255.f-R; G=255.f-G; B=255.f-B; }
        R = R * gain_abs + black_add;
        G = G * gain_abs + black_add;
        B = B * gain_abs + black_add;

        const float R2 = (R*R) * inv255;
        const float G2 = (G*G) * inv255;
        const float B2 = (B*B) * inv255;
        R = (1.f - t)*R + t*R2;
        G = (1.f - t)*G + t*G2;
        B = (1.f - t)*B + t*B2;

        const float Y  = 0.299f*R + 0.587f*G + 0.114f*B;
        const float RY = R - Y;
        const float BY = B - Y;

        const float RYp = ccos*RY +  ssin*BY;
        const float BYp = ccos*BY + (-ssin)*RY;

        const float Rp  = Y + RYp;
        const float Bp  = Y + BYp;
        const float GYp = kGY_RY*RYp + kGY_BY*BYp;
        const float Gp  = Y + GYp;

        R = std::clamp(Rp, 0.f, 255.f);
        G = std::clamp(Gp, 0.f, 255.f);
        B = std::clamp(Bp, 0.f, 255.f);

        row[x] = cv::Vec3f(B, G, R);
      }
    } });
}



void process_pixels_dst(cv::Mat &ImageIn_F, const std::vector<int> &local_params)
{

  CV_Assert(ImageIn_F.type() == CV_32FC3);

  const bool invert = local_params[GAIN] < 0;
  const float gain_abs = std::abs(local_params[GAIN]) / 100.0f;
  const float black_add = 2.0f * static_cast<float>(local_params[BLACK_LEVEL]);
  const float t = static_cast<float>(local_params[IMAGE_GAMMA]) / 100.0f;
  const float hue = static_cast<float>(local_params[COLOR_HUE]) * (float)CV_PI / 180.0f;
  const float cg = static_cast<float>(local_params[COLOR_GAIN]) / 100.0f;
  const float ccos = cg * std::cos(hue);
  const float ssin = cg * std::sin(hue);
  const float kGY_RY = -0.5093f;
  const float kGY_BY = -0.1942f;
  const float inv255 = 1.0f / 255.0f;

  cv::parallel_for_(cv::Range(0, ImageIn_F.rows), [&](const cv::Range &r)
                    {
    for (int y = r.start; y < r.end; ++y) {
      cv::Vec3f* row = ImageIn_F.ptr<cv::Vec3f>(y);
      for (int x = 0; x < ImageIn_F.cols; ++x) {
        float B=row[x][0], G=row[x][1], R=row[x][2];

        if (invert){ R=255.f-R; G=255.f-G; B=255.f-B; }
        R = R * gain_abs + black_add;
        G = G * gain_abs + black_add;
        B = B * gain_abs + black_add;

        const float R2 = (R*R) * inv255;
        const float G2 = (G*G) * inv255;
        const float B2 = (B*B) * inv255;
        R = (1.f - t)*R + t*R2;
        G = (1.f - t)*G + t*G2;
        B = (1.f - t)*B + t*B2;

        const float Y  = 0.299f*R + 0.587f*G + 0.114f*B;
        const float RY = R - Y;
        const float BY = B - Y;

        const float RYp = ccos*RY +  ssin*BY;
        const float BYp = ccos*BY + (-ssin)*RY;

        const float Rp  = Y + RYp;
        const float Bp  = Y + BYp;
        const float GYp = kGY_RY*RYp + kGY_BY*BYp;
        const float Gp  = Y + GYp;

        R = std::clamp(Rp, 0.f, 255.f);
        G = std::clamp(Gp, 0.f, 255.f);
        B = std::clamp(Bp, 0.f, 255.f);

        row[x] = cv::Vec3f(B, G, R);
      }
    } });
}



// for downstream LED Correction
void process_image_with_shift_float(const cv::Mat &src,
                                    cv::Mat &dst,
                                    const std::vector<int> &p)
{
  CV_Assert(src.type() == CV_32FC3);

  if (p[H_SHIFT] != 0)
    h_shift_float(src, dst, p[H_SHIFT]);
  else
    src.copyTo(dst);

  // LEAK TESTING
  process_pixels_dst(dst, p);

   // LEAK TESTING 
  if (p[FILTER_TYPE])
    gauss_blur_inplace(dst, p[FILTER_TYPE]);

// if(Prog_Frame_Counter%30 == 0)
// std::cout
//     << "GAIN " << p[GAIN]
//     << " BLACK " << p[BLACK_LEVEL]
//     << " COLOR_GAIN " << p[COLOR_GAIN]
//     << " HUE " << p[COLOR_HUE]
//     << " GAMMA " << p[IMAGE_GAMMA]
//     << std::endl;

}



// class version faster  (checked it...not much faster 200 usecs maybe)  for use in player class only
Image_Processor::Image_Processor(int rows, int cols)
{
  ShiftedTemp.create(rows, cols, CV_8UC3);
  Image_Processed_F.create(rows, cols, CV_32FC3);
}

// 8 bits in float out  for use in player class only
// 8 bits in float out  for use in player class only
cv::Mat &Image_Processor::Process_Image(const cv::Mat &ImageIn_U,
                                        const ProcessParams &local_params)
{
  CV_Assert(ImageIn_U.type() == CV_8UC3);

  auto process_total = Clock::now();

  // 8U -> 32F
  ImageIn_U.convertTo(Image_Processed_F, CV_32FC3);

  auto process_start = Clock::now();
  // pixel math (in-place on 32F)

    // LEAK TESTING
  process_pixels(Image_Processed_F, local_params);

  auto process_delta = GetDeltaTime(process_start);

  // optional blur on 32F
      // LEAK TESTING
  if (local_params.Filter_Type)
    gauss_blur_inplace(Image_Processed_F, local_params.Filter_Type);

  auto process_total_delta = GetDeltaTime(process_start);

  // std::cout << "Elapsed Para: " << process_delta << "  process_total_delta "  <<  process_total_delta  << " us\n";

  return Image_Processed_F;
}

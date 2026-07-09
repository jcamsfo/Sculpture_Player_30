#pragma once
#include "player_class_Mat.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <array>
#include <vector>
#include "scheduler.h"
#include "process_params.h"

class Sculpture
{

private:
  bool direc_ = true;
  bool fade_done_ = false;
  Clock::time_point process_start_ = Clock::now();
  float Fade_Time = 0.0f;

  int *Sculpture_Map = nullptr;
  uint16_t *Sampled_Buffer_RGB = nullptr;
  uint16_t *Sampled_Buffer_RGBW = nullptr;
  uint16_t *Sculpture_Data_Mapped = nullptr;

  std::vector<std::vector<cv::Point>> Sample_Points_By_Row;

  void Generate_Sample_Point_Map(void);

public:
  Sculpture(const std::string &filename, const std::string &tag, const std::string &filename2, const std::string &tag2);

  Scheduler Schedule;

  uint16_t *Sculpture_Data_Mapped_Params = nullptr;

  void check_gui_buttons();
  void copy_LED_params_from_gui();
  void copy_LED_params_to_gui();
  void copy_VIDEO_params_to_gui(int which_VP);
  void copy_VIDEO_params_from_gui(int which_VP);
  bool check_schedule();

  bool Load_Tonights_Video(bool Video_On_Time);

  bool Load_New_Movie(string path);
  bool Force_Load_Movie_Now(const std::string &path);

  //*******************************  ON OFF SUNSET RELATED   ******************************************/

  struct OnOffTime_Info
  {
    int day_of_year;

    int on_hour_no_dst;
    int on_minute_no_dst;
    int on_minutes_since_6am_no_dst;

    int on_hour_with_dst;   // for display
    int on_minute_with_dst; // for display

    int off_hour_no_dst;
    int off_minute_no_dst;
    int off_minutes_since_6am_no_dst;
  };

  struct CurrentTimeInfo
  {
    int minutes_since_6am_no_dst;

    int current_hour_with_dst;    // for display
    int current_minutes_with_dst; // for display
  };

  OnOffTime_Info On_Off_Time_Struct;
  CurrentTimeInfo Current_Time_Struct;
  int Minutes_Since_6AM(int hour, int minute);
  bool Parse_Day_Of_Year(const std::string &date_string, int &day_of_year);
  int Get_Day_Of_Year_No_Leap();
  bool Load_Todays_On_Off_Time(const std::string &filename, OnOffTime_Info &today);
  CurrentTimeInfo Get_Current_Time_Info();

  //*******************************  ON OFF SUNSET RELATED   ******************************************/

  bool Program_Start_Up;

  bool Advance(std::array<cv::Mat, 4> &outputs);

  void Fade_To_A(const cv::Mat &A, const cv::Mat &B, cv::Mat &dst, float fade_length_sec, bool &fade_done, bool &start_fade);

  void Fade_To_A_Params(const ProcessParams &A, const ProcessParams &B, ProcessParams &dst, float fade_length_sec, bool &fade_done, bool &start_fade_X);

  // CV_32FC3, source // CV_32FC3, SCULPTURE_IMAGE_ROWS x SCULPTURE_IMAGE_COLS
  // void Save_Samples_Grid_To_Buffer_RGB(const cv::Mat &ImageIn_F, cv::Mat &ImageSubSampled_F);

  // void Save_Staggered_Samples_Grid_To_Buffer_RGB(const cv::Mat &ImageIn_F, cv::Mat &ImageSubSampled_F);

  void Sample_To_Buffer_RGBW_16_Faster(const cv::Mat &ImageIn_F, uint16_t *Sampled_Buffer_RGB);

  void Map_Data_To_Sculpture(uint16_t *Input_Sampled, uint16_t *Output_Mapped);

  void Sample_To_Buffer_RGB_16_From_Map(const cv::Mat &ImageIn_F, uint16_t *Sampled_Buffer_RGB);

  void Show_Map_On_Display(cv::Mat &InOut);

  void Write_Buffer_To_File_For_Test(const std::string &filename,
                                     const void *data,
                                     size_t element_size,
                                     size_t element_count);

  void Unique_DesMoines_Post_Mapped_Process(
      uint16_t *Input_Sampled_Mapped,
      uint16_t *Output_With_Params,
      bool Enable_White_Inside,
      bool Enable_Outside_Pixels,
      uint8_t White_Die_Gain,
      uint8_t Outside_Pixel_Gain,
      uint8_t Splash_Timing_0,
      uint8_t Splash_Timing_1,
      uint8_t Splash_Timing_2,
      uint8_t Splash_Timing_3,
      uint8_t Xilinx_Timing,
      uint8_t Red_Correct,
      uint8_t Green_Correct,
      uint8_t Blue_Correct,
      uint8_t Control_Bits,
      uint8_t Control_Bits_2,
      uint8_t Red_Gain_LED,
      uint8_t Green_Gain_LED,
      uint8_t Blue_Gain_LED);

  bool Write_File;

  long long Program_Frame_Count;

  enum
  {
    CURRENT,
    ON_DECK,
    SWAP,
    TOTAL_PLAYERS // bonus: gives you the array size
  };

  Video_Player_With_Processing VP[3];

  // std::vector<int> Mixer_Params;

  ProcessParams Mixer_Params;

  int loop_counter;

  std::string Current_Movie_Info_File;

private:
  Mat Cross_Faded_F;
  Mat Main_Display_M;
  Mat Down_Stream_Corrected_F;
  Mat Downstream_Display_M;

  bool Show_Grid;
  bool Enable_White;

  void Release_Working_Mats();

  float fade_level_ = 1.0f; // 0 = fully B, 1 = fully A

  vector<vector<int>> Sample_Points_Map;
};
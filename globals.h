#pragma once

#include <mutex>
#include <atomic>
#include <string>


extern long long Prog_Frame_Counter;

enum Player_Param_Index
{
  GAIN = 0,
  BLACK_LEVEL = 1,
  COLOR_GAIN = 2,
  COLOR_HUE = 3,
  IMAGE_GAMMA = 4,
  H_SHIFT = 5,
  ROTATE = 6,  
  SPEED = 7,
  FILTER_TYPE = 8,

  PAUSE_TOGGLE = 9,
  FAST_FORWARD = 10,
  REWIND = 11

};

  enum Default_Limits_Index
  {
    DEFAULT = 0,
    LOWER_LIMIT = 1,
    UPPER_LIMIT = 2,
  };


  struct GuiParams
{
    int Gain = 75;
    int Black_Level = 0;
    int Color_Gain = 100;
    int Color_Hue = 0;
    int Image_Gamma = 25;
    int H_Shift = 0;
    int Rotate = 0;
    int Speed = 100;
    int Filter_Type = 3;
};



inline std::atomic<bool> g_running = true;

struct GuiParamsAtomic
{
    std::atomic<int> Gain{75};
    std::atomic<int> Black_Level{0};
    std::atomic<int> Color_Gain{100};
    std::atomic<int> Color_Hue{0};
    std::atomic<int> Image_Gamma{25};
    std::atomic<int> H_Shift{0};
    std::atomic<int> Rotate{0};
    std::atomic<int> Speed{100};
    std::atomic<int> Filter_Type{3};
};

inline GuiParamsAtomic g_gui_params;



// I'd like to add 8 more sliders  below the time texts  :  
// LED_Gain 0-100  
// LED_Black -25-25  
// LED_Color 0-120  
// LED Gamma 0-100  
// LED_Shift 0-360  
// LED_Red 0-120 
// LED_Green 0-120  
// LED_Blue 0-120  
// hese will also work with globals  


inline std::mutex g_drop_path_mutex;
inline std::string g_drop_path;
inline std::atomic<bool> g_new_drop_path{false};


inline std::atomic<bool> g_gui_sliders_need_update{false};

inline std::atomic<bool> g_gui_save_image_params{false};

inline std::atomic<bool> g_gui_save_video_for_tonight{false};


inline std::atomic<bool> g_gui_pause{false};
inline std::atomic<bool> g_gui_fast_forward{false};
inline std::atomic<bool> g_gui_rewind{false};



inline std::mutex g_current_movie_name_mutex;
inline std::string g_current_movie_name;
inline std::atomic<bool> g_current_movie_name_changed{false};

inline std::mutex g_tonights_movie_name_mutex;
inline std::string g_tonights_movie_name;
inline std::atomic<bool> g_tonights_movie_name_changed{false};


inline std::atomic<int> start_up_hours{0};
inline std::atomic<int> start_up_mins{0};
inline std::atomic<bool> g_start_up_time_changed{false};


inline std::atomic<int> current_hours{0};
inline std::atomic<int> current_mins{0};
inline std::atomic<bool> g_current_time_changed{false};



inline std::atomic<int> video_length_mins{0};
inline std::atomic<int> video_length_secs{0};
inline std::atomic<int> current_position_mins{0};
inline std::atomic<int> current_position_secs{0};
inline std::atomic<bool> current_position_changed{false};













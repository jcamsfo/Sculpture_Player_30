#pragma once

#include <mutex>
#include <atomic>
#include <string>


extern long long Prog_Frame_Counter;

inline std::atomic<bool> g_running = true;


// processing params
struct ParamsAtomic
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

    std::atomic<int> Red_Gain{100};
    std::atomic<int> Green_Gain{100};
    std::atomic<int> Blue_Gain{100};
};


inline ParamsAtomic g_gui_params;

inline ParamsAtomic g_gui_LED_params;  // downstream LED




// I'd like to add 8 more sliders  below the time texts  :  
// LED_Gain 0-100  
// LED_Black -25-25  
// LED_Color 0-120  
// LED Gamma 0-100  
// LED_Shift 0-360  
// LED_Red 0-120 
// LED_Green 0-120  
// LED_Blue 0-120  
// these will also work with globals  


inline std::mutex g_drop_path_mutex;
inline std::string g_drop_path;
inline std::atomic<bool> g_new_drop_path{false};


inline std::atomic<bool> g_gui_sliders_need_update{false};


inline std::atomic<bool> g_gui_save_image_params{false};
inline std::atomic<bool> g_gui_load_image_params{false};
inline std::atomic<bool> g_gui_save_video_for_tonight{false};
inline std::atomic<bool> g_gui_reset_video_controls{false};


inline std::atomic<bool> g_gui_save_LED_params{false};
inline std::atomic<bool> g_gui_show_grid{false};
inline std::atomic<bool> g_gui_enable_white_die{false};
inline std::atomic<bool> g_gui_reset_LED_corrections{false};


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













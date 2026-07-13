#pragma once  // config.h



constexpr int FPS = 30; // 30

// not used when xilinx controls the fps
constexpr int MAIN_LOOP_TARGET__USECS = (1'000'000 + FPS / 2) / FPS;  // rounded



constexpr int SCREEN_IMAGE_ROWS = 196;
constexpr int SCREEN_IMAGE_COLS = 1920;

constexpr int SCULPTURE_IMAGE_ROWS = 18;
constexpr int SCULPTURE_IMAGE_COLS = 240;


// constexpr int SCREEN_IMAGE_ROWS = 280;
// constexpr int SCREEN_IMAGE_COLS = 1024;

// constexpr int SCULPTURE_IMAGE_ROWS = 70;
// constexpr int SCULPTURE_IMAGE_COLS = 256;


constexpr int SCULPTURE_SIZE_RGB = SCULPTURE_IMAGE_ROWS * SCULPTURE_IMAGE_COLS * 3;
constexpr int SCULPTURE_SIZE_RGBW = SCULPTURE_IMAGE_ROWS * SCULPTURE_IMAGE_COLS * 4;

constexpr int SCULPTURE_MAPPED_SIZE_RGBW = 25344;   // 16 bit 

constexpr int SCULPTURE_SEND_SIZE_RGBW = 25600;  // 16 bi t multiple of 256

constexpr int SCULPTURE_SEND_SIZE_RGBW_BYTES = SCULPTURE_SEND_SIZE_RGBW * 2;  // 16 bi t multiple of 256



enum Stagger_Options
{
    NO_STAGGER = 0,
    TOP_ROW_LEFT = 1,
    TOP_ROW_RIGHT = 2,
    LEFT_COLUMN_UP = 3,
    LEFT_COLUMN_DOWN = 4
};

constexpr float H_START = 12;
constexpr float V_START = 23;
constexpr float H_GAP = 7.8;
constexpr float V_GAP = 8.3;
constexpr int STAGGER_TYPE = TOP_ROW_LEFT;



enum Player_Param_Index
{
    GAIN = 0,
    BLACK_LEVEL,
    COLOR_GAIN,
    COLOR_HUE,
    IMAGE_GAMMA,
    H_SHIFT,
    ROTATE,
    SPEED,
    FILTER_TYPE,

    RED_GAIN,
    GREEN_GAIN,
    BLUE_GAIN,
    WHITE_GAIN
};

struct ParamLimit
{
    int Default;
    int Min;
    int Max;
};

// note defaults not used  loaded from file
inline constexpr ParamLimit Process_Default_Limits[] =
{
    {0, 0, 130},
    {0, -100, 100},
    {0, 0, 200},
    {0, -180, 180},
    {0, 0, 100},
    {0, 0, 360},
    {0, -100, 100},
    {0, 0, 300},
    {0, 0, 10},

    {0, 0, 100},
    {0, 0, 100},
    {0, 0, 100},
    {0, 0, 100}    
};












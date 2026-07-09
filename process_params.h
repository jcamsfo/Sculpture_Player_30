
#pragma once

#include "config.h"

// enum Player_Param_Index
// {
//   GAIN = 0,
//   BLACK_LEVEL,
//   COLOR_GAIN,
//   COLOR_HUE,
//   IMAGE_GAMMA,
//   H_SHIFT,
//   ROTATE,
//   SPEED,
//   FILTER_TYPE,

//   RED_GAIN,
//   GREEN_GAIN,
//   BLUE_GAIN

// };

// enum Default_Index
// {
//   DEFAULT,
//   MIN,
//   MAX
// };

// struct ParamLimit
// {
//   int Default;
//   int Min;
//   int Max;
// };

// // constexpr int NUM_PARAMS = 16;

// inline constexpr ParamLimit Process_Default_Limits[] =
//     {
//         {100, -100, 200}, // GAIN
//         {0, -100, 100},   // BLACK_LEVEL
//         {100, 0, 200},    // COLOR_GAIN
//         {0, -180, 180},   // COLOR_HUE
//         {25, 0, 100},     // IMAGE_GAMMA
//         {512, 0, 1023},   // H_SHIFT
//         {0, -100, 100},   // ROTATE
//         {100, 0, 300},    // SPEED
//         {3, 0, 10},       // FILTER_TYPE

//         {100, 0, 130}, // Red_Gain
//         {100, 0, 130}, // Green_Gain
//         {100, 0, 130}  // Blue_Gain
// };

struct ProcessParams
{
  int Gain = Process_Default_Limits[GAIN].Default;
  int Black_Level = Process_Default_Limits[BLACK_LEVEL].Default;
  int Color_Gain = Process_Default_Limits[COLOR_GAIN].Default;
  int Color_Hue = Process_Default_Limits[COLOR_HUE].Default;
  int Image_Gamma = Process_Default_Limits[IMAGE_GAMMA].Default;
  int H_Shift = Process_Default_Limits[H_SHIFT].Default;
  int Rotate = Process_Default_Limits[ROTATE].Default;
  int Speed = Process_Default_Limits[SPEED].Default;
  int Filter_Type = Process_Default_Limits[FILTER_TYPE].Default;

  int Red_Gain = Process_Default_Limits[RED_GAIN].Default;
  int Green_Gain = Process_Default_Limits[GREEN_GAIN].Default;
  int Blue_Gain = Process_Default_Limits[BLUE_GAIN].Default;
};





enum Control_Index
{
  PAUSE_TOGGLE = 0,
  FAST_FORWARD,
  REWIND,
  QUIT
};

struct ControlLimit
{
  int Default;
  int Min;
  int Max;
};

inline constexpr ControlLimit Control_Default_Limits[] =
    {
        {0, 0, 1}, // PAUSE_TOGGLE
        {0, 0, 1}, // FAST_FORWARD
        {0, 0, 1}, // REWIND
        {0, 0, 1}  // QUIT
};


struct ControlValues
{
  int Pause_Toggle = Control_Default_Limits[PAUSE_TOGGLE].Default;
  int Fast_Forward = Control_Default_Limits[FAST_FORWARD].Default;
  int Rewind = Control_Default_Limits[REWIND].Default;
  int Quit = Control_Default_Limits[QUIT].Default;
};

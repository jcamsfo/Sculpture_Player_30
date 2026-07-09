#pragma once

#include <string>
#include "process_params.h"

bool Load_LED_Corrections_From_Text_File(
    const std::string &filename,
    ProcessParams &params);

bool Save_LED_Corrections_To_Text_File(
    const std::string &filename,
    const ProcessParams &params);

bool Load_Gui_Params_From_Text_File(
    const std::string &filename,
    ProcessParams &params);

bool Save_Gui_Params_To_Text_File(
    const std::string &filename,
    const ProcessParams &params);

bool Save_Tonights_Filename(
    const std::string &filename,
    const std::string &movie_path);

bool Read_Tonights_Filename(
    const std::string &filename,
    std::string &movie_path);
#include "file_io.h"

#include <fstream>
#include <sstream>


bool Load_LED_Corrections_From_Text_File(const std::string &filename, ProcessParams &params)
{
    std::ifstream in(filename);

    if (!in)
        return false;

    std::string line;

    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);

        std::string key;
        int value;

        if (!(iss >> key >> value))
            continue;

        if (key == "Gain")
            params.Gain = value;
        else if (key == "Black_Level")
            params.Black_Level = value;
        else if (key == "Image_Gamma")
            params.Image_Gamma = value;
        else if (key == "Color_Gain")
            params.Color_Gain = value;
        else if (key == "Color_Hue")
            params.Color_Hue = value;
        else if (key == "H_Shift")
            params.H_Shift = value;
        else if (key == "Red_Gain")
            params.Red_Gain = value;
        else if (key == "Green_Gain")
            params.Green_Gain = value;
        else if (key == "Blue_Gain")
            params.Blue_Gain = value;
        else if (key == "White_Gain")
            params.White_Gain = value;            
    }

    return true;
}

bool Save_LED_Corrections_To_Text_File(const std::string &filename, const ProcessParams &params)
{
    std::ofstream out(filename);

    if (!out)
        return false;

    out << "# parameters for LEDs\n";
    out << "\n";

    out << "Gain            " << params.Gain << "\n";
    out << "Black_Level     " << params.Black_Level << "\n";
    out << "Image_Gamma     " << params.Image_Gamma << "\n";
    out << "Color_Gain      " << params.Color_Gain << "\n";
    out << "Color_Hue       " << params.Color_Hue << "\n";
    out << "H_Shift         " << params.H_Shift << "\n";

    out << "Red_Gain        " << params.Red_Gain << "\n";
    out << "Green_Gain      " << params.Green_Gain << "\n";
    out << "Blue_Gain       " << params.Blue_Gain << "\n";
    out << "White_Gain      " << params.White_Gain << "\n";    

    return true;
}

bool Load_Gui_Params_From_Text_File(const std::string &filename, ProcessParams &params)
{
    std::ifstream in(filename);

    if (!in)
        return false;

    std::string line;

    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        std::istringstream iss(line);

        std::string key;
        int value;

        if (!(iss >> key >> value))
            continue;

        if (key == "Gain")
            params.Gain = value;
        else if (key == "Black_Level")
            params.Black_Level = value;
        else if (key == "Image_Gamma")
            params.Image_Gamma = value;
        else if (key == "Color_Gain")
            params.Color_Gain = value;
        else if (key == "Color_Hue")
            params.Color_Hue = value;
        else if (key == "H_Shift")
            params.H_Shift = value;
        else if (key == "Rotate")
            params.Rotate = value;
        else if (key == "Speed")
            params.Speed = value;
        else if (key == "Filter")
            params.Filter_Type = value;
    }

    return true;
}

bool Save_Gui_Params_To_Text_File(const std::string &filename, const ProcessParams &params)
{
    std::ofstream out(filename);

    if (!out)
        return false;

    out << "# parameters for video\n";
    out << "\n";

    out << "Gain            " << params.Gain << "\n";
    out << "Black_Level     " << params.Black_Level << "\n";
    out << "Image_Gamma     " << params.Image_Gamma << "\n";
    out << "Color_Gain      " << params.Color_Gain << "\n";
    out << "Color_Hue       " << params.Color_Hue << "\n";
    out << "H_Shift         " << params.H_Shift << "\n";
    out << "Rotate          " << params.Rotate << "\n";
    out << "Speed           " << params.Speed << "\n";
    out << "Filter          " << params.Filter_Type << "\n";

    return true;
}

bool Save_Tonights_Filename(const std::string &filename,
                            const std::string &movie_path)
{
    std::ofstream out(filename);

    if (!out)
        return false;

    out << movie_path << '\n';

    return true;
}

bool Read_Tonights_Filename(const std::string &filename,
                            std::string &movie_path)
{
    std::ifstream in(filename);

    if (!in)
        return false;

    std::getline(in, movie_path);

    return !movie_path.empty();
}
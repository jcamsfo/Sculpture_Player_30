
#include "sculpture.h"
#include "measure2.h"
#include "globals.h"
#include "config.h"
#include "image_processor.h"
#include "fs_utils.h"
#include "scheduler.h"

#include <chrono>
#include <thread>
#include <algorithm> // for std::clamp

#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

using Clock = std::chrono::steady_clock;

#define PRINT_TIMING true

// KEY:   F -> float type (vs unsigned char)     U ->  UMat (vs Mat)
// ImageInNew_U.create(IMAGE_ROWS, IMAGE_COLS, CV_8UC(3));
// ImageInOld_U.create(IMAGE_ROWS, IMAGE_COLS, CV_8UC(3));

Sculpture::Sculpture(const std::string &filename, const std::string &tag, const std::string &filename2, const std::string &tag2)
{

    Program_Start_Up = true;
    VP[CURRENT].Open_Image_File_Mat(filename, "Current");
    VP[ON_DECK].Open_Image_File_Mat(filename2, "On_Deck");
    // image_mixed_ will be sized lazily in Cross_Fade based on A/B

    Read_2D_Number(Sample_Points_Map, "Day_For_Night_Sample_Map.csv");

    Mixer_Params = Mixer_Params_Defaults;

    Load_LED_Corrections("swirl_files/LED_Corrections.txt");

    Sculpture_Map = new int[SCULPTURE_MAPPED_SIZE_RGBW];

    Sampled_Buffer_RGB = new uint16_t[SCULPTURE_SIZE_RGB];
    Sampled_Buffer_RGBW = new uint16_t[SCULPTURE_SIZE_RGBW];

    Sculpture_Data_Mapped = new uint16_t[SCULPTURE_MAPPED_SIZE_RGBW];

    Sculpture_Data_Mapped_Params = new uint16_t[SCULPTURE_SEND_SIZE_RGBW];

    for (int i = 0; i < SCULPTURE_MAPPED_SIZE_RGBW; i++)
        Sculpture_Map[i] = -1;

    FILE *csv = fopen("swirl_files/DM_mapping_out_in.csv", "r");
    if (!csv)
    {
        printf("Cannot open DM_mapping_out_in.csv - must be in same folder as exe.\n");
        return;
    }

    for (int n = 0; n < SCULPTURE_MAPPED_SIZE_RGBW; n++)
    {
        int out_flat, in_flat;

        if (fscanf(csv, "%d,%d", &out_flat, &in_flat) != 2)
        {
            printf("Bad mapping line at %d\n", n);
            continue;
        }

        if (out_flat >= 0 && out_flat < SCULPTURE_MAPPED_SIZE_RGBW)
            Sculpture_Map[out_flat] = in_flat;
    }

    fclose(csv);

    printf("DM map loaded.\n");

    Generate_Sample_Point_Map();

    Write_File = false;
    Program_Frame_Count = 0;
}

bool Sculpture::Force_Load_Movie_Now(const std::string &path)
{
    VP[CURRENT].Close_Image_File();
    VP[CURRENT].Open_Image_File_Mat(path, "main image");
    VP[ON_DECK].Close_Image_File();
    VP[ON_DECK].Open_Image_File_Mat(path, "main image");

    return true;
}

bool Sculpture::Load_Params_From_Text_File(const std::string &filename,
                                           std::vector<int> &params)
{
    std::ifstream in(filename);

    if (!in)
        return false;

    // if (params.size() < NUM_PARAMS)
    //     params.resize(NUM_PARAMS);

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
            params[GAIN] = value;
        else if (key == "Black_Level")
            params[BLACK_LEVEL] = value;
        else if (key == "Image_Gamma")
            params[IMAGE_GAMMA] = value;
        else if (key == "Color_Gain")
            params[COLOR_GAIN] = value;
        else if (key == "Color_Hue")
            params[COLOR_HUE] = value;
        else if (key == "H_Shift")
            params[H_SHIFT] = value;
        else if (key == "Rotate")
            params[ROTATE] = value;
        else if (key == "Speed")
            params[SPEED] = value;
        else if (key == "Filter")
            params[FILTER_TYPE] = value;
    }

    return true;
}

bool Sculpture::Load_New_Movie(string path)
{
    string temp_found_file;

    VP[ON_DECK].Close_Image_File();
    VP[ON_DECK].Open_Image_File_Mat(path, "main image");

    Path_Parsed temp_path = Parse_Filename_Path(path);

    Create_Folder(temp_path.Path_Only, "Movies_Info");

    fs::path info_folder = fs::path(temp_path.Path_Only) / "Movies_Info";

    temp_found_file = Find_File(info_folder.string(), temp_path.Filename_No_Ext + ".txt");

    if (temp_found_file.empty())
    {
        fs::path new_file = info_folder / (temp_path.Filename_No_Ext + ".txt");

        Create_Text_File_If_Missing(
            new_file.string(),
            "# parameters for video\n"
            "\n"
            "Gain            75\n"
            "Black_Level     0\n"
            "Image_Gamma     33\n"
            "Color_Gain      100\n"
            "Color_Hue       0\n"
            "H_Shift         0\n"
            "Rotate          0\n"
            "Speed           100\n"
            "Filter          3\n");

        temp_found_file = new_file.string();
    }

    Current_Movie_Info_File = temp_found_file;

    cout << "\nMOVIE INFO FILE: " << temp_found_file << endl;

    // Load_Params_From_Text_File(temp_found_file,
    //                            VP[ON_DECK].Player_Params);

    // g_gui_params.Gain.store(VP[ON_DECK].Player_Params[GAIN]);
    // g_gui_params.Black_Level.store(VP[ON_DECK].Player_Params[BLACK_LEVEL]);
    // g_gui_params.Color_Gain.store(VP[ON_DECK].Player_Params[COLOR_GAIN]);
    // g_gui_params.Color_Hue.store(VP[ON_DECK].Player_Params[COLOR_HUE]);
    // g_gui_params.Image_Gamma.store(VP[ON_DECK].Player_Params[IMAGE_GAMMA]);
    // g_gui_params.H_Shift.store(VP[ON_DECK].Player_Params[H_SHIFT]);
    // g_gui_params.Rotate.store(VP[ON_DECK].Player_Params[ROTATE]);
    // g_gui_params.Speed.store(VP[ON_DECK].Player_Params[SPEED]);
    // g_gui_params.Filter_Type.store(VP[ON_DECK].Player_Params[FILTER_TYPE]);

    Load_Params_From_Text_File(temp_found_file,
                               VP[CURRENT].Player_Params);

    g_gui_params.Gain.store(VP[CURRENT].Player_Params[GAIN]);
    g_gui_params.Black_Level.store(VP[CURRENT].Player_Params[BLACK_LEVEL]);
    g_gui_params.Color_Gain.store(VP[CURRENT].Player_Params[COLOR_GAIN]);
    g_gui_params.Color_Hue.store(VP[CURRENT].Player_Params[COLOR_HUE]);
    g_gui_params.Image_Gamma.store(VP[CURRENT].Player_Params[IMAGE_GAMMA]);
    g_gui_params.H_Shift.store(VP[CURRENT].Player_Params[H_SHIFT]);
    g_gui_params.Rotate.store(VP[CURRENT].Player_Params[ROTATE]);
    g_gui_params.Speed.store(VP[CURRENT].Player_Params[SPEED]);
    g_gui_params.Filter_Type.store(VP[CURRENT].Player_Params[FILTER_TYPE]);

    g_gui_sliders_need_update.store(true);

    {
        std::lock_guard<std::mutex> lock(g_current_movie_name_mutex);
        g_current_movie_name = fs::path(path).filename().string();
    }

    g_current_movie_name_changed.store(true);

    return true;
}

bool Sculpture::Save_Gui_Params_To_Text_File(const std::string &filename)
{
    std::ofstream out(filename);

    if (!out)
        return false;

    out << "# parameters for video\n";
    out << "\n";

    out << "Gain            " << g_gui_params.Gain.load() << "\n";
    out << "Black_Level     " << g_gui_params.Black_Level.load() << "\n";
    out << "Image_Gamma     " << g_gui_params.Image_Gamma.load() << "\n";
    out << "Color_Gain      " << g_gui_params.Color_Gain.load() << "\n";
    out << "Color_Hue       " << g_gui_params.Color_Hue.load() << "\n";
    out << "H_Shift         " << g_gui_params.H_Shift.load() << "\n";
    out << "Rotate          " << g_gui_params.Rotate.load() << "\n";
    out << "Speed           " << g_gui_params.Speed.load() << "\n";
    out << "Filter          " << g_gui_params.Filter_Type.load() << "\n";

    return true;
}

bool Sculpture::Save_Video_For_Tonight()
{
    std::ofstream out("swirl_files/Tonight.txt");

    if (!out)
        return false;

    out << VP[CURRENT].Last_Good_Filename << '\n';

    {
        std::lock_guard<std::mutex> lock(g_tonights_movie_name_mutex);
        g_tonights_movie_name =
            fs::path(VP[CURRENT].Last_Good_Filename)
                .filename()
                .string();
    }

    g_tonights_movie_name_changed.store(true);

    std::cout << "Saved tonight video: "
              << VP[CURRENT].Last_Good_Filename
              << "\nTo file: "
              << fs::absolute("Tonight.txt")
              << std::endl;

    return true;
}

bool Sculpture::Read_Video_For_Tonight(bool Video_On_Time)
{
    std::ifstream in("swirl_files/Tonight.txt");

    if (!in)
        return false;

    std::string path;
    std::getline(in, path);

    if (path.empty())
        return false;

    {
        std::lock_guard<std::mutex> lock(g_tonights_movie_name_mutex);
        g_tonights_movie_name =
            fs::path(path).filename().string();
    }

    g_tonights_movie_name_changed.store(true);

    if (Video_On_Time)
        Load_New_Movie(path);
    else
        Load_New_Movie("swirl_files/black.mov");

    return true;
}

bool Sculpture::Load_LED_Corrections(const std::string &filename)
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
            Mixer_Params[GAIN] = value;

        else if (key == "Black_Level")
            Mixer_Params[BLACK_LEVEL] = value;

        else if (key == "Image_Gamma")
            Mixer_Params[IMAGE_GAMMA] = value;

        else if (key == "Color_Gain")
            Mixer_Params[COLOR_GAIN] = value;

        else if (key == "Color_Hue")
            Mixer_Params[COLOR_HUE] = value;

        else if (key == "H_Shift")
            Mixer_Params[H_SHIFT] = value;

        else if (key == "Rotate")
            Mixer_Params[ROTATE] = value;

        else if (key == "Speed")
            Mixer_Params[SPEED] = value;

        else if (key == "Filter")
            Mixer_Params[FILTER_TYPE] = value;
    }

    return true;
}

bool Sculpture::Advance(std::array<cv::Mat, 4> &outputs)
{
    bool start_fade = false;
    static bool Video_On_Time = false;
    static bool Video_On_Time_Delayed = false;
    static long long process_swap_delta_stored = 0;

    // check for new video dropped
    if (g_new_drop_path.exchange(false))
    {
        std::string path;

        {
            std::lock_guard<std::mutex> lock(g_drop_path_mutex);
            path = g_drop_path;
        }
        Load_New_Movie(path);

        cout << "Loaded " << path << endl;
        start_fade = true;
    }

    /************************************* SCHEDULING RELATED  *********************************************/

    // compare current time to turn on time
    Video_On_Time = Schedule.Get_Video_On_Time();

    // reload the turn on from file at 4pm
    if (Schedule.Check_Reload_Schedule() || Program_Start_Up) // reload the turn on from file at 4pm
    {
        start_up_hours.store(Schedule.On_Off_Time_Struct.on_hour_with_dst_12);
        start_up_mins.store(Schedule.On_Off_Time_Struct.on_minute_with_dst);
        g_start_up_time_changed.store(true);
    }

    if (Program_Frame_Count%60 == 0) // every 2 seconds
    {
        current_hours.store(Schedule.Current_Time_Struct.current_hour_with_dst_12);
        current_mins.store(Schedule.Current_Time_Struct.current_minutes);
        g_current_time_changed.store(true);
    }


    // Load tonights video or black depending on current time
    if (Program_Start_Up)
    {
        Read_Video_For_Tonight(Video_On_Time);
        start_fade = true;
        Program_Start_Up = false;
    }

    // check for Turn On Time
    if (Video_On_Time && !Video_On_Time_Delayed)
    {
        Read_Video_For_Tonight(true);
        start_fade = true;
    }
    // check for Turn Off Time
    else if (!Video_On_Time && Video_On_Time_Delayed)
    {
        Read_Video_For_Tonight(false);
        start_fade = true;
    }
    Video_On_Time_Delayed = Video_On_Time;

    /************************************* SCHEDULING RELATED  DONE ****************************************/

    /************************************* GUI BUTTONS    **************************************************/
    // save params for the current image from the gui
    if (g_gui_save_image_params.exchange(false))
    {
        if (!Current_Movie_Info_File.empty())
        {
            Save_Gui_Params_To_Text_File(Current_Movie_Info_File);
            std::cout << "Saved image controls to "
                      << Current_Movie_Info_File << std::endl;
        }
    }

    // save the current image to be played tonight
    if (g_gui_save_video_for_tonight.exchange(false))
    {
        Save_Video_For_Tonight();
        std::cout << "Saved tonight's selection"
                  << Current_Movie_Info_File << std::endl;
    }
    /************************************* GUI BUTTONS DONE  ***********************************************/

    process_start_ = Clock::now();

    // start the fading process
    if (start_fade) // SWAP so that it's always fading in the same direction  less confusing
    {
        VP[SWAP] = VP[CURRENT];
        VP[CURRENT] = VP[ON_DECK];
        VP[ON_DECK] = VP[SWAP];
        process_swap_delta_stored = GetDeltaTime(process_start_);
    }

    /******************************  IMAGE PROCESS BOTH IMAGES **************************/
    // VP[CURRENT].Process_New_Frame_Ext_Process();
    // VP[ON_DECK].Process_New_Frame_Ext_Process();
    // thread the 2 players
    std::jthread t1([&]
                    { VP[CURRENT].Process_New_Frame_Ext_Process(); });
    std::jthread t2([&]
                    { VP[ON_DECK].Process_New_Frame_Ext_Process(); });
    t1.join();
    t2.join(); // ⬅️ ensure both finished
    /******************************  IMAGE PROCESS BOTH IMAGES DONE *********************/

    auto process_image_processing_both = GetDeltaTime(process_start_);

    Fade_Time = 2.0;

    // Fade to New Image when triggered  Float Image In Res
    Fade_To_A(VP[CURRENT].ImageMain_FM, VP[ON_DECK].ImageMain_FM, Cross_Faded_F, Fade_Time, fade_done_, start_fade);

    // Convert Cross_Faded_F for Display
    Cross_Faded_F.convertTo(Main_Display_M, CV_8UC3); // Main Visual Output to Screen

    auto pre_dwn_stream = GetDeltaTime(process_start_);

    auto ds_start = Clock::now();

    // // Downstream LED Correction and shift on full size image  Note output could be same as input
    // Mixer_Params[H_SHIFT] = 100;
    process_image_with_shift_float(Cross_Faded_F, Down_Stream_Corrected_F, Mixer_Params); // Sculpture Res

    auto ds = GetDeltaTime(ds_start);

    // // Shrink to a size that's close to the output res and convert to downstream display
    Down_Stream_Corrected_F.convertTo(Downstream_Display_M, CV_8UC3);
    resize(Downstream_Display_M, Downstream_Display_M, cv::Size(), .5, .5, cv::INTER_NEAREST);

    auto process_sampler = Clock::now();
    // // Send Downstream Corrected image to Sampler

    Sample_To_Buffer_RGB_16_From_Map(Down_Stream_Corrected_F, Sampled_Buffer_RGB);

    Map_Data_To_Sculpture(Sampled_Buffer_RGB, Sculpture_Data_Mapped);

    Show_Map_On_Display(Main_Display_M);

    auto after_sho_map = GetDeltaTime(process_start_);

    Unique_DesMoines_Post_Mapped_Process(
        Sculpture_Data_Mapped,        // uint16_t *Input_Sampled_Mapped,
        Sculpture_Data_Mapped_Params, // uint16_t *Output_With_Params,
        true,                         // bool Enable_White_Inside,
        true,                         // bool Enable_Outside_Pixels,
        8,                            // uint8_t White_Die_Gain,
        3,                            // uint8_t Outside_Pixel_Gain,
        93,                           // uint8_t Splash_Timing_0,
        93,                           // uint8_t Splash_Timing_1,
        93,                           // uint8_t Splash_Timing_2,
        93,                           // uint8_t Splash_Timing_3,
        4,                            // uint8_t Xilinx_Timing,
        0,                            // uint8_t Red_Correct,
        0,                            // uint8_t Green_Correct,
        0,                            // uint8_t Blue_Correct,
        1,                            // uint8_t Control_Bits,
        0                             // uint8_t Control_Bits_2
    );

    //  for testing to match old proigram
    if (Write_File)
    {

        Write_Buffer_To_File_For_Test("unmapped_gamma100.bin",
                                      Sampled_Buffer_RGB,
                                      sizeof(uint16_t),
                                      SCULPTURE_SIZE_RGB);

        Write_Buffer_To_File_For_Test("mapped_gamma100.bin",
                                      Sculpture_Data_Mapped,
                                      sizeof(uint16_t),
                                      SCULPTURE_MAPPED_SIZE_RGBW);

        Write_Buffer_To_File_For_Test("mapped_with_params_gamma100.bin",
                                      Sculpture_Data_Mapped_Params,
                                      sizeof(uint16_t),
                                      SCULPTURE_SEND_SIZE_RGBW);

        Write_File = false;
    }

    // // convert to Sculpture Res Float max 255
    // // Save_Staggered_Samples_Grid_To_Buffer_RGB(Cross_Faded_F, Cross_Faded_F_Sampled);

    auto process_sampler_delta = GetDeltaTime(process_sampler);

    outputs[0] = VP[CURRENT].VideoDisplay; // shallow copy
    outputs[1] = VP[ON_DECK].VideoDisplay; // shallow copy
    // outputs[1] =  img8;
    outputs[2] = Main_Display_M;
    outputs[3] = Downstream_Display_M;

    auto process_all_delta = GetDeltaTime(process_start_);

    if ((loop_counter % 30 == 0) && PRINT_TIMING)
        cout << "process_all_delta  " << process_all_delta << "    "
             << "pre_dwn_str  " << pre_dwn_stream << "    "

             << "ds  " << ds << "    "
             << "sho_map  " << after_sho_map << "    "

             << "swap  " << process_swap_delta_stored << "    "
             << "sampler  " << process_sampler_delta << "    "
             << "image_processing_both  " << process_image_processing_both << endl;

    Program_Frame_Count++;
    loop_counter++;
    return true;
}

void Sculpture::Fade_To_A(const cv::Mat &A, const cv::Mat &B, cv::Mat &dst, float fade_length_sec, bool &fade_done, bool &start_fade_local)
{
    CV_Assert(A.size() == B.size() && A.type() == B.type());

    dst.create(A.size(), A.type());

    const float step =
        (fade_length_sec > 0.f)
            ? (1.0f / (fade_length_sec * FPS))
            : 1.0f;

    if (start_fade_local)
    {
        fade_level_ = 0.0f;
    }
    else
    {
        fade_level_ += step;
        fade_level_ = std::clamp(fade_level_, 0.0f, 1.0f);
    }

    fade_done = (fade_level_ >= 1.0f);

    cv::addWeighted(
        A, fade_level_,
        B, 1.0f - fade_level_,
        0.0,
        dst);
}

void Sculpture::Show_Map_On_Display(cv::Mat &InOut)
{
    CV_Assert(InOut.type() == CV_8UC3);

    for (int r = 0; r < Sample_Points_By_Row.size(); ++r)
    {
        const auto &row_pts = Sample_Points_By_Row[r];

        for (int c = 0; c < row_pts.size(); ++c)
        {
            int x = row_pts[c].x;
            int y = row_pts[c].y;

            // InOut.ptr<cv::Vec3b>(y)[x] = cv::Vec3b(255, 255, 255);
            auto &pix = InOut.ptr<cv::Vec3b>(y)[x];

            pix = cv::Vec3b(255, 255, 255) - pix;
        }
    }
    if (Program_Frame_Count == 100)
        std::cout << "Show image size: "
                  << InOut.cols << " x " << InOut.rows << std::endl;
}

void Sculpture::Sample_To_Buffer_RGB_16_From_Map(
    const cv::Mat &ImageIn_F,
    uint16_t *Sampled_Buffer_RGB)
{
    CV_Assert(ImageIn_F.type() == CV_32FC3);

    auto f_to_u16 = [](float v) -> uint16_t
    {
        v *= 256.0f;
        if (v < 0.0f)
            return 0;
        if (v > 65535.0f)
            return 65535;
        return static_cast<uint16_t>(v + 0.5f);
    };

    for (int row_out = 0; row_out < SCULPTURE_IMAGE_ROWS; ++row_out)
    {
        uint16_t *dst = Sampled_Buffer_RGB +
                        row_out * SCULPTURE_IMAGE_COLS * 3;

        const auto &row_pts = Sample_Points_By_Row[row_out];

        for (int col_out = 0; col_out < row_pts.size(); ++col_out)
        {

            int x = row_pts[col_out].x;
            int y = row_pts[col_out].y;
            const cv::Vec3f &pix = ImageIn_F.ptr<cv::Vec3f>(y)[x];

            dst[2] = f_to_u16(pix[0]); // B
            dst[1] = f_to_u16(pix[1]); // G
            dst[0] = f_to_u16(pix[2]); // R

            dst += 3;
        }
    }
    if (Program_Frame_Count == 100)
        std::cout << "Sample image size: "
                  << ImageIn_F.cols << " x " << ImageIn_F.rows << std::endl;
}

void Sculpture::Map_Data_To_Sculpture(uint16_t *Input_Sampled,
                                      uint16_t *Output_Mapped)
{
    for (int n = 0; n < SCULPTURE_MAPPED_SIZE_RGBW; n++)
    {
        int src = Sculpture_Map[n];
        Output_Mapped[n] = (src >= 0) ? Input_Sampled[src] : 0;
    }
}

void Sculpture::Generate_Sample_Point_Map()
{
    Sample_Points_By_Row.clear();
    Sample_Points_By_Row.resize(SCULPTURE_IMAGE_ROWS);

    auto clampi = [](int v, int lo, int hi)
    { return (v < lo) ? lo : (v > hi) ? hi
                                      : v; };

    // include V_START
    float row_f = V_START + V_GAP * 0.5f;

    for (int row_out = 0; row_out < SCULPTURE_IMAGE_ROWS; ++row_out)
    {
        int row_in = int(row_f + 0.5f);
        row_in = clampi(row_in, 0, SCREEN_IMAGE_ROWS - 1);

        Sample_Points_By_Row[row_out].resize(SCULPTURE_IMAGE_COLS);

        float h_phase = (STAGGER_TYPE == NO_STAGGER) ? 0.5f : (STAGGER_TYPE == TOP_ROW_LEFT) ? 0.25f
                                                                                             : 0.75f;

        if (row_out & 1)
            h_phase = 1.0f - h_phase;

        // include H_START
        float col_f = H_START + h_phase * H_GAP;

        for (int col_out = 0; col_out < SCULPTURE_IMAGE_COLS; ++col_out)
        {
            int col_in = int(col_f + 0.5f);
            col_in = clampi(col_in, 0, SCREEN_IMAGE_COLS - 1);

            Sample_Points_By_Row[row_out][col_out] = cv::Point(col_in, row_in);

            col_f += H_GAP;
        }

        row_f += V_GAP;
    }

    for (int r = 0; r < 2; ++r)
    {
        std::cout << "Row " << r << ":\n";

        for (int c = 0; c < 10; c++) // Sample_Points_By_Row[r].size(); ++c)
        {
            const cv::Point &p = Sample_Points_By_Row[r][c];
            std::cout << "(" << p.x << "," << p.y << ") ";
        }

        std::cout << "\n\n";
    }
}

void Sculpture::Write_Buffer_To_File_For_Test(const std::string &filename,
                                              const void *data,
                                              size_t element_size,
                                              size_t element_count)
{
    FILE *f = fopen(filename.c_str(), "wb");
    if (!f)
    {
        printf("Failed to open %s\n", filename.c_str());
        return;
    }

    size_t written = fwrite(data, element_size, element_count, f);
    fclose(f);

    printf("Wrote %zu elements (%zu bytes each) to %s\n",
           written, element_size, filename.c_str());
}

void Sculpture::Unique_DesMoines_Post_Mapped_Process(
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
    uint8_t Control_Bits_2)

{

    constexpr uint16_t New_Parameters_Start = 11540;

    std::fill(Output_With_Params, Output_With_Params + SCULPTURE_SEND_SIZE_RGBW, 0);

    for (unsigned int iisection_cnt = 0; iisection_cnt < 36; iisection_cnt++)
    {
        unsigned int RGBWX_cnt = 0;
        unsigned int iipointer_R = 0, iipointer_G = 0, iipointer_B = 0, iipointer = 0;
        unsigned int Temp_Outside_Pixel = 0;
        int R = 0, G = 0, B = 0, X = 0;

        for (unsigned int iirow_full_cnt = 0; iirow_full_cnt < 22; iirow_full_cnt++)
        {
            for (unsigned int iicolumn_local_cnt = 0; iicolumn_local_cnt < 32; iicolumn_local_cnt++)
            {
                if ((iirow_full_cnt == 11) && (iicolumn_local_cnt == 0))
                    RGBWX_cnt = 0;

                iipointer_R = iipointer_G;
                iipointer_G = iipointer_B;
                iipointer_B = iipointer;
                iipointer = iisection_cnt * 32 + iirow_full_cnt * 1152 + iicolumn_local_cnt;

                if (RGBWX_cnt == 0)
                    R = (int)*(Input_Sampled_Mapped + iipointer);
                else if (RGBWX_cnt == 1)
                    G = (int)*(Input_Sampled_Mapped + iipointer);
                else if (RGBWX_cnt == 2)
                    B = (int)*(Input_Sampled_Mapped + iipointer);
                else if (RGBWX_cnt == 3)
                {
                    Temp_Outside_Pixel = (30 * R + 55 * G + 15 * B) / 100;
                    if (Enable_White_Inside)
                    {
                        X = (R <= G) ? R : G;
                        X = (B <= X) ? B : X;
                        X = (X * White_Die_Gain) / 8;
                        if (X >= 65535)
                            X = 65535;
                    }
                    else
                        X = 0;

                    *(Output_With_Params + iipointer_B) = static_cast<unsigned short>(B - X);
                    *(Output_With_Params + iipointer_G) = static_cast<unsigned short>(G - X);
                    *(Output_With_Params + iipointer_R) = static_cast<unsigned short>(R - X);
                    *(Output_With_Params + iipointer) = static_cast<unsigned short>(X);
                }
                else if (RGBWX_cnt == 4)
                {
                    if (Enable_Outside_Pixels)
                        *(Output_With_Params + iipointer) = static_cast<unsigned short>((Temp_Outside_Pixel * Outside_Pixel_Gain) / 16);
                    else
                        *(Output_With_Params + iipointer) = 0;
                }
                RGBWX_cnt++;
                if (RGBWX_cnt == 5)
                    RGBWX_cnt = 0;
            }
        }
    }

    for (int ii = 0; ii < 36; ii++)
    {
        int Parameters_Offset = ii * 32;
        *(Output_With_Params + New_Parameters_Start + Parameters_Offset) = Splash_Timing_0 + 256 * Splash_Timing_1;
        *(Output_With_Params + New_Parameters_Start + Parameters_Offset + 1) = Splash_Timing_2 + 256 * Splash_Timing_3;
        *(Output_With_Params + New_Parameters_Start + Parameters_Offset + 2) = Xilinx_Timing;
        *(Output_With_Params + New_Parameters_Start + Parameters_Offset + 3) = Red_Correct + 256 * Green_Correct;
        *(Output_With_Params + New_Parameters_Start + Parameters_Offset + 4) = Blue_Correct + 256 * Control_Bits;
        *(Output_With_Params + New_Parameters_Start + Parameters_Offset + 5) = Control_Bits_2;
    }
}

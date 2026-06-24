
#include "measure2.h"
#include "timing.h"
#include "globals.h"
#include "config.h"
#include "sculpture.h"
#include "ftdi_local.h"
#include "sdl2_local.h"

#include "ftd2xx.h"
#include "WinTypes.h"
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

#include <gtkmm.h>
#include <thread>
#include <atomic>
#include <iostream>

#include "ControlPanelWindow.hpp"

using Clock = std::chrono::steady_clock;

// Note Des Moines and other works expect a 60Hz trigger from Xilinx and an FTDI return of 60Hz
// But the video read and process etc are 30Hz

// in globals also
long long Prog_Frame_Counter = 0;

// ---------- Main ----------
int video_processor_main(void)
{

    static Prog_Durations Timing_All;

    FT_HANDLE ftHandle;
    if (!Init_FTDI(ftHandle))
    {
        exit(0);
    }

    char *RxBuffer = nullptr;
    RxBuffer = new char[8192]; // dummy buffer for receiving sync from hardware

    Sculpture SC("swirl_files/black.mov", "black", "swirl_files/black.mov", "black");

    // displays for testing and main
    std::array<cv::Mat, 4> Main_Display;

    bool Start_Main = true;
    bool Main_Done = true;

    bool Start_Delay = false;
    bool Delay_Done = false;

    bool Valid_Advance = false;

    // All for measuring time of functions:
    auto FT_start = Clock::now();
    auto FT_delta = GetDeltaTime(FT_start);

    auto long_frame_start = Clock::now();
    auto long_frame_delta = GetDeltaTime(long_frame_start);

    auto long_frame_start_2 = Clock::now();
    auto long_frame_delta_2 = GetDeltaTime(long_frame_start_2);

    auto short_frame_start = Clock::now();
    auto short_frame_delta = GetDeltaTime(short_frame_start);
    long long short_frame_delta_1 = 0;
    long long short_frame_delta_2 = 0;

    auto Timing_Start = Clock::now();

    auto sdl_show_start = Clock::now();
    auto sdl_show_delta = GetDeltaTime(sdl_show_start);

    auto sdl_show_del_loop_start = Clock::now();
    auto sdl_show_del_loop_delta = GetDeltaTime(sdl_show_del_loop_start);

    long Frame_Counter = 0;
    Prog_Durations Main_Loop_Timing;
    // auto main_start = Clock::now();
    // auto main_delta = GetDeltaTime(main_start);

    while (g_running)
    {

        // wait for the FTDI input from the hardware to start the loop
        bool FT_Read_GTEQ_64 = Check_FT_For_Read(ftHandle);
        if (FT_Read_GTEQ_64) // normally 60fps
        {
            short_frame_delta = GetDeltaTime(short_frame_start);
            short_frame_start = Clock::now();

            if (Start_Main)
                short_frame_delta_1 = short_frame_delta;
            else
                short_frame_delta_2 = short_frame_delta;

            Timing_Start = Clock::now(); // for syncing wait key

            FT_start = Clock::now();

            if (Valid_Advance)
            {
                FTDI_Write_Buffer(ftHandle, SC.Sculpture_Data_Mapped_Params, SCULPTURE_SEND_SIZE_RGBW_BYTES);
            }

            Start_Main = !Start_Main; // normally 30fps
            Main_Done = false;

            Start_Delay = !Start_Main;
            Delay_Done = false;

            FT_delta = GetDeltaTime(FT_start);
        }

        if (Start_Main && !Main_Done)
        {

            Main_Loop_Timing.Start_Delay_Timer();

            auto sculpture_call_start = Clock::now();

            Valid_Advance = SC.Advance(Main_Display);
            if (!Valid_Advance)
                break;

            auto sculpture_call_delta = GetDeltaTime(sculpture_call_start);

            Main_Loop_Timing.End_Delay_Timer();

            if (Prog_Frame_Counter % 30 == 0)
                cout
                    << "   \nlongFrmDelta " << long_frame_delta
                    << "   longFrmDelta_2 " << long_frame_delta_2
                    << "   shortFrmDelta1 " << short_frame_delta_1
                    << "   shortFrmDelta2 " << short_frame_delta_2
                    << "   FT_delta " << FT_delta
                    << "   sdl_show " << sdl_show_delta
                    << "   Main Avg " << Main_Loop_Timing.time_delay_avg_ms
                    << "   Main " << sculpture_call_delta
                    << endl;

            // main_delta = GetDeltaTime(main_start);

            // Xilinx does timing so dont use this for now
            // regulate_main_loop_timing(loop_start, MAIN_LOOP_TARGET__USECS);

            // sleep until 13 ms from Timing_Start out of 16.6
            regulate_delta_from_sync_timing(Timing_Start, 13000);
            long_frame_delta = GetDeltaTime(long_frame_start);
            long_frame_start = Clock::now();

            sdl_show_start = Clock::now();
            if (!Update_And_Show_Main_Displays_sdl2_2(Main_Display))
            {
                g_running = false;
                break;
            }
            sdl_show_delta = GetDeltaTime(sdl_show_start);

            Main_Done = true;
            Prog_Frame_Counter++; // 30fps

            // Main_Loop_Timing.End_Delay_Timer();

            Frame_Counter++;
        }
        else if (Start_Delay)
        {
            // sleep until 13 ms from Timing_Start out of 16.6
            regulate_delta_from_sync_timing(Timing_Start, 13000);
            long_frame_delta_2 = GetDeltaTime(long_frame_start_2);
            long_frame_start_2 = Clock::now();

            // by putting this in the screen update happens twice as often
            // display at 60fps even though video is updated at 30fps ?
            sdl_show_del_loop_start = Clock::now();
            if (!Update_And_Show_Main_Displays_sdl2_2(Main_Display))
            {
                g_running = false;
                break;
            }
            sdl_show_del_loop_delta = GetDeltaTime(sdl_show_del_loop_start);

            Start_Delay = false;
        }
    }

    // shut down black video
    SC.Force_Load_Movie_Now("swirl_files/black.mov");
    for (int i = 0; i < 5; i++)
    {
        SC.Advance(Main_Display);
        FTDI_Write_Buffer(ftHandle, SC.Sculpture_Data_Mapped_Params, SCULPTURE_SEND_SIZE_RGBW_BYTES);
        Delay_Msec(17);
    }

    // Signal GTK to quit from its own thread safely
    Glib::signal_idle().connect_once([]()
                                     {
    auto app = Gtk::Application::get_default();
    if (app)
        app->quit(); });

    return 0;
}

int main(int argc, char *argv[])
{
    std::thread video_thread(video_processor_main);

    auto app = Gtk::Application::create("gtkmm4.control_panel");

    int result = app->make_window_and_run<ControlPanelWindow>(argc, argv);

    g_running = false;

    if (video_thread.joinable())
        video_thread.join();

    return result;
}
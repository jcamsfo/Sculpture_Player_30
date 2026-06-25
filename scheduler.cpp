#include "scheduler.h"
#include "globals.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>

Scheduler::Scheduler()
{
    Load_Schedule();
}

// //*******************************  ON OFF SUNSET RELATED   ******************************************/

void Scheduler::Print_Time_Info() const
{
    std::cout
        << "\nToday's On/Off Time\n"
        << "-------------------\n"
        << "DOY                : " << On_Off_Time_Struct.day_of_year << '\n'
        << "ON no DST          : "
        << On_Off_Time_Struct.on_hour_no_dst << ':'
        << std::setw(2) << std::setfill('0')
        << On_Off_Time_Struct.on_minute_no_dst << '\n'
        << "ON minutes         : "
        << On_Off_Time_Struct.on_minutes_since_6am_no_dst << '\n'
        << "ON with DST        : "
        << On_Off_Time_Struct.on_hour_with_dst << ':'
        << std::setw(2) << std::setfill('0')
        << On_Off_Time_Struct.on_minute_with_dst << '\n'
        << "OFF no DST         : "
        << On_Off_Time_Struct.off_hour_no_dst << ':'
        << std::setw(2) << std::setfill('0')
        << On_Off_Time_Struct.off_minute_no_dst << '\n'
        << "OFF minutes        : "
        << On_Off_Time_Struct.off_minutes_since_6am_no_dst << '\n'
        << std::endl;

    std::cout
        << "\nCurrent Time Info\n"
        << "-----------------\n"
        << "Current Time (DST) : "
        << Current_Time_Struct.current_hour_with_dst << ':'
        << std::setw(2) << std::setfill('0')
        << Current_Time_Struct.current_minutes << '\n'

        << "Minutes Since 6AM : "
        << Current_Time_Struct.minutes_since_6am_no_dst << '\n'
        << std::endl;

    std::cout << std::setfill(' ');
}

int Scheduler::Minutes_Since_6AM(int hour, int minute)
{
    int mins = hour * 60 + minute;

    mins -= 6 * 60; // 6 AM

    if (mins < 0)
        mins += 24 * 60;

    return mins;
}

int Scheduler::Get_Day_Of_Year_No_Leap()
{
    time_t now = time(nullptr);
    tm *local = localtime(&now);

    int doy = local->tm_yday + 1; // 1..365 or 366

    int year = local->tm_year + 1900;

    bool leap_year =
        ((year % 4 == 0) && (year % 100 != 0)) ||
        (year % 400 == 0);

    // After Feb 28 in a leap year, subtract one day
    if (leap_year && local->tm_mon > 1)
    {
        doy--;
    }

    return doy;
}

bool Scheduler::Parse_Day_Of_Year(const std::string &date_string, int &day_of_year)
{
    static const int MonthOffset[12] =
        {
            0, 31, 59, 90, 120, 151,
            181, 212, 243, 273, 304, 334};

    std::string month_str = date_string.substr(0, 3);
    int day = std::stoi(date_string.substr(3));

    int month = 0;

    if (month_str == "Jan")
        month = 1;
    else if (month_str == "Feb")
        month = 2;
    else if (month_str == "Mar")
        month = 3;
    else if (month_str == "Apr")
        month = 4;
    else if (month_str == "May")
        month = 5;
    else if (month_str == "Jun")
        month = 6;
    else if (month_str == "Jul")
        month = 7;
    else if (month_str == "Aug")
        month = 8;
    else if (month_str == "Sep")
        month = 9;
    else if (month_str == "Oct")
        month = 10;
    else if (month_str == "Nov")
        month = 11;
    else if (month_str == "Dec")
        month = 12;
    else
        return false;

    day_of_year = MonthOffset[month - 1] + day;

    return true;
}

bool Scheduler::Load_Todays_On_Off_Time(const std::string &filename, OnOffTime_Info &today)
{
    std::ifstream in(filename);

    if (!in)
        return false;

    int today_doy = Get_Day_Of_Year_No_Leap();

    std::string header;
    std::getline(in, header);

    std::string date_string;

    bool found = false;

    while (true)
    {
        OnOffTime_Info t{};

        if (!(in >> date_string >> t.on_hour_no_dst >> t.on_minute_no_dst >> t.off_hour_no_dst >> t.off_minute_no_dst))
        {
            break;
        }

        if (!Parse_Day_Of_Year(date_string,
                               t.day_of_year))
        {
            continue;
        }

        if (t.day_of_year > today_doy)
            break;

        t.on_minutes_since_6am_no_dst =
            Minutes_Since_6AM(
                t.on_hour_no_dst,
                t.on_minute_no_dst);

        t.off_minutes_since_6am_no_dst =
            Minutes_Since_6AM(
                t.off_hour_no_dst,
                t.off_minute_no_dst);

        t.on_hour_with_dst =
            (t.on_hour_no_dst + 1) % 24;

        t.on_hour_with_dst_12 =
            t.on_hour_with_dst % 12;

        if (t.on_hour_with_dst_12 == 0)
            t.on_hour_with_dst_12 = 12;

        t.on_minute_with_dst =
            t.on_minute_no_dst;

        today = t;
        found = true;
    }

    int on_hour_with_dst = 0;
    int on_minute_with_dst = 0;

    int on_hour_with_dst_12 = 0;

    // if (found)
    // {
    //     int temp = today.on_hour_with_dst > 12 ? today.on_hour_with_dst - 12 : today.on_hour_with_dst;
    //     start_up_hours.store(temp);
    //     start_up_mins.store(today.on_minute_with_dst);
    //     g_start_up_time_changed.store(true);
    // }

    return found;
}

bool Scheduler::Load_Schedule()
{
    if (!Load_Todays_On_Off_Time("swirl_files/DM_On_Off_Times_No_DST.txt",
                                 On_Off_Time_Struct))
    {
        std::cerr << "Could not reload on/off time file\n";
        return false;
    }

    Current_Time_Struct = Get_Current_Time_Info();

    Print_Time_Info();

    return true;
}

Scheduler::CurrentTimeInfo Scheduler::Get_Current_Time_Info()
{
    CurrentTimeInfo info{};

    time_t now = time(nullptr);
    tm *local = localtime(&now);

    info.current_hour_with_dst =
        local->tm_hour;

    info.current_hour_with_dst_12 =
        info.current_hour_with_dst % 12;

    if (info.current_hour_with_dst_12 == 0)
        info.current_hour_with_dst_12 = 12;

    info.current_minutes =
        local->tm_min;

    int hour_no_dst = local->tm_hour;

    if (local->tm_isdst > 0)
    {
        hour_no_dst--;

        if (hour_no_dst < 0)
            hour_no_dst += 24;
    }

    info.minutes_since_6am_no_dst =
        Minutes_Since_6AM(
            hour_no_dst,
            local->tm_min);

    // if (Prog_Frame_Counter % 30 == 0)
    //     std::cout << " \n\n curr "  << Current_Time_Struct.minutes_since_6am_no_dst
    //                                 << "   comp " << On_Off_Time_Struct.on_minutes_since_6am_no_dst
    //                                 << "    " << On_Off_Time_Struct.off_minutes_since_6am_no_dst << std::endl
    //               << std::endl;

    return info;
}

bool Scheduler::Get_Video_On_Time()
{
    Current_Time_Struct = Get_Current_Time_Info();

    return Current_Time_Struct.minutes_since_6am_no_dst >=
               On_Off_Time_Struct.on_minutes_since_6am_no_dst &&
           Current_Time_Struct.minutes_since_6am_no_dst <=
               On_Off_Time_Struct.off_minutes_since_6am_no_dst;
}

bool Scheduler::Check_Reload_Schedule()
{
    static int Cur_Time_Delayed = -1;
    int Cur_Time_Now = Current_Time_Struct.current_hour_with_dst;

    if ((Cur_Time_Now == 16) && (Cur_Time_Delayed == 15))
    {
        Cur_Time_Delayed = Cur_Time_Now;
        return Load_Schedule();
    }

    Cur_Time_Delayed = Cur_Time_Now;
    return false;
}

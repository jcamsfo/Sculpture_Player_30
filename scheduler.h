#pragma once
#include <string>

class Scheduler
{
private:
  int Minutes_Since_6AM(int hour, int minute);
  bool Parse_Day_Of_Year(const std::string &date_string, int &day_of_year);
  int Get_Day_Of_Year_No_Leap();

public:
  Scheduler();
  // Sculpture(const std::string &filename, const std::string &tag);

  //*******************************  ON OFF SUNSET RELATED   ******************************************/

  struct OnOffTime_Info
  {
    int day_of_year = 0;

    int on_hour_no_dst = 0;
    int on_minute_no_dst = 0;
    int on_minutes_since_6am_no_dst = 0;

    int on_hour_with_dst = 0;
    int on_minute_with_dst = 0;

    int on_hour_with_dst_12 = 0;

    int off_hour_no_dst = 0;
    int off_minute_no_dst = 0;
    int off_minutes_since_6am_no_dst = 0;
  };

  struct CurrentTimeInfo
  {
    int minutes_since_6am_no_dst = 0;
    int current_hour_with_dst = 0;
    int current_minutes = 0;
    int current_hour_with_dst_12 = 0;
  };

  OnOffTime_Info On_Off_Time_Struct;
  CurrentTimeInfo Current_Time_Struct;

  bool Load_Todays_On_Off_Time(const std::string &filename, OnOffTime_Info &today);
  CurrentTimeInfo Get_Current_Time_Info();

  bool Get_Video_On_Time();

  void Print_Time_Info() const;

  bool Load_Schedule();

  bool Check_Reload_Schedule();

  //*******************************  ON OFF SUNSET RELATED   ******************************************/
};
#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <unordered_map>

namespace timing {












  
using Vecf = std::vector<float>;
using high_res_clock = std::chrono::high_resolution_clock;

// maps a timing label to a tuple
// tuple: <0> interval timings  microseconds
//        <1> overall min
//        <2> overall max,
//        <3> overall avg
inline std::fstream log_file;
inline std::unordered_map<std::string, std::tuple<Vecf, float, float, float>> timing_map;

// Create log file
void init();
// Add a new entry to timing map for each string. Report error
// if it already exists.
void add_timer_labels(const std::initializer_list<std::string> timing_labels);

struct Timer;
// acts as a Timer factory - will only
// create Timer if the label given
// exists in the map
Timer create_timer(const std::string label);
// timer struct is created and destroyed each frame
struct Timer {
  friend Timer create_timer(std::string);
  ~Timer();
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;
  private:
  Timer(std::string timing_label); // private constructor
  high_res_clock::time_point start;
  high_res_clock::time_point end;
};

// calc_min_max_and_avg for last interval
// update overall min, max, avg for all timers
// log them to file
// clear interval timing vectors
void calc_and_log_interval_timing_data(); // call at end of frame
void calc_and_log_final_timing_data(); // call at end of program

// Accessory functions
std::string return_current_date_and_time();
std::pair<float, float> calc_frames_per_second(const high_res_clock::time_point& time1);

}

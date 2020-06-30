#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <ostream>
#include <fstream>
#include <set>
namespace timing {

using Vecf = std::vector<float>;
using high_res_clock = std::chrono::high_resolution_clock;

// map a timing label to a tuple
// tuple: <0> interval timings  microseconds
//        <1> overall min
//        <2> overall max,
//        <3> overall avg
inline std::unique_ptr<std::ofstream> timing_stream;

inline std::unordered_map<std::string, std::tuple<Vecf, float, float, float>> timing_map;

// Initialize the timing manager.
//      Connect up timing_stream
//      Add a new entry to timing map for each string. Report error
//      if it already exists.
void initialize_timers(std::ostream&, const std::initializer_list<std::string> timing_labels);

struct Timer;

// timer struct is created and destroyed each frame
struct Timer {
  friend Timer create_timer(std::string);
  ~Timer();
  Timer() = delete;
  Timer(std::string timing_label); // private constructor
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  private:
  std::string label;
  high_res_clock::time_point start;
  high_res_clock::time_point end;
};

// calc_min_max_and_avg for last interval
// update overall min, max, avg for all timers
// log them to file
// clear interval timing vectors
void calc_and_log_interval_timing_data(); // call at end of frame

// print timing statistics to cout
// log timing statistics to file
void calc_and_log_final_timing_data(); // call at end of program

// Accessory functions
std::string return_current_date_and_time();
std::pair<float, float> calc_frames_per_second(const high_res_clock::time_point& time1);

}

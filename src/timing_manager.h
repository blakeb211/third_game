#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <functional>
#include <set>
namespace timing {

using Vecf = std::vector<float>;
using high_res_clock = std::chrono::high_resolution_clock;


// hold a reference to the output stream
inline auto timing_ostream = std::ref(std::cout);

// map a timing label to a tuple
// tuple: <0> vector of timings for each frame (microseconds)
//        <1> overall min
//        <2> overall max,
//        <3> overall avg
inline std::unordered_map<std::string, std::tuple<Vecf, float, float, float>> timing_map;

// store the min, max, and average timings for each label for each timing interval 
// The individual frame timings are thrown out. 
// This is the data that will be logged to a file.
inline std::vector<IntervalData> histogram_data;

// Initialize the timing manager.
//      Connect up timing_stream
//      Add a new entry to timing map for each string. Report error
//      if it already exists.
void initialize_timers(std::ostream&, const std::initializer_list<std::string>);

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

// store the min, max, and average timings for each timing interval
struct IntervalData {
  std::unordered_map<std::string, std::tuple<float, float, float>> data_map;

};


// calc_min_max_and_avg for last interval
// store them to histogram_data vector
// clear interval timing vectors
void calc_and_log_interval_timing_data(); // call at end of frame

// print timing statistics to cout
// log timing data to file
void calc_and_log_final_timing_data(std::initializer_list<std::string>); // call at end of program

// Accessory functions
  std::pair<float, float> calc_frames_per_second(const high_res_clock::time_point& time1);

}

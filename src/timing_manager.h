#pragma once
#include <algorithm>
#include <chrono>
#include <fstream>
#include <unordered_map>

namespace timing {

// vectors to hold timing data in microseconds
inline std::vector<float> timings_check_coll;
inline std::vector<float> timings_check_free_frag_coll;
inline std::vector<float> timings_process_set_of_freed_frags;
inline std::vector<float> timings_erase_freed_frags;
inline std::vector<float> timings_remove_dead_ent;
inline std::vector<float> timings_draw_player_code;

struct Timing_Data;
inline std::unordered_map<std::string, Timing_Data> timing_map;

void init();

// make a new timing entry in unordered map
void add_new_timer(std::initializer_list<std::string> timing_labels);

// each timer has a
// string for the label
// vector of durations
// overall min, max, avg
// timer struct is created and destroyed each frame

struct Timing_Data {
  std::vector<float> interval_timings;
  float overall_min;
  float overall_max;
  float overall_avg;
};

struct Timer {
    Timer();
    ~Timer();
};

// calc_min_max_and_avg for last interval
// update overall min, max, avg for all timers
// log them to file
// clear interval timing vectors
void calc_and_log_interval_timing_data();
void calc_and_log_final_timing_data();

// assert that the timer label has an entry in
// the timing_map
Timer create_timer(string label);

// accessory timing functions
string return_current_date_and_time();
std::pair<float, float> calc_frames_per_second();
}

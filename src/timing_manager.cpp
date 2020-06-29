#include "timing_manager.h"
using namespace std;

void timing::initialize_timers(ostream&, const initializer_list<string> timing_labels) { }

timing::Timer::Timer(string s) { this->start = high_res_clock::now(); }

timing::Timer::~Timer() { }

timing::Timer timing::create_timer(const string label) { }

void timing::calc_and_log_interval_timing_data()
{
  // Calculate min max and average for the interval.
  //    auto t_coll_min_max
  //        = minmax_element(global::timings_check_coll.begin(), global::timings_check_coll.end());
  //    auto t_coll_avg = std::accumulate(timings_check_coll.begin(), timings_check_coll.end(), 0)
  //        / timings_check_coll.size();

  // TODO: MAKE THIS ASYNC FOR BETTER SPEED ?
  // Log calculated data to the log file
  //    *log_file << "TIMINGS: "
  //              << "\n";
  //    *log_file << "collisions: " << *t_coll_min_max.first << " " << *t_coll_min_max.second << " "
  //              << t_coll_avg << "\n";

  // Clear the timing interval vector.
  //    timings_check_coll.clear();
}

void timing::calc_and_log_final_timing_data()
{ // call at end of program
}

//
// ACCESSORY FUNCTIONS
//

// free function to calculate the frames per second using
// the time the frame started
pair<float, float> timing::calc_frames_per_second(const high_res_clock::time_point& time1)
{
  auto time2(high_res_clock::now());
  // calculate Frametime in milliseconds
  float fTime { chrono::duration_cast<chrono::duration<float, milli>>(time2 - time1).count() };
  auto fSeconds = fTime / 1000.f;
  auto fps = 1.f / fSeconds;
  return move(make_pair(roundf(fps), fTime));
}

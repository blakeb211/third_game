#include "timing_manager.h"
#include <iostream>
#include <tuple>
#include <memory>
#include <functional>
using namespace std;

void timing::initialize_timers(ostream& os, const initializer_list<string> timing_labels)
{
  timing_ostream = os;
  // assign an os to os
  for (string s : timing_labels) {
    // add timing label to timing_map
    auto it = timing_map.find(s);
    auto end_it = timing_map.end();
    // check to make sure not trying to add same label twice
    if (it != end_it) {
      throw exception("Attempt to initialize timing_map with same key twice");
    }
    // add key to map and initialize the tuple. Init the min value to 999
    timing_map[s] = make_tuple(vector<float>(), 999.f, 0.f, 0.f);
  }
}

timing::Timer::Timer(string s)
{
  if (timing_map.count(s) != 1)
    throw exception("incorrect timer label used to construct a Timer");
  label = s;
  start = high_res_clock::now();
}

timing::Timer::~Timer()
{
  end = high_res_clock::now();
  // calc the duration in microseconds
  float duration { chrono::duration_cast<chrono::duration<float, micro>>(end - start).count() };
  // add the duration to the vector
  auto& tuple_ref = timing_map[label];
  get<0>(tuple_ref).push_back(duration);
  // if the duration is less than the overall min replace the overall min with it.
  if (duration < get<1>(tuple_ref)) {
    get<1>(tuple_ref) = duration;
  }
  // if the duration is greater than the overall max replace the overall max with it.
  if (duration > get<2>(tuple_ref)) {
    get<2>(tuple_ref) = duration;
  }
  // add the duration timing to the average timing so we can divide to get the average
  // at the end
  get<3>(tuple_ref) += duration;
}

void timing::calc_and_log_interval_timing_data()
{
  // divide the accumulated value by the size to get the average
  for (auto& timer_pair : timing_map) { }
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

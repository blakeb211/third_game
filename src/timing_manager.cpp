#include "timing_manager.h"
#include <boost/histogram.hpp>
#include <boost/histogram/ostream.hpp>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
using namespace std;
using Vecf = std::vector<float>;

void timing::initialize_timers(ostream &os, const initializer_list<string> timing_labels)
{
  timing_ostream = os;
  // assign an os to os
  for (string s : timing_labels)
  {
    // add timing label to timing_map
    auto it = timing_map.find(s);
    auto end_it = timing_map.end();
    // check to make sure not trying to add same label twice
    if (it != end_it)
    {
      throw exception("Attempt to initialize timing_map with same key twice");
    }
    // add key to map and initialize the tuple. Init the min value to 999
    timing_map[s] = make_tuple(vector<float>(), 9'999'999.f, 0.f, 0.f);
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
  float duration{chrono::duration_cast<chrono::duration<float, micro>>(end - start).count()};
  // add the duration to the vector
  auto &tuple_ref = timing_map[label];
  get<0>(tuple_ref).push_back(duration);
  // if the duration is less than the overall min replace the overall min with it.
  if (duration < get<1>(tuple_ref))
  {
    get<1>(tuple_ref) = duration;
  }
  // if the duration is greater than the overall max replace the overall max with it.
  if (duration > get<2>(tuple_ref))
  {
    get<2>(tuple_ref) = duration;
  }
  // add the duration timing to the average timing so we can divide to get the average
  // at the end
  get<3>(tuple_ref) += duration;
}

void timing::calc_and_log_interval_timing_data()
{
  // TODO: MAKE THIS ASYNC FOR BETTER SPEED ?
  for (auto &timer_pair : timing_map)
  {
    auto label = timer_pair.first;
    auto &[vec_ref, min_ref, max_ref, avg_ref] = timer_pair.second;
    size_t datapoint_count = vec_ref.size();
    // calc the average for that interval
    avg_ref = avg_ref / datapoint_count;
    // Log calculated data to histogram data structure
    // report the min, max and average for th interval
    auto &os = timing_ostream.get();
    timing::IntervalData idata;
    idata[label] = make_tuple(min_ref, max_ref, avg_ref);
    timing::histogram_data.push_back(idata);
    // reset the tuple values
    avg_ref = 0.f;
    max_ref = 0.f;
    min_ref = 9'999'999.f;
    vec_ref.clear();
  }
}

void timing::calc_and_log_final_timing_data(const initializer_list<string> labels)
{ // create a 1D histogram for each timing label in labels
  using namespace boost::histogram;

  // create a vector of histograms
  using axis_t = axis::regular<>;
  auto h = make_histogram(axis_t(13, 0.0, 13'000));
  vector<decltype(h)> hist_data{};
  auto num_intervals = timing::histogram_data.size();
  for (auto label : labels)
  {
    auto h = make_histogram(axis::regular<>(13, 0.0, 13'000));

    for (auto &interval : timing::histogram_data)
    {
      // verify data for each timing label is present in the Interval
      if (interval.count(label) != 1)
      {
        cout << "timing label given to calc_and_log_final_timing_data is not present!!\n";
        throw exception("incorrect timer label used to construct a Timer");
      }
      // add data from interval to current histogram
      h(get<1>(interval[label]));
    }
    // add histogram to histogram vector
    hist_data.push_back(h);
  }
}

//
// ACCESSORY FUNCTIONS
//

// free function to calculate the frames per second using
// the time the frame started
pair<float, float> timing::calc_frames_per_second(const high_res_clock::time_point &time1)
{
  auto time2(high_res_clock::now());
  // calculate Frametime in milliseconds
  float fTime{chrono::duration_cast<chrono::duration<float, milli>>(time2 - time1).count()};
  auto fSeconds = fTime / 1000.f;
  auto fps = 1.f / fSeconds;
  return move(make_pair(roundf(fps), fTime));
}

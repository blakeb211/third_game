#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "global.h"

using namespace std;
using namespace sf;
typedef chrono::high_resolution_clock high_res_clock;

int main() {
  // Initialization
  using namespace global;
  auto window = create_window();
  Event event;
  float ftAccum = 0.f;  // frametime accumulator
  float keyTimeAccum = 0.f;
  auto log_file = create_log_file(return_current_time_and_date());

  while (window->isOpen()) {
    const auto timePoint1 = high_res_clock::now();

    // Get user input
    handle_keyboard_input(keyTimeAccum, keyInputStep);

    if (check_for_window_close(*window, event)) {
      break;
    }
    // Update phase

    for (; ftAccum >= ftStep; ftAccum -= ftStep) {
      /*  */
    }

    // Draw Phase
    window->clear(Color::Black);
    window->display();

    // Frame Timings
    auto timings = calc_frames_per_second(timePoint1);
    auto fps_string = "FPS: " + to_string(static_cast<int>(timings.first));
    window->setTitle(fps_string);
    ftAccum += timings.second;
    keyTimeAccum += timings.second;
    *log_file << fps_string << "\n";
  }
  log_file->close();
  cerr << "Logfile closing" << endl;
  return 0;
}

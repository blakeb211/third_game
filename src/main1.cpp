#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "global.h"

using namespace std;
using namespace sf;
using namespace global;
typedef chrono::high_resolution_clock high_res_clock;
typedef Vector2f Vec2;

vector<RectangleShape> shapes;

void rotated_rectange_init() {
  for (int i = 0; i < 10; i++) {
    RectangleShape r;
    r.setSize(Vec2(rand_engine() % 50, rand_engine() % 50));
    r.setPosition(Vec2(rand_engine() % 500, rand_engine() % 700));
    r.setFillColor(Color(rand_engine() % 250, rand_engine() % 250,
                         rand_engine() % 250, rand_engine() % 250));
    r.setOrigin(Vec2(0.f, 0.f));
    shapes.push_back(move(r));
  }
}

void rotated_rectange_update() {
  for (auto& r : shapes) {
    r.move(Vec2(-2.f + rand_engine() % 5, -3.f + rand_engine() % 5));
  }
}

int main() {
  // Initialization
  auto window = create_window();
  Event event;
  float ftAccum = 0.f;  // frametime accumulator
  float keyTimeAccum = 0.f;
  auto log_file = create_log_file(return_current_time_and_date());
  size_t frameCounter = 0;
  rotated_rectange_init();

  while (window->isOpen()) {
    const auto timePoint1 = high_res_clock::now();

    // Get user input
    if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window)) {
      break;
    }

    if (check_for_window_close(*window, event)) {
      break;
    }

    // Update phase
    for (; ftAccum >= ftStep; ftAccum -= ftStep) {
      rotated_rectange_update();
    }

    // Draw Phase
    window->clear(Color::Black);
    for (const auto& r : shapes) {
      window->draw(r);
    }

    window->display();

    // Frame Timings
    frameCounter++;
    auto timings = calc_frames_per_second(timePoint1);
    auto fps_string = "FPS: " + to_string(static_cast<int>(timings.first));
    window->setTitle(fps_string);
    ftAccum += timings.second;
    keyTimeAccum += timings.second;
    if (frameCounter % 400 == 0) {
      *log_file << fps_string << "\n";
    }
  }

  log_file->close();
  cerr << "Logfile closing" << endl;
  return 0;
}

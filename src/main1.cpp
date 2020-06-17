// Project Purpose: Be able to work on the game and try demos within the same
// codebase by commenting and uncommenting methods in Init and Update sections
// of this file.

#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "global.h"
// #include "entity.h"

using namespace std;
using namespace sf;
using namespace global;
typedef chrono::high_resolution_clock high_res_clock;
typedef Vector2f Vec2;

vector<RectangleShape> shapes;

void rotated_rectange_init() {
  for (int i = 0; i < 1000; i++) {
    RectangleShape r;
    r.setOrigin(Vec2(winWidth / 2.f, winHeight / 2.f));
    r.setSize(Vec2(30 + rand_engine() % 60, 30 + rand_engine() % 60));
    r.setPosition(Vec2(500 + rand_engine() % 100, 360 + rand_engine() % 100));
    r.setFillColor(Color(rand_engine() % 250, rand_engine() % 250,
                         rand_engine() % 250, rand_engine() % 250));
    r.setOrigin(Vec2(0.f, 0.f));
    shapes.push_back(move(r));
  }
}

void rotated_rectange_update() {
  for (auto& r : shapes) {
    r.rotate(rand_engine() % 3);
  }
}

void init_player_test() {
  entity.push_back(make_shared<Player>());
  cout << "init player test running" << endl;
  cout << "entity.size() = " << entity.size() << endl;
  cout << "entity[0].frags.size() = " << get_entity_with_id(0)->frags.size()
       << endl;
}

void update_player_test(const float& ftStep) {
  for (const auto& e : entity) {
    e->update(ftStep);
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

  //
  // initialize demo
  //
  // rotated_rectange_init();
  init_player_test();

  while (window->isOpen()) {
    const auto timePoint1 = high_res_clock::now();

    // Get user input
    if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window)) {
      break;
    } else if (check_for_window_close(*window, event)) {
      break;
    }

    if (ftAccum >= ftStep) {
      // Draw Phase
      window->clear(Color::Black);
      for (const auto& s : shapes) {
        window->draw(s);
      }
      for (const auto& e : entity) {
        for (const auto& f : e->frags) {
          window->draw(f);
        }
      }
      window->display();
    }

    // Update phase
    for (; ftAccum >= ftStep; ftAccum -= ftStep) {
      // rotated_rectange_update();
      update_player_test(ftStep);
    }

    // Frame Timings
    frameCounter++;
    auto fps_string = "FPS: " + to_string(lastFPS);
    window->setTitle(fps_string);
    if (frameCounter % 400 == 0) {
      *log_file << fps_string << "\n";
    }
    auto timings = calc_frames_per_second(timePoint1);
    lastFPS = static_cast<int>(timings.first);
    ftAccum += timings.second;
    keyTimeAccum += timings.second;
  }

  log_file->close();
  cerr << "Logfile closing" << endl;
  return 0;
}

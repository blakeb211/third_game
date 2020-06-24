// Project Purpose: Be able to work on the game and try demos within the same
// codebase by commenting and uncommenting methods in Init and Update sections
// of this file.

#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "global.h"
// #include "entity.h"
//#define HITBOX

using namespace std;
using namespace sf;
using namespace global;
typedef chrono::high_resolution_clock high_res_clock;
typedef Vector2f Vec2;

vector<RectangleShape> shapes;

//
// ROTATED RECTANGES
//
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

void draw_rotated_rectange(RenderWindow& window) {
  for (const auto& s : shapes) {
    window.draw(s);
  }
}

//
// PLAYER TEST
//

void build_long_wall_player_test(Vec2 start1, Vec2 end1) {
  // start of build a multi-segment wall
  auto wall_start = move(start1);
  auto wall_end = (end1);
  auto wall_vec = wall_end - wall_start;
  const auto length = hypot(wall_vec.x, wall_vec.y);
  const auto unit_vec = wall_vec / length;

  while (calc_dist(wall_start, wall_end) >= 2.8f * bW) {
    // place voxel
    auto tmp_end = wall_start + unit_vec * 5.f * static_cast<float>(bW);
    entity.push_back(make_shared<BouncyWall>(wall_start, tmp_end));
    wall_start = tmp_end + unit_vec * 0.2f * static_cast<float>(bW);
  }
  // end of build a multi-segment wall
}

void init_player_test() {
  entity.push_back(make_shared<Player>());

  build_long_wall_player_test(Vec2(0.f, blockWidth),
                              Vec2(winWidth / 3.f, blockWidth));
  build_long_wall_player_test(Vec2(2.f * winWidth / 3.f, blockWidth),
                              Vec2(winWidth, blockWidth));

  build_long_wall_player_test(Vec2(0, winHeight / 10.f),
                              Vec2(3.f * winWidth / 12.f, winHeight / 20.f));
  build_long_wall_player_test(Vec2(2.f * winWidth / 3.f, winHeight / 20.f),
                              Vec2(winWidth, winHeight / 10.f));

  build_long_wall_player_test(
      Vec2(0, 3.f * winHeight / 9.f),
      Vec2(2.f * winWidth / 12.f, 3.f * winHeight / 20.f));
  build_long_wall_player_test(
      Vec2(2.f * winWidth / 3.f, 3.f * winHeight / 20.f),
      Vec2(winWidth, 3.f * winHeight / 9.f));

  global::entity.push_back(make_shared<Enemy>(Enemy(0)));
  // print debug information
}

void update_player_test(const float& ftStep) {
  for (auto& e : entity) {
    e->update(ftStep);
  }
  for (auto& f : free_frags) {
    f.update();
  }

  {
    Timer t("check entities for collisions", global::timings_check_coll);
    global::check_entities_for_collisions();
  }
  {
    Timer t("process set of freed frags",
            global::timings_process_set_of_freed_frags);
    global::process_set_of_freed_frags();
  }
  {
    Timer t("erase freed frags", global::timings_erase_freed_frags);
    global::erase_freed_frags();
  }

  {
    Timer t("remove dead entities", global::timings_remove_dead_ent);
    global::remove_dead_entities();
  }
  {
    Timer t("check free frags for collisions", global::timings_check_free_frag_coll);
    global::check_free_frags_for_collisions();
  }
}

void draw_player_test(RenderWindow& window) {
  Timer t("draw player test", global::timings_draw_player_code);
  for (const auto& e : entity) {
    for (const auto& f : e->frags) {
      window.draw(f);
    }
#ifdef HITBOX
    window.draw(e->hitbox);
#endif
  }
  // draw free frags
  for (const auto& f : free_frags) {
    window.draw(f);
  }
}

//
// MAIN PROGRAM
//
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
      draw_player_test(*window);
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

    if (frameCounter % 600 == 0) {
      *log_file << fps_string << "\n";
      *log_file << "Entity.size() " << global::entity.size() << "\n";
      *log_file << "frags_to_move.size() " << global::frags_to_move.size()
                << "\n";
      *log_file << "free_frags.size() " << global::free_frags.size() << "\n";
      // timing data
      auto t_coll_min_max = minmax_element(global::timings_check_coll.begin(),
                                           global::timings_check_coll.end());
      auto t_coll_avg = std::accumulate(timings_check_coll.begin(),
                                        timings_check_coll.end(), 0) /
                        timings_check_coll.size();
      auto t_process_min_max =
          minmax_element(global::timings_process_set_of_freed_frags.begin(),
                         global::timings_process_set_of_freed_frags.end());
      auto t_process_avg =
          std::accumulate(timings_process_set_of_freed_frags.begin(),
                          timings_process_set_of_freed_frags.end(), 0) /
          timings_process_set_of_freed_frags.size();
      auto t_erase_min_max =
          minmax_element(global::timings_erase_freed_frags.begin(),
                         global::timings_erase_freed_frags.end());
      auto t_erase_avg = std::accumulate(timings_erase_freed_frags.begin(),
                                         timings_erase_freed_frags.end(), 0) /
                         timings_erase_freed_frags.size();
      auto t_remove_ent_min_max =
          minmax_element(global::timings_remove_dead_ent.begin(),
                         global::timings_remove_dead_ent.end());
      auto t_remove_ent_avg =
          std::accumulate(timings_remove_dead_ent.begin(),
                          timings_remove_dead_ent.end(), 0) /
          timings_remove_dead_ent.size();
      auto t_draw_min_max =
          minmax_element(global::timings_draw_player_code.begin(),
                         global::timings_draw_player_code.end());
      auto t_draw_avg = std::accumulate(timings_draw_player_code.begin(),
                                        timings_draw_player_code.end(), 0) /
                        timings_draw_player_code.size();
      auto ff_coll_min_max =
          minmax_element(global::timings_check_free_frag_coll.begin(),
                         global::timings_check_free_frag_coll.end());
      auto ff_coll_avg = std::accumulate(timings_check_free_frag_coll.begin(),
                                        timings_check_free_frag_coll.end(), 0) /
                        timings_check_free_frag_coll.size();


      *log_file << "TIMINGS: "
                << "\n";
      *log_file << "collisions: " << *t_coll_min_max.first << " "
                << *t_coll_min_max.second << " " << t_coll_avg << "\n";
      *log_file << "ff collisions: " << *ff_coll_min_max.first << " "
                << *ff_coll_min_max.second << " " << ff_coll_avg << "\n";
      *log_file << "process freed frags: " << *t_process_min_max.first << " "
                << *t_process_min_max.second << " " << t_process_avg << "\n";
      *log_file << "eraes freed frags: " << *t_erase_min_max.first << " "
                << *t_erase_min_max.second << " " << t_erase_avg << "\n";
      *log_file << "remove entities: " << *t_remove_ent_min_max.first << " "
                << *t_remove_ent_min_max.second << " " << t_remove_ent_avg
                << "\n";
      *log_file << "draw code: " << *t_draw_min_max.first << " "
                << *t_draw_min_max.second << " " << t_draw_avg << "\n";

      global::timings_check_coll.clear();
      global::timings_check_free_frag_coll.clear();
      global::timings_process_set_of_freed_frags.clear();
      global::timings_erase_freed_frags.clear();
      global::timings_remove_dead_ent.clear();
      global::timings_draw_player_code.clear();
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

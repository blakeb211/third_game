// Project Purpose: Be able to work on the game and try demos within the same
// codebase by commenting and uncommenting methods in Init and Update sections
// of this file.

#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "global.h"
#include "timing_manager.h"
//#define HITBOX

using namespace std;
using namespace sf;
using namespace global;
typedef chrono::high_resolution_clock high_res_clock;
typedef Vector2f Vec2;

vector<RectangleShape> shapes;

//
// PLAYER TEST
//

void build_long_wall(Vec2 wall_start, Vec2 wall_end)
{
  // start of build a multi-segment wall
  const auto unit_vec = make_unit_vec(wall_end - wall_start);

  while (calc_dist(wall_start, wall_end) >= 2.8f * bW) {
    // place voxel
    auto tmp_end = wall_start + unit_vec * 5.f * static_cast<float>(bW);
    entity.push_back(make_shared<BouncyWall>(wall_start, tmp_end));
    wall_start = tmp_end + unit_vec * 0.2f * static_cast<float>(bW);
  } // end of build a multi-segment wall
}

void init_player_test()
{
  entity.push_back(make_shared<Player>());

  build_long_wall(Vec2(0.f, blockWidth), Vec2(winWidth / 3.f, blockWidth));
  build_long_wall(Vec2(2.f * winWidth / 3.f, blockWidth), Vec2(winWidth, blockWidth));

  build_long_wall(Vec2(0, winHeight / 10.f), Vec2(3.f * winWidth / 12.f, winHeight / 20.f));
  build_long_wall(Vec2(2.f * winWidth / 3.f, winHeight / 20.f), Vec2(winWidth, winHeight / 10.f));

  build_long_wall(Vec2(winWidth / 12.f, 3.f * winHeight / 9.f),
      Vec2(2.f * winWidth / 12.f, 3.f * winHeight / 20.f));
  build_long_wall(Vec2(2.f * winWidth / 3.f, 3.f * winHeight / 20.f),
      Vec2(11.5f * winWidth / 12.f, 3.f * winHeight / 9.f));

  build_long_wall(
      Vec2(blockWidth, 1.f * blockWidth + winHeight / 3.f), Vec2(blockWidth, winHeight));

  build_long_wall(Vec2(winWidth - blockWidth, 1.f * blockWidth + winHeight / 3.f),
      Vec2(winWidth - blockWidth, winHeight));

  for (auto i : { 1, 1, 1, 1, 1, 1 }) {
    global::entity.push_back(make_shared<Enemy>(Enemy(0)));
  }

  for (auto i : { 1, 1, 1, 1, 1, 1 }) {
    global::entity.push_back(make_shared<Enemy>(Enemy(1)));
  }
}

void update_player_test(const float& ftStep)
{
  for (auto& e : entity) {
    e->update(ftStep);
  }
  for (auto& f : free_frags) {
    f.update();
  }

  //  {
  //    Timer t("check entities for collisions", global::timings_check_coll);
  global::check_entities_for_collisions();
  //  }
  //  {
  //    Timer t("process set of freed frags", global::timings_process_set_of_freed_frags);
  global::process_set_of_freed_frags();
  //  }
  //  {
  //    Timer t("erase freed frags", global::timings_erase_freed_frags);
  global::erase_freed_frags();
  //  }
  //
  //  {
  //    Timer t("remove dead entities", global::timings_remove_dead_ent);
  global::remove_dead_entities();
  //  }
  //  {
  //    Timer t("check free frags for collisions", global::timings_check_free_frag_coll);
  global::check_free_frags_for_collisions();
  //  }
}

void draw_player_test(RenderWindow& window)
{
  //Timer t("draw player test", global::timings_draw_player_code);
  for (const auto& e : entity) {
    for_each(begin(e->frags), end(e->frags), [&window](const Frag& f) { window.draw(f); });
#ifdef HITBOX
    window.draw(e->hitbox);
#endif
  }
  // draw free frags
  for_each(begin(free_frags), end(free_frags), [&window](const Frag& f) { window.draw(f); });
}

//
// MAIN PROGRAM
//
int main()
{
  // Initialization
  auto window = create_window();
  Event event;
  float ftAccum = 0.f; // frametime accumulator
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
    if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window)
        || check_for_window_close(*window, event)) {
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

    // TODO: MAKE LOGGING FREQUENCY A GLOBAL
    if (frameCounter % 600 == 0) {
      *log_file << fps_string << "\n";
      *log_file << "Entity.size() " << global::entity.size() << "\n";
      *log_file << "frags_to_free.size() " << global::frags_to_free.size() << "\n";
      *log_file << "free_frags.size() " << global::free_frags.size() << "\n";

    }
    

    // TODO: replace this with a Timer created at beginning of main
    // game loop
    auto timings = timing::calc_frames_per_second(timePoint1);
    lastFPS = static_cast<int>(timings.first);
    ftAccum += timings.second;
    keyTimeAccum += timings.second;
  }

  log_file->close();
  cerr << "Logfile closing" << endl;
  return 0;
}

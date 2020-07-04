// Project Purpose: Be able to work on the game and try demos within the same
// codebase by commenting and uncommenting methods in Init and Update sections
// of this file.

#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "builder.h"
#include "global.h"
#include "timing_manager.h"
#include <tuple>
//#define HITBOX

using namespace std;
using namespace sf;
using namespace global;
typedef chrono::high_resolution_clock high_res_clock;
typedef Vector2f Vec2;

//
// PLAYER TEST
//
void init_menu()
{
  // Create a graphical text to display
  if (!global::font.loadFromFile("../assets/LucidaSansRegular.ttf"))
  {
    cerr << "Error loading font from file!" << endl;
  }
  else
  {
    cout << "Font loaded from file" << endl;
    float _y {winHeight / 10.f};
    const float _dy = {winHeight / 18.f};
    const float _x {winWidth / 4.f};
    sf::Text text0("(Blake's) Space Invaders - Menu Screen",global::font, 30);
    text0.setPosition(Vec2(_x, _y)); 
    global::menu_text.push_back(text0);
    sf::Text text1("Return_to_game-----Enter key", global::font, 20);
    _y += _dy;
    text1.setPosition(Vec2(_x, _y)); 
    global::menu_text.push_back(text1);
    sf::Text text2("Level_editor---------Tab key", global::font, 20);
    _y += _dy;
    text2.setPosition(Vec2(_x, _y)); 
    global::menu_text.push_back(text2);
    sf::Text text3("Quit_game----------Delete key or click X", global::font, 20);
    _y += _dy;
    text3.setPosition(Vec2(_x, _y)); 
    global::menu_text.push_back(text3);
  }
}

void init_player_test()
{

  // Spawn entities
  entity.push_back(make_shared<Player>());

  builder::build_long_wall(Vec2(0.f, bW), Vec2(winWidth / 3.f, bW));
  builder::build_long_wall(Vec2(2.f * winWidth / 3.f, bW), Vec2(winWidth, bW));

  builder::build_long_wall(Vec2(0, winHeight / 10.f),
                           Vec2(3.f * winWidth / 12.f, winHeight / 20.f));
  builder::build_long_wall(Vec2(2.f * winWidth / 3.f, winHeight / 20.f),
                           Vec2(winWidth, winHeight / 10.f));

  builder::build_long_wall(Vec2(winWidth / 12.f, 3.f * winHeight / 9.f),
                           Vec2(2.f * winWidth / 12.f, 3.f * winHeight / 20.f));
  builder::build_long_wall(Vec2(2.f * winWidth / 3.f, 3.f * winHeight / 20.f),
                           Vec2(11.5f * winWidth / 12.f, 3.f * winHeight / 9.f));

  builder::build_long_wall(Vec2(bW, 1.f * bW + winHeight / 3.f), Vec2(bW, winHeight + 5.f));

  builder::build_long_wall(Vec2(winWidth - bW, 1.f * bW + winHeight / 3.f),
                           Vec2(winWidth - bW, winHeight + 5.f));

  for (auto i : {1, 1, 1, 1, 1, 1})
  {
    global::entity.push_back(make_shared<Enemy>(Enemy(0)));
  }

  for (auto i : {1, 1, 1, 1, 1, 1})
  {
    global::entity.push_back(make_shared<Enemy>(Enemy(1)));
  }
}

void update_player_test(const float &ftStep)
{

  {
    timing::Timer timer("update");
    for (auto &e : entity)
    {
      e->update(ftStep);
    }
    for (auto &f : free_frags)
    {
      f.update();
    }
  }
  {
    timing::Timer t("entity collision");
    global::check_entities_for_collisions();
  }
  {
    timing::Timer timer("process frags");
    global::process_set_of_freed_frags();
    global::erase_freed_frags();
  }

  {
    timing::Timer timer("remove entities");
    global::remove_dead_entities();
  }
  {
    timing::Timer timer("free frag collision");
    global::check_free_frags_for_collisions();
  }
}
void draw_free_frags(RenderWindow &window) {
  global::ff_varray.clear();
  size_t num = 4;
  sf::Color col;
  sf::Transform trans;
  for(auto & f : free_frags) {
    col = f.getFillColor();
    trans = f.getTransform();
    for (int i {0}; i < num; i++) {
      Vec2 p_transformed = trans.transformPoint(f.getPoint(i)); 
      sf::Vertex v = sf::Vertex(p_transformed, col);
      ff_varray.append(v);
    }
  }
  window.draw(ff_varray);
}
void draw_player_test(RenderWindow &window)
{
  timing::Timer timer("drawing");
  for (const auto &e : entity)
  {
    for_each(begin(e->frags), end(e->frags), [&window](const Frag &f) { window.draw(f); });
#ifdef HITBOX
    window.draw(e->hitbox);
#endif
  }
  // draw free frags
  //draw_free_frags(window);
  for_each(begin(free_frags), end(free_frags), [&window](const Frag &f) { window.draw(f); });
}

void draw_menu_screen(RenderWindow &window)
{
  for_each(begin(menu_text), end(menu_text), [&window](const sf::Text t) { window.draw(t); });
}

void update_menu_screen(const float &ftStep)
{
}

//
// MAIN PROGRAM
//
int main()
{
  // Initialization
  auto window = create_window();
  Event event;
  float ftAccum{0.f}; // frametime accumulator
  float keyTimeAccum{0.f};
  auto log_file = create_log_file(return_current_time_and_date());
  size_t frameCounter{0};
  // Setup timers
  timing::initialize_timers(*log_file, {"drawing", "update", "entity collision", "process frags",
                                        "remove entities", "free frag collision", "frametime"});
  state = GAME_STATE::Game;

  // Initialize
  init_player_test();
  init_menu();
  while (window->isOpen())
  {
    if (state == GAME_STATE::Game)
    {

      timing::Timer timer("frametime");
      // Get user input
      if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window) ||
          check_for_window_close(*window, event))
      {
        break;
      }

      if (ftAccum >= ftStep)
      {
        // Draw Phase
        window->clear(Color::Black);
        draw_player_test(*window);
        window->display();
      }

      // Update phase
      for (; ftAccum >= ftStep; ftAccum -= ftStep)
      {
        // rotated_rectange_update();
        update_player_test(ftStep);
      }

      // Frame Timings
      frameCounter++;

      // TODO: MAKE LOGGING FREQUENCY A GLOBAL
      string fps_string = "FPS: ";
      if (get<0>(timing::timing_map["frametime"]).size() > 0)
      {
        auto ftMicro = get<0>(timing::timing_map["frametime"]).back();
        auto ftMilli = ftMicro / 1'000.f;
        auto fps = (int)round(1.f / (ftMilli / 1'000.f));
        fps_string = fps_string + to_string(fps);
        window->setTitle(fps_string);
        ftAccum += ftMilli;
        keyTimeAccum += ftMilli;
      }
      if (frameCounter % 800 == 0)
      {
        *log_file << setw(20) << fps_string << "\n";
        *log_file << setw(20) << "Len(Entity, Free Frag)" << setw(8) << global::entity.size();
        *log_file << setw(8) << global::free_frags.size() << "\n";
        timing::calc_and_log_interval_timing_data();
      }
    } // GAME_STATE::Game

    else if (state == GAME_STATE::Menu)
    {
      // Accumulate keyTimeAccum so that pressing keys doesn't generate too many events in the menu
      keyTimeAccum += global::ftStep;
      // check for switching state back to game
      //
      if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window) ||
          check_for_window_close(*window, event))
      {
        break;
      }
      window->clear(sf::Color(128, 128, 115, 255));
      draw_menu_screen(*window);
      update_menu_screen(global::ftStep);
      window->display();

    } // GAME_STATE::Menu

    else if (state == GAME_STATE::Level_Screen)
    {

    } // GAME_STATE::Level_Screen

  } // Main game loop
  log_file->close();
  cerr << "Logfile closing" << endl;
  return 0;
}

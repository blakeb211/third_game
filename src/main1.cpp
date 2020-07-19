// Project Purpose: Be able to work on the game and try demos within the same
// codebase by commenting and uncommenting methods in Init and Update sections
// of this file.

#include "stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "builder.h"
#include "frag_man.h"
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
    float _y{winHeight / 10.f};
    const float _dy = {winHeight / 18.f};
    const float _x{winWidth / 4.f};
    sf::Text text0("(Blake's) Space Invaders - Menu Screen", global::font, 30);
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

void init_score()
{
  global::txt_score = Text(to_string(global::score), global::font, 22);
  global::txt_score.setFillColor(Color(10, 100, 230, 130));
  global::txt_score.setOutlineColor(Color(10, 10, 180, 100));
  txt_score.setPosition(Vec2(winWidth - 70.f, winHeight - 27.f));
}

void update_score()
{
  global::txt_score.setString(to_string(global::score));
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

bool are_frags_and_varray_synced_up()
{
  assert(global::free_frags.size() == 4*frag_man::ff_varray.getVertexCount());
  for (auto &e : global::entity)
  {
    for (auto i = 0; i < e->frags.size(); i++)
    {
      if (global::calc_dist(e->frags[i].getPosition(), e->varray[i * 4].position) > 0.2f)
        return false;
    }
  }
  
  return true;
}

bool are_free_frags_and_varray_synced_up()
{
  auto sz = global::free_frags.size();
  for (auto i = 0; i < sz; i++)
  {
    auto &fi = global::free_frags[i];
    if (global::calc_dist(fi.getPosition(), frag_man::ff_varray[i*4].position) > 0.2f)
      return false;
  }
  return true;
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
  global::process_set_of_freed_frags();
  global::erase_freed_frags();
  global::remove_dead_entities();
  {
    timing::Timer timer("free frag collision");
    global::check_free_frags_for_collisions();
  }
}

void draw_player_test(RenderWindow &window)
{
  timing::Timer timer("drawing");
  for (const auto &e : entity)
  {
    // draw entity's vertex array in a single draw call
    window.draw(e->varray);
    // auto sz = e->frags.size();
    // for (int i = 0; i < sz; i++)
    //{
    //  window.draw(e->frags[i]);
    //}
#ifdef HITBOX
    window.draw(e->hitbox);
#endif
  }
  // draw free frags
  // auto sz = free_frags.size();
  // for (int i = 0; i < sz; i++)
  //{
  //  window.draw(free_frags[i]);
  //}
  // draw free frags in a single draw call
  window.draw(frag_man::ff_varray);
  // draw player health bar
  sf::CircleShape cs(global::bW * 3, 30);
  cs.setFillColor(sf::Color(255, 178, 102, 60));
  cs.setPosition(winWidth / 30.f, 26.f * winHeight / 27.f);
  for (int i = 0; i < global::playerHealth; i++)
  {
    window.draw(cs);
    cs.move(Vec2(0, -10));
  }
  // draw score
  window.draw(global::txt_score);
}

void draw_menu_screen(RenderWindow &window)
{
  for_each(begin(menu_text), end(menu_text), [&window](const sf::Text t) { window.draw(t); });
}

void draw_level_screen(RenderWindow &window)
{
  sf::Text txt_level("Level: " + to_string(global::level), global::font, 50);
  auto width = txt_level.getLocalBounds().width;
  auto height = txt_level.getLocalBounds().height;
  txt_level.setOutlineThickness(3.f);
  txt_level.setOutlineColor(sf::Color(0, 102, 204, 255));
  txt_level.setPosition(Vec2(winWidth / 2 - (width / 2.f), winHeight / 2 - (height / 2.f)));
  // instructions text
  sf::Text txt_level2("Press enter key to start ", global::font, 30);
  auto width2 = txt_level2.getLocalBounds().width;
  auto height2 = txt_level2.getLocalBounds().height;
  txt_level2.setOutlineThickness(2.f);
  txt_level2.setOutlineColor(sf::Color(0, 52, 204, 255));
  txt_level2.setPosition(
      Vec2(winWidth / 2 - (width2 / 2.f), 4.f * winHeight / 5.f - (height2 / 2.f)));
  window.draw(txt_level);
  window.draw(txt_level2);
}

void draw_game_over_screen(RenderWindow &window)
{
  sf::Text txt("GAME OVER", global::font, 50);
  auto width = txt.getLocalBounds().width;
  auto height = txt.getLocalBounds().height;
  txt.setOutlineThickness(3.f);
  txt.setOutlineColor(sf::Color(0, 102, 204, 255));
  txt.setPosition(Vec2(winWidth / 2 - (width / 2.f), winHeight / 2 - (height / 2.f)));
  // instructions text
  sf::Text txt2("Press escape to exit", global::font, 30);
  auto width2 = txt2.getLocalBounds().width;
  auto height2 = txt2.getLocalBounds().height;
  txt2.setOutlineThickness(2.f);
  txt2.setOutlineColor(sf::Color(0, 52, 204, 255));
  txt2.setPosition(Vec2(winWidth / 2 - (width2 / 2.f), 4.f * winHeight / 5.f - (height2 / 2.f)));
  window.draw(txt);
  window.draw(txt2);
}

void update_level_screen()
{
  // in case we want some moving graphics on the level screen
}

void update_menu_screen(const float &ftStep)
{
  // in case we want some moving graphics on the level screen
}

void draw_editor(RenderWindow &window)
{
}

void update_editor(const float &ftStep)
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
  timing::initialize_timers(
      *log_file, {"drawing", "update", "entity collision", "free frag collision", "frametime"});

  // Initialize
  init_menu();
  init_score();
  global::start_next_level();
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
        update_player_test(ftStep);
        update_score();
        if (is_win_condition_met())
        {
          cout << "Frag Count for Level " << global::level << ": ";
          cout << global::fragCounter << endl;
          start_next_level();
          frameCounter = 0;
        }
        else if (is_lose_condition_met())
        {
          cout << "Frag Count for Level " << global::level << ": ";
          cout << global::fragCounter << endl;
          start_next_level();
          restart_current_level();
          frameCounter = 0;
        }
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
      // Log data every X number of frames. Throw away first several hundred frames
      if (frameCounter % 80 == 0 && frameCounter > 500)
      {
        timing::calc_and_log_interval_timing_data();
      }
    } // GAME_STATE::Game

    else if (state == GAME_STATE::Menu)
    {
      // Accumulate keyTimeAccum so that pressing keys doesn't generate too many events in the menu
      // -> adding fictional time so that timer doesn't sit at 0
      keyTimeAccum += global::ftStep / 2;
      // check for switching state back to game
      //
      if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window) ||
          check_for_window_close(*window, event))
      {
        break;
      }
      window->clear(clearscreen_color);
      draw_menu_screen(*window);
      update_menu_screen(global::ftStep);
      window->display();

    } // GAME_STATE::Menu

    else if (state == GAME_STATE::Level_Screen)
    {
      // adding fictional time so that timer doesn't sit at 0
      keyTimeAccum += global::ftStep / 2;
      if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window) ||
          check_for_window_close(*window, event))
      {
        break;
      }
      // start timer
      window->clear(clearscreen_color);
      draw_level_screen(*window);
      window->display();
      update_level_screen();

    } // GAME_STATE::Level_Screen
    else if (state == GAME_STATE::Editor)
    {
      high_res_clock::time_point editor_time1 = high_res_clock::now();
      // check for switching state back to game
      //
      if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window) ||
          check_for_window_close(*window, event))
      {
        break;
      }
      window->clear(clearscreen_color);
      // Update phase
      for (; ftAccum >= global::ftStep; ftAccum -= global::ftStep)
      {
        update_editor(global::ftStep);
      }
      draw_editor(*window);
      window->display();
      // level editor frame timing
      float ftMilli{chrono::duration_cast<chrono::duration<float, milli>>(high_res_clock::now() -
                                                                          editor_time1)
                        .count()};
      keyTimeAccum += ftMilli;
      ftAccum += ftMilli;
    } // GAME_STATE::Editor
    else if (state == GAME_STATE::Game_Over)
    {
      // adding fictional time so that timer doesn't sit at 0
      keyTimeAccum += global::ftStep / 20;
      if (handle_keyboard_input(keyTimeAccum, keyInputStep, *window) ||
          check_for_window_close(*window, event))
      {
        break;
      }
      // start timer
      window->clear(clearscreen_color);
      draw_game_over_screen(*window);
      window->display();
    } // GAME_STATE::Level_Screen

  } // Main game loop
  timing::calc_and_log_final_timing_data({"drawing", "frametime"});
  log_file->close();
  cout << "Logfile closing" << endl;
  // print out memory information
  cout << "sizeof Frag: " << sizeof(Frag) << endl;
  cout << "sizeof Vec2: " << sizeof(Vec2) << endl;
  cout << "sizeof sf::Vertex " << sizeof(sf::Vertex) << endl;
  return 0;
}

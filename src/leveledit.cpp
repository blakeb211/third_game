/* Level editor:
 *      Create New Level Files
 *      Load Level Files
 *      Edit Newly created or Loaded Level file
 *          place and delete objects
 */

#include "global.h"
/* */
#include "builder.h"
#include "entity.h"
#include "magic_enum.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <array>
#include <boost/circular_buffer.hpp>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <optional>
#include <regex>

using namespace std;
namespace fs = std::filesystem;
using Vec2 = sf::Vector2f;
using high_res_clock = std::chrono::high_resolution_clock;
// global variables
const unsigned MAX_OBJ_COUNT{10};
const unsigned MAX_LEVEL_COUNT{10};
const string TITLE_STRING{"Level Editor"};
float time_accum{0.0};
const float KEY_EVENT_GAP_MILLI{200.0f};
const float ENEMY_NEARBY_CUTOFF{50.f};
optional<size_t> curr_selected_enemy{nullopt};
// array to hold the start and end vector of a wall segment
array<optional<Vec2>, 2> wall_vecs = {nullopt, nullopt};
//

string sep(3, '\n');

// return a vector given in percent as a vector in pixels
Vec2 perc_to_pix(float x, float y)
{
  assert(x >= 0 && x <= 100);
  assert(y >= 0 && y <= 100);
  return Vec2((x / 100.f) * global::winWidth, (y / 100.f) * global::winHeight);
}

optional<size_t> check_for_nearby_enemy(sf::Vector2i mouse_pos)
{
  optional<size_t> ent_id;
  for (auto &e : global::entity)
  {
   if ( e->type == EType::Enemy && global::calc_dist(e->center, Vec2(mouse_pos.x, mouse_pos.y)) < ENEMY_NEARBY_CUTOFF ) {
    ent_id = e->id;
    // if enemy is already selected then unselect it
    if (ent_id == curr_selected_enemy) {
      curr_selected_enemy = nullopt;
      return nullopt;
    }
   }
  }
  return ent_id;
}

int main()
{

  // Create a graphical text to display
  if (!global::font.loadFromFile("../assets/LucidaSansRegular.ttf"))
  {
    cerr << "Error loading font from file!" << endl;
    return 1;
  }
  // print out level editor information
  cout << left << setw(15) << "Window dimensions: " << global::winWidth << " x "
       << global::winHeight;
  cout << right << setw(15) << "blockWidth: " << global::blockWidth << sep;

  // cb_obj circular buffer and print objects out
  // current object is cb_obj.front()
  boost::circular_buffer<EType> cb_obj(MAX_OBJ_COUNT);
  constexpr auto all_objects = magic_enum::enum_values<EType>();
  cout << "Objects available to editor: " << endl;
  for (auto &i : all_objects)
  {
    cb_obj.push_back(i);
    cout << "\t" << static_cast<unsigned>(i) << ": " << magic_enum::enum_name(i) << endl;
  }
  cout << sep;

  // cb_obj.rotate(begin(cb_obj) + 1);

  // build level_file circular buffer and print files out
  // current file is cb_level.front()
  boost::circular_buffer<fs::path> cb_level(MAX_LEVEL_COUNT);
  cout << "Level files available to edit:" << endl;
  string level_path{"../assets/"};
  auto const level_data_regex = regex("level(\\d+)_data(\\.+)txt", regex::ECMAScript);
  // loop over files in the assets directory
  auto file_num = 0;
  for (const auto &entry : fs::directory_iterator(level_path))
  {
    string abs_path = entry.path().string();
    // verify filename matches pattern
    bool const path_matches_regex = regex_search(abs_path, level_data_regex);
    if (path_matches_regex)
    {
      file_num++;
      cout << "\t" << entry.path() << endl;
      cb_level.push_back(entry.path());
    }
  }
  // add an additional file "new_file"
  fs::path new_file = level_path;
  new_file.replace_filename("level" + to_string(++file_num) + "_data.txt");
  cb_level.push_back(new_file);
  cout << "\t" << new_file << endl;
  cout << sep;

  // create window
  auto win = global::create_window();
  win->setVerticalSyncEnabled(true);
  win->setTitle(TITLE_STRING);
  win->display();
  sf::Text mouse_coords("", global::font, 15);

  // text wall adding
  // builder::build_long_wall(perc_to_pix(1, 0), perc_to_pix(1, 99));
  // builder::build_long_wall(perc_to_pix(99, 0), perc_to_pix(99, 99));

  // main loop
  sf::Event event;
  while (true)
  {
    high_res_clock::time_point t_start = high_res_clock::now();
    win->clear(global::clearscreen_color);
    if (global::check_for_window_close(*win, event))
      break;
    // get the mouse position relative to the window

    for (auto e : global::entity)
    {
      bool change_fill_color = (e->id == curr_selected_enemy) ? true : false;
      for (auto f : e->frags)
      {
        // record old vals
        // change vals so drawn differently this frame
        if (change_fill_color) {
          f.setOutlineColor(sf::Color::Red);
          f.setOutlineThickness(3.f);
        }
        else {
          f.setOutlineThickness(0.f);
        }
        win->draw(f);
      }
    }
    // set window current title: Level editor
    stringstream ss;
    ss << "current file: " << left << setw(20) << cb_level.front().filename().string()
       << " current object: " << left << setw(20) << magic_enum::enum_name(cb_obj.front());
    win->setTitle(ss.str());
    // draw mouse position
    auto mouse_pos = sf::Mouse::getPosition(*win);
    mouse_coords.setString("x: " + to_string(mouse_pos.x) + " y: " + to_string(mouse_pos.y));
    auto txt_size = mouse_coords.getLocalBounds();
    auto mt_w = txt_size.width;
    auto mt_h = txt_size.height;
    auto txt_offset = Vec2(mt_w / 2.f, mt_h + 3.f);
    txt_offset.x +=
        (global::winWidth - mouse_pos.x < mt_w * 2.f ? -mt_w * 2.f - 5.f : 0.f); // right case
    txt_offset.x += (mouse_pos.x - mt_w * 2.f - 5.f < 0.f ? +mt_w + 5.f : 0.f);  // left case
    txt_offset.y += (global::winHeight - (mouse_pos.y + mt_h * 4.f) < mt_h ? -mt_h * 3.f - 5.f
                                                                           : 0.f); // bottom case
    txt_offset.y += (mouse_pos.y - mt_h - 5.f < 0.f ? +mt_h + 5.f : 0.f);          // top case
    mouse_coords.setPosition(Vec2(mouse_pos.x, mouse_pos.y) + txt_offset);
    win->draw(mouse_coords);
    // display window
    win->display();

    // handle keyboard events
    bool okayToPlaceObject{false};
    bool okayToMoveToNextObject{false};
    bool enemySelectedForPathAddition{false};
    // Set flags to do Editor Actions in this block
    if (event.type == sf::Event::KeyPressed)
    {
      if (event.key.code == sf::Keyboard::Space && time_accum > KEY_EVENT_GAP_MILLI)
      {
        okayToPlaceObject = true;
      }
      else if (event.key.code == sf::Keyboard::Down && time_accum > KEY_EVENT_GAP_MILLI)
      {
        okayToMoveToNextObject = true;
      }
      else if (event.key.code == sf::Keyboard::P && time_accum > KEY_EVENT_GAP_MILLI)
      {
        if (auto nearby_enemy_id = check_for_nearby_enemy(mouse_pos); nearby_enemy_id  )
        {
          enemySelectedForPathAddition = !enemySelectedForPathAddition;
          curr_selected_enemy = enemySelectedForPathAddition ? nearby_enemy_id : nullopt; 
        }
      }
      time_accum = 0.f;
    }
    // Perform editor actions based on flags set
    if (okayToMoveToNextObject)
    {
      cb_obj.rotate(begin(cb_obj) + 1);
    }

    // place object if flag is set
    if (okayToPlaceObject)
    {
      auto curr_obj = cb_obj.front();
      cout << "placing object type: " << setw(10) << magic_enum::enum_name(curr_obj) << endl;
      switch (curr_obj)
      {
      case EType::Enemy:
        global::entity.push_back(make_shared<Enemy>(1, Vec2(mouse_pos.x, mouse_pos.y)));
        break;
      case EType::BouncyWall:
        if (!wall_vecs[0])
        {
          wall_vecs[0] = Vec2(mouse_pos.x, mouse_pos.y);
        }
        else if (!wall_vecs[1])
        {
          wall_vecs[1] = Vec2(mouse_pos.x, mouse_pos.y);
        }
        if (wall_vecs[0] && wall_vecs[1])
        {
          builder::build_long_wall(*wall_vecs[0], *wall_vecs[1]);
          wall_vecs = {nullopt, nullopt};
        }
        break;
      default:
        cout << "Error - unhandled type placed" << endl;
        break;
      };
    }
    // handle time_accum
    high_res_clock::time_point t_end = high_res_clock::now();
    float ftMilli{chrono::duration_cast<chrono::duration<float, milli>>(t_end - t_start).count()};
    time_accum += ftMilli;
  } // end of main game loop

  return 0;
}

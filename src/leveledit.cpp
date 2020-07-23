/* Level editor:
 *      Create New Level Files
 *      Load Level Files
 *      Edit Newly created or Loaded Level file
 *          place and delete objects
 *
 *      TODO: Logic that creates and sets flags in the main loop is inconsistent.
 *            Change to either set a flag for each action or don't set flags for 
 *            any actions
 */

#include "global.h"
/* */
#include "builder.h"
#include "entity.h"
#include "magic_enum.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <array>
#include <boost/circular_buffer.hpp>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <utility>

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
const string sep(3, '\n');
// array to hold the start and end vector of a wall segment
array<optional<Vec2>, 2> wall_vecs = {nullopt, nullopt};
vector<pair<Vec2, Vec2>> bouncy_wall_vecs;
vector<pair<Vec2, Vec2>> absorby_wall_vecs;

// computed grid spacing that divides evenly into both winWidth and winHeight
constexpr unsigned find_fixed_grid_spacing(unsigned w, unsigned h)
{
  for (int i = global::blockWidth; i < std::min({w, h}); i++)
  {
    if (w % i == 0 && h % i == 0)
    {
      if (i < 15)
      {
        i *= 2;
      }
      return i;
    }
  }
  return 0;
}
constexpr unsigned grid_spacing = find_fixed_grid_spacing(global::winWidth, global::winHeight);

// round vector to grid functions
Vec2 round_vec_to_grid(const Vec2 vec)
{
  int midx = grid_spacing * static_cast<int>(round((round(vec.x) / grid_spacing)));
  int midy = grid_spacing * static_cast<int>(round((round(vec.y) / grid_spacing)));
  return Vec2(midx, midy);
}
Vec2 round_vec_to_grid(sf::Vector2i vec)
{
  return round_vec_to_grid(Vec2(vec.x, vec.y));
}

// return a vector given in percent as a vector in pixels
Vec2 perc_to_pix(float x, float y)
{
  assert(x >= 0 && x <= 100);
  assert(y >= 0 && y <= 100);
  return Vec2((x / 100.f) * global::winWidth, (y / 100.f) * global::winHeight);
}

// find nearby enemy to do enemy selection
optional<size_t> check_for_nearby_enemy(sf::Vector2i mouse_pos)
{
  optional<size_t> ent_id;
  for (auto &e : global::entity)
  {
    if (e->type == EType::Enemy &&
        global::calc_dist(e->center, Vec2(mouse_pos.x, mouse_pos.y)) < ENEMY_NEARBY_CUTOFF)
    {
      ent_id = e->id;
      // if enemy is already selected then unselect it
      if (ent_id == curr_selected_enemy)
      {
        curr_selected_enemy = nullopt;
        return nullopt;
      }
    }
  }
  return ent_id;
}

// save level to file
void save_level_to_file(ofstream &out_file)
{
  // loop to print out enemy paths
  for (auto &e : global::entity)
  {
    if (e->type == EType::Enemy)
    {
      shared_ptr<Enemy> ehandle = dynamic_pointer_cast<Enemy>(global::get_entity_with_id(e->id));
      out_file << "-1\n";
      for (auto &p : ehandle->path)
      {
        out_file << left << setw(4) << p.x << " " << p.y << "|";
      }
      out_file << endl;
    }
  }
  // print bouncy wall coords
  for (auto &w : bouncy_wall_vecs)
  {
    auto width = (float)global::winWidth;
    auto height = (float)global::winHeight;
    out_file << w.first.x / width << " " << w.first.y / height;
    out_file << " " << w.second.x / width << " " << w.second.y / height;
    out_file << "B" << endl;
  }
  // print absorby wall coords
  for (auto &w : absorby_wall_vecs)
  {
    auto width = (float)global::winWidth;
    auto height = (float)global::winHeight;
    out_file << w.first.x / width << " " << w.first.y / height;
    out_file << " " << w.second.x / width << " " << w.second.y / height;
    out_file << "A" << endl;
  }
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
  cout << right << setw(15) << "Block width: " << global::blockWidth << endl;
  cout << left << setw(15) << "Grid spacing: " << grid_spacing << sep;

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
  auto const level_data_regex = regex("^level(\\d+)_data(\\.+)txt$", regex::ECMAScript);
  // loop over files in the assets directory
  auto file_num = 0;
  for (const auto &entry : fs::directory_iterator(level_path))
  {
    // verify filename matches pattern
    string filename = entry.path().filename().string();
    bool const path_matches_regex = regex_search(filename, level_data_regex);
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

    // draw grid
    sf::CircleShape cgrid(2.f);
    cgrid.setFillColor(sf::Color(50, 50, 50, 255));
    for (int i = grid_spacing; i < global::winWidth; i += grid_spacing)
    {
      for (int j = grid_spacing; j < global::winHeight; j += grid_spacing)
      {
        cgrid.setPosition(Vec2(i, j));
        win->draw(cgrid);
      }
    }
    for (auto &e : global::entity)
    {
      bool change_fill_color = (e->id == curr_selected_enemy) ? true : false;
      for (auto f : e->frags)
      {
        // record old vals
        // change vals so drawn differently this frame
        if (change_fill_color)
        {
          f.setOutlineColor(sf::Color::Red);
          f.setOutlineThickness(3.f);
        }
        else
        {
          f.setOutlineThickness(0.f);
        }
        win->draw(f);
      }
      // draw enemy paths
      if (e->type == EType::Enemy)
      {
        shared_ptr<Enemy> ehandle = dynamic_pointer_cast<Enemy>(global::get_entity_with_id(e->id));
        sf::CircleShape cs(global::bW * 3, 12);
        cs.setFillColor(sf::Color(255, 178, 102, 160));
        sf::Text txt("", global::font, 14);
        txt.setFillColor(sf::Color::Black);
        unsigned path_count = 1;
        for (Vec2 pt : ehandle->path)
        {
          cs.setPosition(Vec2(pt.x * global::winWidth, pt.y * global::winHeight));
          txt.setString(to_string(path_count));
          txt.setPosition(cs.getPosition() + Vec2(5, 5));
          path_count++;
          win->draw(cs);
          win->draw(txt);
        }
      }
    }
    // set window current title: Level editor
    stringstream ss;
    ss << "current file: " << left << setw(20) << cb_level.front().filename().string()
       << " current object: " << left << setw(20) << magic_enum::enum_name(cb_obj.front());
    win->setTitle(ss.str());
    // draw mouse position
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*win);
    Vec2 rounded_mouse_pos = round_vec_to_grid(mouse_pos);
    mouse_pos = sf::Vector2i(rounded_mouse_pos.x, rounded_mouse_pos.y);
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
    bool addPathPointToCurrSelectedEnemy{false};
    // Set flags to do Editor Actions
    if (event.type == sf::Event::KeyPressed)
    {
      if (event.key.code == sf::Keyboard::Space && time_accum > KEY_EVENT_GAP_MILLI)
      {
        // Place an object at the mouse position
        okayToPlaceObject = true;
      }
      else if (event.key.code == sf::Keyboard::Down && time_accum > KEY_EVENT_GAP_MILLI)
      {
        // Switch to next object
        okayToMoveToNextObject = true;
      }
      else if (event.key.code == sf::Keyboard::A && time_accum > KEY_EVENT_GAP_MILLI)
      {
        // Add a path point to the selected enemy
        if (curr_selected_enemy)
        {
          shared_ptr<Enemy> ehandle =
              dynamic_pointer_cast<Enemy>(global::get_entity_with_id(*curr_selected_enemy));
          ehandle->path.push_back(Vec2((float)rounded_mouse_pos.x / global::winWidth,
                                       (float)rounded_mouse_pos.y / global::winHeight));
          cout << "added " << ehandle->path[ehandle->path.size() - 1].x << " , "
               << ehandle->path[ehandle->path.size() - 1].y << endl;
        }
      }
      else if (event.key.code == sf::Keyboard::P && time_accum > KEY_EVENT_GAP_MILLI)
      {
        // Select or unselect nearby enemy
        if (auto nearby_enemy_id = check_for_nearby_enemy(mouse_pos); nearby_enemy_id)
        {
          enemySelectedForPathAddition = !enemySelectedForPathAddition;
          curr_selected_enemy = enemySelectedForPathAddition ? nearby_enemy_id : nullopt;
        }
      }
      else if (event.key.code == sf::Keyboard::S && time_accum > KEY_EVENT_GAP_MILLI)
      {
        // open file for writing
        std::ofstream out_file(new_file, ios::out);
        // write level data to file
        save_level_to_file(out_file);
        // exit program
        return 0;
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
        global::entity.push_back(make_shared<Enemy>(1, rounded_mouse_pos));
        break;
      case EType::BouncyWall:
        if (!wall_vecs[0])
        {
          wall_vecs[0] = rounded_mouse_pos;
        }
        else if (!wall_vecs[1])
        {
          wall_vecs[1] = rounded_mouse_pos;
        }
        if (wall_vecs[0] && wall_vecs[1])
        {
          builder::build_long_wall(*wall_vecs[0], *wall_vecs[1]);
          // save to bouncy_wall_vecs so editor can print out
          bouncy_wall_vecs.push_back(make_pair(*wall_vecs[0], *wall_vecs[1])); // end of wall
          // reset wall vecs
          wall_vecs = {nullopt, nullopt};
        }
        break;
      case EType::AbsorbyWall:
        if (!wall_vecs[0])
        {
          wall_vecs[0] = rounded_mouse_pos;
        }
        else if (!wall_vecs[1])
        {
          wall_vecs[1] = rounded_mouse_pos;
        }
        if (wall_vecs[0] && wall_vecs[1])
        {
          builder::build_long_absorby_wall(*wall_vecs[0], *wall_vecs[1]);
          // save to absorby_wall_vecs so editor can print out
          absorby_wall_vecs.push_back(make_pair(*wall_vecs[0], *wall_vecs[1])); // end of wall
          // reset wall vecs
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

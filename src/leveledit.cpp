#include "global.h"
/* */
#include "builder.h"
#include "entity.h"
#include "magic_enum.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <boost/circular_buffer.hpp>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;
using Vec2 = sf::Vector2f;
// Support objects to place
// BouncyWall, AbsorbyWall, Enemy1, Enemy2, Enemy3, Enemy4

string sep(3, '\n');

// return a vector given in percent as a vector in pixels
Vec2 perc_to_pix(float x, float y)
{
  assert(x >= 0 && x <= 100);
  assert(y >= 0 && y <= 100);
  return Vec2((x / 100.f) * global::winWidth, (y / 100.f) * global::winHeight);
}

int main()
{

  // print out level editor information
  cout << left << setw(15) << "Window dimensions: " << global::winWidth << " x "
       << global::winHeight;
  cout << right << setw(15) << "blockWidth: " << global::blockWidth << sep;

  // cb_obj circular buffer and print objects out
  // current object is cb_obj.front()
  boost::circular_buffer<EType> cb_obj(6);
  EType curr_object;
  constexpr auto all_objects = magic_enum::enum_values<EType>();
  cout << "Objects available to editor: " << endl;
  for (auto &i : all_objects)
  {
    cb_obj.push_back(i);
    cout << "\t" << static_cast<unsigned>(i) << ": " << magic_enum::enum_name(i) << endl;
  }
  cout << sep;
  curr_object = cb_obj.front();

  // cb_obj.rotate(begin(cb_obj) + 1);

  // build level_file circular buffer and print files out
  // current file is level_list.front()
  boost::circular_buffer<fs::path> cb_level;
  fs::path curr_level{};
  cout << "Level files available to edit:" << endl;
  string level_path{"../assets/"};
  auto const level_data_regex = regex("level(\\d+)_data(\\.+)txt", regex::ECMAScript);
  // loop over files in the assets directory
  for (const auto &entry : fs::directory_iterator(level_path))
  {
    string abs_path = entry.path().string();
    // verify filename matches pattern
    bool const path_matches_regex = regex_search(abs_path, level_data_regex);
    if (path_matches_regex)
    {
      cout << "\t" << entry.path() << endl;
      cb_level.push_back(entry.path());
    }
  }
  cout << sep;
  //curr_level = cb_level.front().path(); // Error

  // create window
  auto win = global::create_window();
  win->setVerticalSyncEnabled(true);
  win->setTitle("Level editor");
  win->display();

  sf::Event event;
  builder::build_long_wall(perc_to_pix(1, 0), perc_to_pix(1, 99));
  builder::build_long_wall(perc_to_pix(99, 0), perc_to_pix(99, 99));
  // main loop
  while (true)
  {
    win->clear(global::clearscreen_color);
    if (global::check_for_window_close(*win, event))
      break;

    for (auto e : global::entity)
    {
      for (auto f : e->frags)
      {
        win->draw(f);
      }
    }
    win->display();
  }

  return 0;
}

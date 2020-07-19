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
#include <boost/circular_buffer.hpp>
#include <boost/serialization/serialization.hpp>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <regex>

using namespace std;
namespace fs = std::filesystem;
using Vec2 = sf::Vector2f;

// global variables
const unsigned MAX_OBJ_COUNT = 10;
const unsigned MAX_LEVEL_COUNT = 10;
const string TITLE_STRING = "Level Editor";
//

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
  builder::build_long_wall(perc_to_pix(1, 0), perc_to_pix(1, 99));
  builder::build_long_wall(perc_to_pix(99, 0), perc_to_pix(99, 99));

  // main loop
  sf::Event event;
  while (true)
  {
    win->clear(global::clearscreen_color);
    if (global::check_for_window_close(*win, event))
      break;
    // get the mouse position relative to the window

    for (auto e : global::entity)
    {
      for (auto f : e->frags)
      {
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
    // handle keyboard events
    if (event.type == sf::Event::KeyPressed)
    {
      if (event.key.code == sf::Keyboard::Space)
      {
        auto curr_obj = cb_obj.front();
        cout << "placing object type: " << setw(10) << magic_enum::enum_name(cb_obj.front())
             << endl;
        switch (curr_obj)
        {
        case EType::Enemy:
            global::entity.push_back(make_shared<Enemy>(1));
            break;
           case EType::BouncyWall:
            break;
           default:
            cout << "Error - unhandled type placed" << endl;
            break;
        };
      }
    }
    win->display();
  }

  return 0;
}

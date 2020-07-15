#include "global.h"
/* */
#include "builder.h"
#include "entity.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <boost/circular_buffer.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

using namespace std;
using namespace global;
namespace fs = std::filesystem;

// Support objects to place
// BouncyWall, AbsorbyWall, Enemy1, Enemy2, Enemy3, Enemy4

string sep(3, '\n');

int main()
{
  // initialize the circular buffer
  boost::circular_buffer<string> obj_list(6);
  obj_list.push_back("BouncyWall");
  obj_list.push_back("AbsorbyWall");
  vector<int> enemy_nums{1, 2, 3, 4};
  for_each(begin(enemy_nums), end(enemy_nums),
           [&](const auto &num) { obj_list.push_back("Enemy " + to_string(num)); });

  // print out level editor information
  cout << left << setw(15) << "Window dimensions: " << global::winWidth << " x "
       << global::winHeight;
  cout << right << setw(15) << "blockWidth: " << global::blockWidth << sep;

  cout << "Object list of editor: " << endl;
  for (auto i = 0; i < obj_list.size(); i++)
  {
    auto curr_item = obj_list.front();
    obj_list.rotate(begin(obj_list) + 1);
    cout << "object " << i << ": " << curr_item << endl;
  }
  cout << sep;
  cout << "Level files available to edit:" << endl;

  // print out level files
  string level_path{"../assets/"};
  auto const level_data_regex = regex("level(\\d+)_data(\\.+)txt", regex::ECMAScript);
  // loop over files in the assets directory
  for (const auto &entry : fs::directory_iterator(level_path))
  {
    string abs_path = entry.path().string();
    bool const path_matches_regex = regex_search(abs_path, level_data_regex);
    // verify its a level data file 'level#_data.txt'
    if (path_matches_regex)
    {
      cout << entry.path() << endl;
      cout("\t{}", entry.path());
    }
  }
  cout << sep;

  // create window
  auto win = global::create_window();
  win->setVerticalSyncEnabled(true);
  win->setTitle("Level editor");
  win->display();
  
  sf::Event event;

  EType objType = EType::BouncyWall;
    cout << objType << endl;
  // main loop
  while (true)
  {
    if (global::check_for_window_close(*win, event))
      break;
  }

  return 0;
}

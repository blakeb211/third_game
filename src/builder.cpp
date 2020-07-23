#include "builder.h"

#include <SFML/Graphics.hpp>

#include "entity.h"
#include "global.h"
#include <filesystem>

using namespace std;
using namespace sf;
using namespace global;
using Vec2 = Vector2<float>;


void builder::create_entity_varray_from_frags(IEntity &e) {
  auto sz = e.frags.size();
  auto col = e.frags[0].getFillColor();
 
  for (auto i = 0; i < sz; i++) {
    auto pos = e.frags[i].getPosition();
    //add 4 vertices for each frag
    e.varray.append(sf::Vertex(pos, col));  
    e.varray.append(sf::Vertex(pos + x_offset, col));
    e.varray.append(sf::Vertex(pos + x_offset + y_offset, col));
    e.varray.append(sf::Vertex(pos + y_offset, col));
  }
}

void builder::build_long_wall(Vec2 wall_start, Vec2 wall_end)
{
  // start of build a multi-segment wall
  const auto unit_vec = make_unit_vec(wall_end - wall_start);

  while (calc_dist(wall_start, wall_end) >= 2.8f * bW)
  {
    // place voxel
    auto tmp_end = wall_start + unit_vec * 5.f * static_cast<float>(bW);
    entity.push_back(make_shared<BouncyWall>(wall_start, tmp_end));
    wall_start = tmp_end + unit_vec * 0.2f * static_cast<float>(bW);
  } // end of build a multi-segment wall
}

void builder::build_long_absorby_wall(Vec2 wall_start, Vec2 wall_end)
{
  // start of build a multi-segment wall
  const auto unit_vec = make_unit_vec(wall_end - wall_start);

  while (calc_dist(wall_start, wall_end) >= 2.8f * bW)
  {
    // place voxel
    auto tmp_end = wall_start + unit_vec * 5.f * static_cast<float>(bW);
    entity.push_back(make_shared<AbsorbyWall>(wall_start, tmp_end));
    wall_start = tmp_end + unit_vec * 0.2f * static_cast<float>(bW);
  } // end of build a multi-segment wall
}

void builder::set_frag_health(IEntity &e, optional<unsigned int> num)
{
  for_each(begin(e.frags), end(e.frags), [&num](auto &f) { f.health = num; });
}

void builder::add_wall_frags(IEntity &e, Vec2 start, Vec2 end, Color c)
{
  // create a vector from start to end
  Vec2 unitVec = make_unit_vec(end - start);
  // march from start to end placing voxels
  while (calc_dist(start, end) > 0.55f * bW)
  {
    // place voxel
    e.frags.emplace_back(start.x, start.y, c);
    // consider making 1.2f below be depending on the slope of the
    // start and end vectors
    start += unitVec * 1.f * (float)(bW);
  }
  create_entity_varray_from_frags(e);
}

bool builder::build_level(unsigned int &levelId)
{
  // open the file levelN_data.txt
  cout << "opening level loading input file" << endl;
  string in_file_string = "../assets/level" + to_string(levelId) + "_data.txt";
  if (!std::filesystem::exists(in_file_string)) {
    cerr << "File: " << in_file_string << " does not exist" << endl;
    return false;
  }
  ifstream in_file(in_file_string, ios::in);
  unsigned int enemyCount = 0;
  unsigned int pathCount = 0;
  unsigned int wallCount = 0;
  queue<unsigned int> enemyIds = {};
  entity.push_back(make_shared<Player>());
  cout << "player entity id" << entity.back()->id << endl;
  while (in_file.good())
  {
    string line;
    getline(in_file, line);
    // check if its a level layout line
    if (line.find('-') != string::npos)
    {
      for (int i = 0; i < line.size(); i++)
        if (isdigit(line[i]))
        {
          enemyCount++;
          // convert char number to int number
          int digit = line[i] - '0';
          switch (digit)
          {
          case 1:
            entity.push_back(make_shared<Enemy>(0));
            break;
          case 2:
            entity.push_back(make_shared<Enemy>(1));
            break;
          case 3:
            entity.push_back(make_shared<Enemy>(2));
            break;
          case 4:
            entity.push_back(make_shared<Enemy>(3));
            break;
          default:
            throw exception("switch in builder::build_level failed");
            break;
          };
          // add the newly created enemy's entity id to the enemyIds vector
          enemyIds.push((*(--end(entity)))->id);
        }
    }
    // load the paths
    // check if its a path line
    if (line.find('|') != string::npos)
    {
      pathCount++;
      unsigned int currEntityId = enemyIds.front(); // pop from the front
      enemyIds.pop();
      // get a pointer to the enemy by downcasting IEntity
      auto e_ptr = dynamic_pointer_cast<Enemy>(get_entity_with_id(currEntityId));

      // read in the path
      istringstream ss(line);
      // read in the whole line of float float|
      while (ss.good())
      {
        float x;
        float y;
        char c;
        ss >> x;
        if (ss.bad())
          throw exception("sstring stream accessed when bad in builder::build_level");
        // level editor coords are in percentages of winHeight and winWidth
        ss >> y;
        ss >> c;
        assert(x * winWidth <= winWidth);
        assert(y * winHeight <= winHeight);
        e_ptr->path.push_back(Vec2(x * winWidth, y * winHeight));
      }
      // set enemies starting position to the first point on its path
      // move entity to its first path point
      global::move_entity(*e_ptr, e_ptr->path[0]);
    }
    // load the walls
    // check if its a wall line: a B means a bouncy wall, a D means a
    // destructible wall
    if (line.find('B') != string::npos || line.find('A') != string::npos)
    {
      wallCount++;
      // read in the path
      istringstream ss(line);
      // read in the whole line of float float|
      while (ss.good())
      {
        float x_start, y_start, x_end, y_end;
        char c;
        ss >> x_start;
        ss >> y_start;
        ss >> x_end;
        ss >> y_end;
        ss >> c; // read in type of wall, 'B' or 'A'
        assert(c == 'B' || c == 'A');
        // Convert from percentages of width and height to pixels 
        x_start *= winWidth;
        x_end *= winWidth;
        y_start *= winHeight;
        y_end *= winHeight;
        if (c == 'B')
        {
          build_long_wall(Vec2(x_start,y_start), Vec2(x_end,y_end));
          // break out of the stringstream reading loop
          break;
        }
        if (c == 'A')
        {
          build_long_absorby_wall(Vec2(x_start,y_start), Vec2(x_end,y_end));
          // break out of the stringstream reading loop
          break;
        }
      }
    }
  }
  cout << "closing level loading input file" << endl;
  in_file.close();
  assert(enemyCount == pathCount);
  return true;
}

void builder::add_player_frags(IEntity &e)
{
  e.frags.emplace_back(0.f, 0.f, Color::White);
  e.frags.emplace_back(0.f + 1.f * bW, 0.f, Color::White);
  e.frags.emplace_back(0.f + 2.f * bW, 0.f, Color::White);
  e.frags.emplace_back(0.f + 1.f * bW, 0.f - 1.f * bW, Color::Cyan);
  create_entity_varray_from_frags(e);
}

void builder::add_bullet1_frags(IEntity &e)
{
  e.frags.emplace_back(1.f * bW, 0.f * bW, Color(153, 76, 0, 255));
  e.frags.emplace_back(0.f * bW, 1.f * bW, Color(153, 76, 0, 255));
  e.frags.emplace_back(1.f * bW, 1.f * bW, Color(153, 76, 0, 255));
  e.frags.emplace_back(2.f * bW, 1.f * bW, Color(153, 76, 0, 255));
  e.frags.emplace_back(1.f * bW, 2.f * bW, Color(153, 76, 0, 255));
  create_entity_varray_from_frags(e);
}

void builder::add_bullet2_frags(IEntity &e)
{
  e.frags.emplace_back(2.f * bW, 0.f * bW, Color(255, 165, 0, 255));
  e.frags.emplace_back(1.f * bW, 1.f * bW, Color(255, 165, 0, 255));
  e.frags.emplace_back(2.f * bW, 1.f * bW, Color(255, 165, 0, 255));
  e.frags.emplace_back(3.f * bW, 1.f * bW, Color(255, 165, 0, 255));
  e.frags.emplace_back(1.f * bW, 2.f * bW, Color(255, 165, 0, 255));
  e.frags.emplace_back(3.f * bW, 2.f * bW, Color(255, 165, 0, 255));
  create_entity_varray_from_frags(e);
}

void builder::add_enemy1_frags(IEntity &e)
{
  e.frags.emplace_back(9.f * bW, 0.f * bW, Color::Cyan);
  e.frags.emplace_back(10.f * bW, 0.f * bW, Color::Cyan);
  e.frags.emplace_back(11.f * bW, 0.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 0.f * bW, Color::Cyan);
  e.frags.emplace_back(7.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(8.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(9.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(10.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(11.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(13.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(14.f * bW, 1.f * bW, Color::Cyan);
  e.frags.emplace_back(6.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(7.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(8.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(9.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(10.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(11.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(13.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(14.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(15.f * bW, 2.f * bW, Color::Cyan);
  e.frags.emplace_back(5.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(6.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(9.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(10.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(11.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(15.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(16.f * bW, 3.f * bW, Color::Cyan);
  e.frags.emplace_back(5.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(6.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(7.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(8.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(9.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(10.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(11.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(13.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(14.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(15.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(16.f * bW, 4.f * bW, Color::Cyan);
  e.frags.emplace_back(8.f * bW, 5.f * bW, Color::Cyan);
  e.frags.emplace_back(13.f * bW, 5.f * bW, Color::Cyan);
  e.frags.emplace_back(7.f * bW, 6.f * bW, Color::Cyan);
  e.frags.emplace_back(9.f * bW, 6.f * bW, Color::Cyan);
  e.frags.emplace_back(10.f * bW, 6.f * bW, Color::Cyan);
  e.frags.emplace_back(11.f * bW, 6.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 6.f * bW, Color::Cyan);
  e.frags.emplace_back(14.f * bW, 6.f * bW, Color::Cyan);
  e.frags.emplace_back(6.f * bW, 7.f * bW, Color::Cyan);
  e.frags.emplace_back(9.f * bW, 7.f * bW, Color::Cyan);
  e.frags.emplace_back(12.f * bW, 7.f * bW, Color::Cyan);
  e.frags.emplace_back(15.f * bW, 7.f * bW, Color::Cyan);
  create_entity_varray_from_frags(e);
}

void builder::add_enemy2_frags(IEntity &e)
{
  e.frags.emplace_back(3.f * bW, 0.f * bW, Color::Magenta);
  e.frags.emplace_back(4.f * bW, 0.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 0.f * bW, Color::Magenta);
  e.frags.emplace_back(18.f * bW, 0.f * bW, Color::Magenta);
  e.frags.emplace_back(6.f * bW, 1.f * bW, Color::Magenta);
  e.frags.emplace_back(16.f * bW, 1.f * bW, Color::Magenta);
  e.frags.emplace_back(5.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(6.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(7.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(8.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(9.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(10.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(11.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(12.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(13.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(14.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(15.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(16.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 2.f * bW, Color::Magenta);
  e.frags.emplace_back(4.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(5.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(6.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(8.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(9.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(10.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(11.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(12.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(13.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(14.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(16.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(18.f * bW, 3.f * bW, Color::Magenta);
  e.frags.emplace_back(2.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(3.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(4.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(5.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(6.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(7.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(8.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(9.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(10.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(11.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(12.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(13.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(14.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(15.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(16.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(18.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(19.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(20.f * bW, 4.f * bW, Color::Magenta);
  e.frags.emplace_back(5.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(6.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(7.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(8.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(9.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(10.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(11.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(12.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(13.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(14.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(15.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(16.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 5.f * bW, Color::Magenta);
  e.frags.emplace_back(5.f * bW, 6.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 6.f * bW, Color::Magenta);
  e.frags.emplace_back(6.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(7.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(8.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(9.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(14.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(15.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(16.f * bW, 7.f * bW, Color::Magenta);
  e.frags.emplace_back(17.f * bW, 7.f * bW, Color::Magenta);
  create_entity_varray_from_frags(e);
}

void builder::add_enemy3_frags(IEntity &e)
{
  e.frags.emplace_back(9.f * bW, 0.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(10.f * bW, 0.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(11.f * bW, 0.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(12.f * bW, 0.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(13.f * bW, 0.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(5.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(6.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(7.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(8.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(9.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(10.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(11.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(12.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(13.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(14.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(15.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(16.f * bW, 1.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(4.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(5.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(6.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(7.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(8.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(9.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(10.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(11.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(12.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(13.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(14.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(15.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(16.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(17.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(18.f * bW, 2.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(4.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(5.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(6.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(9.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(10.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(11.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(12.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(13.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(16.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(17.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(18.f * bW, 3.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(4.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(5.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(6.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(7.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(8.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(9.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(10.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(11.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(12.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(13.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(14.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(15.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(16.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(17.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(18.f * bW, 4.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(6.f * bW, 5.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(7.f * bW, 5.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(8.f * bW, 5.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(14.f * bW, 5.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(15.f * bW, 5.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(16.f * bW, 5.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(5.f * bW, 6.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(6.f * bW, 6.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(7.f * bW, 6.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(15.f * bW, 6.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(16.f * bW, 6.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(17.f * bW, 6.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(3.f * bW, 7.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(4.f * bW, 7.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(5.f * bW, 7.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(17.f * bW, 7.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(18.f * bW, 7.f * bW, Color::Red + Color::Yellow);
  e.frags.emplace_back(19.f * bW, 7.f * bW, Color::Red + Color::Yellow);
  create_entity_varray_from_frags(e);
}

void builder::add_enemy4_frags(IEntity &e)
{
  e.frags.emplace_back(9.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(10.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(11.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(12.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(13.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(14.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(15.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(16.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(17.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(18.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(19.f * bW, 0.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(6.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(7.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(8.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(9.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(10.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(11.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(12.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(13.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(14.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(15.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(16.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(17.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(18.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(19.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(20.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(21.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(22.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(23.f * bW, 1.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(4.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(5.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(6.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(7.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(8.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(9.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(10.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(11.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(12.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(13.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(14.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(15.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(16.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(17.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(18.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(19.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(20.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(21.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(22.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(23.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(24.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(25.f * bW, 2.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(3.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(4.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(5.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(6.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(8.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(9.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(10.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(11.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(13.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(14.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(15.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(16.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(18.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(19.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(20.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(21.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(23.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(24.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(25.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(26.f * bW, 3.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(0.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(1.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(2.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(3.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(4.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(5.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(6.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(7.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(8.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(9.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(10.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(11.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(12.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(13.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(14.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(15.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(16.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(17.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(18.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(19.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(20.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(21.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(22.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(23.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(24.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(25.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(26.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(27.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(28.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(29.f * bW, 4.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(5.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(6.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(7.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(8.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(9.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(13.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(14.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(15.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(16.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(20.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(21.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(22.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(23.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(24.f * bW, 5.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(7.f * bW, 6.f * bW, Color(255, 140, 0, 245));
  e.frags.emplace_back(22.f * bW, 6.f * bW, Color(255, 140, 0, 245));
  create_entity_varray_from_frags(e);
}


#include "global.h"

#include "stdlibs.h"
//#define COLL_LOGGING

using namespace std;
using namespace sf;
typedef chrono::high_resolution_clock high_res_clock;

inline std::ostream &global::operator<<(std::ostream &Str, EType V)
{
  switch (V)
  {
  case EType::Bullet:
    return Str << "bullet";
  case EType::BouncyWall:
    return Str << "bouncyWall";
  case EType::Player:
    return Str << "player";
  case EType::Enemy:
    return Str << "enemy";
  default:
    return Str << (int)V;
  };
}

Vec2 global::make_unit_vec(Vec2 v)
{
  auto len = hypot(v.x, v.y);
  return v / len;
}

int global::rand_between(int low, int high)
{
  return low + rand_engine() % (high - low);
}

void global::set_frag_health(IEntity &e, optional<unsigned int> h)
{
  for (auto &f : e.frags)
  {
    *(f.health) = *h;
  }
}
void global::erase_frag_with_id(IEntity &e, size_t frag_id)
{
  for (auto it = e.frags.begin(); it != e.frags.end(); it++)
  {
    if (it->id == frag_id)
    {
      e.frags.erase(it);
      return;
    }
  }
}

unique_ptr<Frag> global::get_frag_with_id(IEntity &e, size_t frag_id)
{
  for (auto &f : e.frags)
  {
    if (f.id == frag_id)
    {
      return make_unique<Frag>(f);
    }
  }
  return nullptr;
}

// calculate entity center if it hasn't been calculated yet, otherwise return it
Vec2 global::get_center(IEntity &e)
{
  if (e.center.x > 0 || e.center.y > 0)
  {
    return e.center;
  }
  else
  {
    e.center = e.hitbox.getPosition() + Vec2(e.hitbox.getSize().x / 2, e.hitbox.getSize().y / 2);
  }
  return e.center;
}
// collide enemy hitboxes to keep the out of each others hitbox
void global::collide_enemy_hitboxes(IEntity &ei_ref, IEntity &ej_ref)
{
  auto ei_cent = get_center(ei_ref);
  auto ej_cent = get_center(ej_ref);
  auto bounce_vec = make_unit_vec(ei_cent - ej_cent);
  // apply bounce forces
  ei_ref.dvel += bounce_vec * 0.8f;
  ej_ref.dvel += bounce_vec * -0.8f;
}
// check free frags for collisions with entities, but not other free frags
void global::check_free_frags_for_collisions()
{
  int ffSize = free_frags.size();
  int entSize = entity.size();
  bool collision_flag = false;
  for (int fi = 0; fi < ffSize; fi++)
  {
    for (int ei = 0; ei < entSize; ei++)
    {
      // check if hitboxes collide
      if (free_frags[fi].getGlobalBounds().intersects(entity[ei]->hitbox.getGlobalBounds()))
      {
        auto &fi_ref = free_frags[fi];
        auto &ei_ref = *entity[ei];
        auto ev_size = ei_ref.frags.size();
        // check which pixels the frag hit
        Vec2 ff_pos = free_frags[fi].getPosition();
        for (int vi = 0; vi < ev_size; vi++)
        {
          if (fi_ref.getGlobalBounds().intersects(ei_ref.frags[vi].getGlobalBounds()))
          {
            collision_flag = true;
            Vec2 ev_pos = ei_ref.frags[vi].getPosition();
            fi_ref.collide_with(ei_ref, ev_pos);
            ei_ref.collide_with_free_frag(vi, fi_ref);
          }
          if (collision_flag)
          {
            // break here if want frags to collide only once per frame
            break;
          }
        }
      }
      // free frag can only hit one entity voxel at a time
      if (collision_flag)
      {
#ifdef COLL_LOGGING
        cout << "Collision: "
             << "free frag and ";
        cout << "(" << entity[ei]->type << ")" << endl;
#endif
        // break here if want frags to collide only once per frame
        break;
      }
    }
  }
}

void global::process_set_of_freed_frags()
{
  // move frags to the free frag vector and remove it from the
  // bullet entity vector
  // The pair in frags_to_free is entity_id, frag idx.

  size_t frag_id;
  shared_ptr<IEntity> e_ptr;
  unique_ptr<Frag> f_ptr;

  try
  {
    for (const auto &eid_fid : frags_to_free)
    {
      e_ptr = get_entity_with_id(eid_fid.first);
      if (e_ptr == nullptr)
        continue;
      frag_id = eid_fid.second;
      f_ptr = get_frag_with_id(*e_ptr, frag_id);
      if (f_ptr != nullptr)
      {
        free_frags.push_back(*f_ptr);
        erase_frag_with_id(*e_ptr, frag_id);
      }
    }
  }
  catch (exception &e)
  {
    cout << "exception in for loop of process_set_of_freed_frags" << endl;
  }
  frags_to_free.clear();
}

void global::erase_freed_frags()
{
  Vec2 pos;
  for (auto it = begin(free_frags); it != end(free_frags); it++)
  {
    pos = it->getPosition();
    if (*(it->health) < 0 || pos.x < 0 || pos.x > winWidth || pos.y < 0 || pos.y > winHeight)
    {
      free_frags.erase(it);
      break;
    }
    // set free_frag limit to see if it helps draw timings
    unsigned int ffSize = free_frags.size();
    const uint32_t ffCutoff = 100u;
    if (ffSize > ffCutoff)
    {
      unsigned int sizeDiff = ffSize - ffCutoff;
      swap_ranges(begin(free_frags) + ffCutoff, end(free_frags), begin(free_frags));
      free_frags.erase(begin(free_frags) + ffCutoff, end(free_frags));
    }
  }
}

// check entities for collisions
void global::check_entities_for_collisions()
{
  auto entSize = entity.size();
  for (int i = 0; i < entSize - 1; i++)
  {
    for (int j = i + 1; j < entSize; j++)
    {
      // check if hitboxes collide
      if (entity[i]->hitbox.getGlobalBounds().intersects(entity[j]->hitbox.getGlobalBounds()))
      {
        // hitboxes collided
        auto &ei_ref = *entity[i];
        auto &ej_ref = *entity[j];
        if (ei_ref.type == EType::Enemy && ej_ref.type == EType::Enemy)
        {
          // enemies collide one another with their hitboxes not their voxels
          // calc hitbox centers to get rough approx of collision vector
          collide_enemy_hitboxes(ei_ref, ej_ref);
          return;
        }
        auto fi_size = ei_ref.frags.size();
        auto fj_size = ej_ref.frags.size();
        bool collision_flag = false;
        Vec2 ii_pos, jj_pos;
        for (int ii = 0; ii < fi_size; ii++)
        {
          for (int jj = 0; jj < fj_size; jj++)
          {
            if (ei_ref.frags[ii].getGlobalBounds().intersects(ej_ref.frags[jj].getGlobalBounds()))
            {
              ii_pos = ei_ref.frags[ii].getPosition();
              jj_pos = ej_ref.frags[jj].getPosition();
              ei_ref.collide_with(ej_ref, ii, move(jj_pos), ej_ref.frags[jj].getFillColor());
              ej_ref.collide_with(ei_ref, jj, move(ii_pos), ei_ref.frags[ii].getFillColor());
              collision_flag = true;
            }
          }
          if (collision_flag)
          {
#ifdef COLL_LOGGING
            cout << "Collision: " << i << "(" << entity[i]->type << ") and " << j;
            cout << "(" << entity[j]->type << ")" << endl;
#endif
          }
        }
      }
    }
  }
}

void global::remove_dead_entities()
{
  // used for
  //    a) removing entities that went off screen
  //    b) marking entities as dead if they are below their health cutoff
  //    c) exploding entities that fall below their healthCutoff

  for (auto &e : entity)
  {
    // copy its frags to free_frags
    // set the frag velociy
    // mark as isDead
    if (e->frags.size() < e->healthCutoff)
    {
      for (auto &f : e->frags)
      {
        // modulate exploded enemy frag health
        (*f.health) = 1;
        f.vel = Vec2(rand_between(-3, 6), rand_between(-3, 6));
        // explode entities into free frags
        free_frags.push_back(move(f));
      }
      e->isDead = true;
      if (e->type == EType::Enemy) {global::score += 20; }
    }
  }
  // erase all entities flagged as isDead, which can happen by
  // moving off screen or if they dropped below their health cutoff
  // and got exploded
  entity.erase(remove_if(begin(entity), end(entity),
                         [](const shared_ptr<IEntity> &e) { return e->isDead == true; }),
               end(entity));
}

// process current frags and build hitbox from it
void global::build_hitbox(IEntity &e)
{
  vector<float> x, y;
  int frag_count = e.frags.size();
  for (int i = 0; i < frag_count; i++)
  {
    auto frag_pos = e.frags[i].getPosition();
    x.push_back(frag_pos.x);
    y.push_back(frag_pos.y);
  }
  // these return a pair of iterators
  auto xmm = minmax_element(x.begin(), x.end());
  auto ymm = minmax_element(y.begin(), y.end());
  auto width = (*xmm.second - *xmm.first);
  auto height = (*ymm.second - *ymm.first);
  float spacer = global::blockWidth * 0.25f;
  e.hitbox.setPosition(Vec2(*xmm.first - spacer, *ymm.first - spacer));
  e.hitbox.setSize(Vec2(width + spacer * 2.f, height + spacer * 2.f));
  // drawing options for hitbox
  e.hitbox.setFillColor(Color(0, 0, 0, 0));
  e.hitbox.setOutlineColor(Color(51, 153, 255, 240));
  e.hitbox.setOutlineThickness(1.f);
}

// rotate entity's frags by offset
void global::rotate_entity(IEntity &e, const float ang_offset)
{
}

// move entity frags and its hitbox
void global::move_entity(IEntity &e, const Vec2 offset)
{
  for_each(begin(e.frags), end(e.frags), [&offset](Frag &f) { f.move(offset); });
  e.hitbox.move(offset);
  e.center += offset;
}

// free function to get a new entity id
unsigned int global::get_new_entity_id()
{
  return entityCounter++;
}

// free function to create a window
unique_ptr<RenderWindow> global::create_window()
{
  const ContextSettings cs(0, 0, 2, 4, 4, ContextSettings::Default, true);
  auto window = make_unique<RenderWindow>(VideoMode(global::winWidth, global::winHeight),
                                          "Iteration 3", Style::Close, cs);
  window->setVerticalSyncEnabled(false); // prevent screen tearing
  return move(window);
}

// free funnction to check for window close
bool global::check_for_window_close(RenderWindow &window, Event &event)
{
  while (window.pollEvent(event))
  {
    switch (event.type)
    {
    case Event::Closed:
      cerr << "User exited window" << endl;
      window.close();
      return true;
    default:
      return false;
    };
  }
  return false;
}

// get user input
// exit main game loop by returning true
bool global::handle_keyboard_input(float &timer, const float maxTime, RenderWindow &window)
{
  if (state == GAME_STATE::Game)
  {
    if (timer < maxTime)
      return false; // don't exit main game loop
    timer = 0.f;
    if (Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
      // escape key activates the Menu
      state = GAME_STATE::Menu;
      return false;
    }
    if (global::player_ptr == nullptr)
    {
      global::player_ptr = get_entity_with_id(0);
    }
    if (Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
      if (player_ptr && player_ptr->type == EType::Player)
      {
        if (player_ptr->center.x < 30.f)
          return false;
        auto &dvel_ref = player_ptr->dvel;
        dvel_ref += (abs(dvel_ref.x) < 8.9f) ? Vec2(-4.1f, 0.f) : Vec2(0.f, 0.f);
      }
    }
    if (Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
      if (player_ptr && player_ptr->type == EType::Player)
      {
        if (player_ptr->center.x > global::winWidth - 30.f)
          return false;
        auto &dvel_ref = player_ptr->dvel;
        dvel_ref += (abs(dvel_ref.x) < 8.9f) ? Vec2(+4.1f, 0.f) : Vec2(0.f, 0.f);
      }
    }
    if (Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
      if (player_ptr && player_ptr->type == EType::Player)
      {
        auto player_ptr2 = dynamic_pointer_cast<Player>(player_ptr);
        player_ptr2->fire_shot();
      }
    }
    if (Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
      cerr << "LShift key pressed" << endl;
    }
    return false; // don't exit the main game loop
  }
  else if (state == GAME_STATE::Menu)
  {
    // return true to quit the game from the menu
    if (timer < maxTime)
      return false;
    // check for return to Game
    if (Keyboard::isKeyPressed(sf::Keyboard::Enter))
    {
      // escape key activates the Menu
      timer = 0.0f;
      state = GAME_STATE::Game;
    }
    if (Keyboard::isKeyPressed(sf::Keyboard::Delete))
    {
      cout << "game exited from menu" << endl;
      return true;
    }
    return false;
  } // GAME_STATE::Menu
  return false;
} // handle_keyboard_input

// free function to get current date and time as string
const string global::return_current_time_and_date()
{
  chrono::system_clock::time_point p = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(p);
  char str[26];
  ctime_s(str, sizeof str, &t);
  // replace spaces and colons with _ and -
  string s(str);
  size_t pos;
  // replace spaces with underscore
  while ((pos = s.find(" ")) != std::string::npos)
  {
    s.replace(pos, 1, "_");
  }
  // replace colons with underscore
  while ((pos = s.find(":")) != std::string::npos)
  {
    s.replace(pos, 1, "_");
  }
  while ((pos = s.find("\n")) != std::string::npos)
  {
    s.replace(pos, 1, ".");
  }
  return move(s);
}

// free function to create a log file
unique_ptr<fstream> global::create_log_file(const string currDateTime)
{
  string log_file_name = "log-" + currDateTime + "txt";
  auto path = filesystem::current_path();
  cerr << setw(20) << left << "Log location" << path.string() << endl;
  cerr << setw(20) << left << "Log name" << log_file_name << endl;
  return make_unique<fstream>(log_file_name, ios::out);
}

float global::calc_dist(const sf::Vector2f &va, const sf::Vector2f &vb)
{
  return move(sqrt(pow(va.x - vb.x, 2) + pow(va.y - vb.y, 2)));
}

shared_ptr<IEntity> global::get_entity_with_id(unsigned int _id)
{
  for (auto &e : global::entity)
  {
    if (e->id == _id)
    {
      return e;
    }
  }
  return nullptr; // if no entity with that id was found
}


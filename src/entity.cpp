#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
/* */
#include "stdlibs.h"
/* */
#include "builder.h"
#include "entity.h"
#include "global.h"
using FrameTime = float;

using namespace std;
using namespace sf;

// Frag constructors
Frag::Frag(float mX, float mY, sf::Color c = sf::Color::White)
    : vel{0.f, 0.f}, dvel{vel}, health{nullopt}, entity_broke_off_from{nullopt}
{
  setOrigin(global::bW / 2.f, global::bW / 2.f);
  setSize({global::bW, global::bW});
  // don't need to touch vertices here because vertices are
  // added after this move
  move(Vec2(mX, mY));
  setFillColor(c);
  id = global::fragCounter++;
}

void Frag::update()
{
  if (vel.x > global::frag_speed_max) {
    vel.x *= 0.95f;
  }
  if (vel.y > global::frag_speed_max) {
    vel.y *= 0.95f;
  }
  global::move_frag_and_vertices(*this, vel+dvel);
    
}

void Frag::collide_with(const IEntity &e, Vec2 voxPos)
{
  // frags should bounce off bouncy walls and lose health when they hit
  // everything else
  // Dampen explosivity of collisions
  //if (entity_broke_off_from == e.id) return;
  Vec2 bounce_unit_vec;
  float curr_vel_len;
  switch (e.type)
  {
  case EType::BouncyWall:
    curr_vel_len = hypot(dvel.x + vel.x, dvel.y + vel.y);
    bounce_unit_vec = global::make_unit_vec(getPosition() - voxPos);
    vel = bounce_unit_vec * curr_vel_len;
    global::move_frag_and_vertices(*this, bounce_unit_vec * static_cast<float>(global::bW) * 1.4f);
    break;
  case EType::Bullet:
    (*health) = -1;
    bounce_unit_vec = global::make_unit_vec(getPosition() - voxPos);
    vel = bounce_unit_vec * hypot(dvel.x + vel.x, dvel.y + vel.y);
  case EType::Enemy:
    (*health) = -2;
    break;
  default:
    (*health) = 0;
    break;
  };
}
// member functions
unsigned int IEntity::get_health()
{
  return frags.size();
}

//
// BouncyWall Definitions
//
BouncyWall::BouncyWall(Vec2 start, Vec2 end)
{
  // data
  id = global::get_new_entity_id();
  type = EType::BouncyWall;
  builder::add_wall_frags(*this, start, end, Color(210, 35, 90, 255));
  global::set_frag_health(*this, nullopt);
  healthCutoff = 4;
  global::build_hitbox(*this);
}

void BouncyWall::update(FrameTime ftStep)
{

}

void BouncyWall::collide_with(const IEntity &e, unsigned int ivox, Vec2 voxPos, sf::Color fragColor)
{
  // this really should be the color of the frag that hit it not the
  auto modified_color = frags[ivox].getFillColor() + sf::Color(0, 40, 9, 0);
  frags[ivox].setFillColor(modified_color);
  // if we change the frags color we should change it's vertices color
  varray[ivox*4].color = modified_color;
  varray[ivox*4 + 1].color = modified_color;
  varray[ivox*4 + 1].color = modified_color;
  varray[ivox*4 + 1].color = modified_color;
}

void BouncyWall::collide_with_free_frag(unsigned int vi, const Frag &f) { }

//
// Player Definitions
//
Player::Player()
{
  using namespace global;
  // initialize members
  id = global::get_new_entity_id();
  type = EType::Player;
  frags.reserve(5);
  builder::add_player_frags(*this);
  global::build_hitbox(*this);
  healthCutoff = 3;
  vel = Vec2(0.f, 0.f);
  dvel = Vec2(0.f, 0.f);
  canShoot = false;
  currTimer = 0.f;
  timerMax = 215.f;
  global::get_center(*this);
  move_entity(*this, Vec2(winWidth / 2.f, winHeight - 4.f * (float)global::bW));
}

void Player::update(FrameTime ftStep)
{
  currTimer += ftStep;
  global::move_entity(*this, vel + dvel);
  dvel *= (abs(dvel.x) < 0.01f) ? 0 : 0.85f;
}

void Player::collide_with(const IEntity &e, unsigned int ivox, Vec2 voxPos, sf::Color c)
{
  auto bounce_vec = global::make_unit_vec(center - voxPos);
  switch (e.type)
  {
  case EType::BouncyWall:
    dvel += Vec2(bounce_vec.x, 0) * 2.8f;
    break;
  case EType::Bullet:
    global::move_entity(*this, Vec2(bounce_vec.x, 0) * (float)global::bW * 4.f);
    if (global::playerHealth > 0)
    {
      global::playerHealth--;
    }
    break;
  }
}

void Player::collide_with_free_frag(unsigned int vi, const Frag &f)
{
}

void Player::fire_shot()
{
  if (currTimer < timerMax)
  {
    return;
  }
  Vec2 player_pos = hitbox.getPosition();
  auto hitbox_width = hitbox.getSize().x;
  auto player_center = global::get_center(*this);
  // create bullet
  auto new_bullet = make_shared<Bullet>(Vec2(0.f, 0.f));
  // position bullet in front of player
  auto bullet_width = new_bullet->hitbox.getSize().x;
  // this is a hack to center the bullet over the player
  // not sure why I can't subtract bullet_width / 2 and have
  // it line up properly but 3/4 works
  global::move_entity(*new_bullet,
                      player_center - Vec2(3.f * bullet_width / 4.f, +1.f * global::blockWidth * 4.f));
  // add bullet to entity mega-vector
  // TODO: is this move doing anything??
  global::entity.push_back(move(new_bullet));
  // reset shot timer
  currTimer = 0.f;
}

//
// Enemy Definitions
//
Enemy::Enemy(unsigned int enemy_type)
{
  id = global::get_new_entity_id();
  type = EType::Enemy;
  canShoot = false;

  switch (enemy_type)
  {
  case 0:
    builder::add_enemy1_frags(*this);
    global::set_frag_health(*this, 3);
    // ICanShoot
    timerMax = 2000.f; // timerMax in milliseconds
    // IEnemy
    // path loaded from file or generated
    currPathPoint = 0;
    global::get_center(*this);
    // define when enemy explodes
    healthCutoff = 3 * frags.size() / 4;
    global::build_hitbox(*this);
    break;
  case 1:
    builder::add_enemy2_frags(*this);
    global::set_frag_health(*this, 3);
    // ICanShoot
    timerMax = 2000.f; // timerMax in milliseconds
    // randomly generated first path point
    currPathPoint = 0;
    global::get_center(*this);
    // define when enemy explodes
    healthCutoff = 3 * frags.size() / 4;
    global::build_hitbox(*this);
    break;
  };
}
void Enemy::fire_shot()
{
}

void Enemy::update(FrameTime ftStep)
{
  currTimer += ftStep;
  auto hitbox_pos = hitbox.getPosition();
  auto hitbox_size = hitbox.getSize();
  global::move_entity(*this, vel + dvel);
  // dampen enemies dvel
  dvel *= 0.03f;
  // mark enemies as dead if they move off screen
  if (hitbox_pos.x < 0 || hitbox_pos.x + hitbox_size.x / 2 > global::winWidth || hitbox_pos.y < 0 ||
      hitbox_pos.y + hitbox_size.y / 2 > global::winHeight)
  {
    isDead = true;
  }
  // TODO: Make enemies move
}

void Enemy::collide_with(const IEntity &e, unsigned int ivox, Vec2 voxPos, sf::Color c)
{
  auto bounce_vec = frags[ivox].getPosition() - voxPos;
  auto bv_len = hypot(bounce_vec.x, bounce_vec.y);
  auto bounce_unit_vec = bounce_vec / bv_len;
  Vec2 frag_velocity;
  switch (e.type)
  {
  case EType::Bullet:
    (*frags[ivox].health)--;
    frags[ivox].setFillColor(sf::Color::Red);
    // dvel = hypot(vel.x, vel.y) * bounce_unit_vec * 1.2f;
    dvel += bounce_unit_vec * 0.5f;
    frag_velocity = bv_len * bounce_unit_vec;
    // if enemy frag health decreased move it to free frag
    if (*frags[ivox].health == 1)
    {
      // move the frag outside the enemy so doesn't hurt itself
      frags[ivox].vel = frag_velocity;
      global::frags_to_free.insert(make_pair(id, frags[ivox].id));
    }
    break;
  case EType::Enemy:
    // this case left out because its handled by global::collide_enemy_hitboxes()
    return;
  case EType::BouncyWall:
    dvel += bounce_unit_vec * 0.8f;
    break;
  };
}
void Enemy::collide_with_free_frag(unsigned int vi, const Frag &f)
{
  auto voxPos = f.getPosition();
  auto bounce_vec = frags[vi].getPosition() - voxPos;
  auto bv_len = hypot(bounce_vec.x, bounce_vec.y);
  auto bounce_unit_vec = bounce_vec / bv_len;
  frags[vi].setFillColor(sf::Color::Red - sf::Color(90, 0, 0, 0));
  (*(frags[vi]).health)--;
  if (*frags[vi].health <= 1)
  {
    frags[vi].vel = bounce_unit_vec * hypot(f.vel.x, f.vel.y) * 0.5f;
    global::frags_to_free.insert(make_pair(id, frags[vi].id));
  }
}
//
// Bullet Definitions
//
Bullet::Bullet(Vec2 pos)
{
  using namespace global;
  // initialize members
  id = global::get_new_entity_id();
  type = EType::Bullet;
  frags.reserve(6);
  builder::add_bullet2_frags(*this);
  builder::set_frag_health(*this, 2);
  global::build_hitbox(*this);
  healthCutoff = 2;
  vel = Vec2(0.f, -5.5f);
  dvel = Vec2(0.f, -3.f);
  global::get_center(*this);
  move_entity(*this, pos);
}
void Bullet::update(FrameTime ftStep)
{
  global::move_entity(*this, vel + dvel);
  dvel *= 0.9f;
  auto hitbox_pos = hitbox.getPosition();
  auto hitbox_size = hitbox.getSize();
  if (hitbox_pos.x < 0 || hitbox_pos.x + hitbox_size.x > global::winWidth || hitbox_pos.y < 0 ||
      hitbox_pos.y > global::winHeight)
  {
    isDead = true;
  }
}

void Bullet::collide_with(const IEntity &e, unsigned int ivox, Vec2 voxPos, sf::Color c)
{
  auto bounce_vec = frags[ivox].getPosition() - voxPos;
  auto bv_len = hypot(bounce_vec.x, bounce_vec.y);
  auto bounce_unit_vec = bounce_vec / bv_len;
  Vec2 frag_velocity;
  switch (e.type)
  {
  case EType::BouncyWall:
    // move entity so it doesn't collide again in this frame
    global::move_entity(*this, bounce_unit_vec * (float)global::bW * 0.6f);
    vel = hypot(vel.x, vel.y) * bounce_unit_vec;
    vel += Vec2((-2.5f + global::rand_engine() % 4) * 0.05f, 0.f);
    frag_velocity = vel;
    break;
  case EType::Enemy:
    frag_velocity = hypot(vel.x, vel.y) * bounce_unit_vec * 1.2f;
    (*frags[ivox].health)--;
    break;
  };
  // if bullet has taken a hit we move it to free frag
  if (*frags[ivox].health == 1)
  {
    frags[ivox].vel = frag_velocity;
    global::frags_to_free.insert(make_pair(id, frags[ivox].id));
  }
}

void Bullet::collide_with_free_frag(unsigned int vi, const Frag &f)
{
  (*frags[vi].health)--;
  // if bullet has taken a hit we move it to free frag
  auto bounce_vec = frags[vi].getPosition() - f.getPosition();
  auto bv_len = hypot(bounce_vec.x, bounce_vec.y);
  auto bounce_unit_vec = bounce_vec / bv_len;
  auto frag_velocity = bounce_unit_vec * hypot(dvel.x + vel.x, dvel.y + vel.y);
  if (*frags[vi].health == 1)
  {
    frags[vi].vel = frag_velocity;
    global::frags_to_free.insert(make_pair(id, frags[vi].id));
  }
}


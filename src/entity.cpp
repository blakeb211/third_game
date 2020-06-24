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
Frag::Frag() {}
Frag::Frag(float mX, float mY, sf::Color c = sf::Color::White)
    : vel{0.f, 0.f}, dvel{vel}, health{nullopt} {
  setOrigin(global::bW / 2.f, global::bW / 2.f);
  setSize({global::bW, global::bW});
  move(Vec2(mX, mY));
  setFillColor(c);
  id = global::fragCounter++;
}
void Frag::update() { move(vel + dvel); }
void Frag::collide_with(const IEntity& e, Vec2 voxPos) { (*health)--; }
// member functions
unsigned int IEntity::get_health() { return frags.size(); }

//
// BouncyWall Definitions
//
// wall is just a non-moving entity with optional health
BouncyWall::BouncyWall(Vec2 start, Vec2 end) {
  // data
  id = global::get_new_entity_id();
  type = EType::BouncyWall;
  healthCutoff = 4;

  builder::add_wall_frags(*this, start, end, Color(210, 35, 90, 255));
  global::build_hitbox(*this);
}

void BouncyWall::update(FrameTime ftStep) {}
void BouncyWall::collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos,
                              sf::Color fragColor) {
  // this really should be the color of the frag that hit it not the
  // color of the first frag of the entity
  frags[ivox].setFillColor(frags[ivox].getFillColor() - sf::Color(15,15,15,0));
}

void BouncyWall::collide_with_free_frag(unsigned int vi, const Frag& f) {}

//
// Player Definitions
//
Player::Player() {
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
  // data
  canShoot = false;
  currTimer = 0.f;
  timerMax = 215.f;
  move_entity(*this, Vec2(10.f, winHeight - 10.f));
}

void Player::update(FrameTime ftStep) {
  currTimer += ftStep;
  global::move_entity(*this, vel + dvel);
  dvel *= (abs(dvel.x) < 0.01f) ? 0 : 0.75f;
}

void Player::collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos,
                          sf::Color c) {}
void Player::collide_with_free_frag(unsigned int vi, const Frag& f) {}
void Player::fire_shot() {
  if (currTimer < timerMax) return;
  Vec2 player_pos = hitbox.getPosition();
  auto hitbox_width = hitbox.getSize().x;
  auto player_center = player_pos + Vec2(hitbox_width / 2.f, 0.f);

  auto new_bullet = make_shared<Bullet>(Vec2(0.f, 0.f));
  auto bullet_width = new_bullet->hitbox.getSize().x;
  global::move_entity(*new_bullet,
                      player_center - Vec2(bullet_width / 4.f,
                                           +1.f * global::blockWidth * 4.f));
  global::entity.push_back(move(new_bullet));
  // reset shot timer
  currTimer = 0.f;
}

//
// Enemy Definitions
//
Enemy::Enemy(unsigned int enemy_type) {
  id = global::get_new_entity_id();
  type = EType::Enemy;
  builder::add_enemy1_frags(*this);
  healthCutoff = 2 * frags.size() / 3;
  global::build_hitbox(*this);
  global::set_frag_health(*this, 5);
  // ICanShoot
  canShoot = false;
  timerMax = 2000.f;  // timerMax in milliseconds
  // IEnemy
  // path loaded from file or generated
  path.emplace_back(Vec2(200 + global::rand_engine() % 600, 200 + global::rand_engine() % 300));
  currPathPoint = 0;
  global::move_entity(*this, path[currPathPoint]);
}
void Enemy::fire_shot() {}
void Enemy::update(FrameTime ftStep) {
  auto hitbox_pos = hitbox.getPosition();
  auto hitbox_size = hitbox.getSize();
  global::move_entity(*this, vel + dvel);
  dvel *= 0.03f;
  // mark enemies as dead if they move off screen
  if (hitbox_pos.x < 0 || hitbox_pos.x + hitbox_size.x > global::winWidth ||
      hitbox_pos.y < 0 || hitbox_pos.y > global::winHeight) {
    isDead = true;
  }
}

void Enemy::collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos,
                         sf::Color c) {
  auto bounce_vec = frags[ivox].getPosition() - voxPos;
  auto bv_len = hypot(bounce_vec.x, bounce_vec.y);
  auto bounce_unit_vec = bounce_vec / bv_len;
  Vec2 frag_velocity;
  switch (e.type) {
    case EType::Bullet:
      (*frags[ivox].health)--;
      frags[ivox].setFillColor(sf::Color::Red);
      // dvel = hypot(vel.x, vel.y) * bounce_unit_vec * 1.2f;
      dvel += bounce_unit_vec * 0.5f;
      frag_velocity = 8.f * dvel;
      break;
    case EType::Enemy:
      break;
  };
  // if enemy frag health == 1 move it to free frag
  if (*frags[ivox].health <= 2) {
    frags[ivox].vel = frag_velocity;
    global::frags_to_move.insert(make_pair(id, frags[ivox].id));
  }
}
void Enemy::collide_with_free_frag(unsigned int vi, const Frag& f) {
  (*(frags[vi]).health)--;
}
//
// Bullet Definitions
//
Bullet::Bullet(Vec2 pos) {
  using namespace global;
  // initialize members
  id = global::get_new_entity_id();
  type = EType::Bullet;
  frags.reserve(6);
  builder::add_bullet1_frags(*this);
  builder::set_frag_health(*this, 2);
  global::build_hitbox(*this);
  healthCutoff = 2;
  vel = Vec2(0.f, -5.5f);
  dvel = Vec2(0.f, -3.f);
  move_entity(*this, pos);
}
void Bullet::update(FrameTime ftStep) {
  global::move_entity(*this, vel + dvel);
  dvel *= 0.9f;
  auto hitbox_pos = hitbox.getPosition();
  auto hitbox_size = hitbox.getSize();
  if (hitbox_pos.x < 0 || hitbox_pos.x + hitbox_size.x > global::winWidth ||
      hitbox_pos.y < 0 || hitbox_pos.y > global::winHeight) {
    isDead = true;
  }
}

void Bullet::collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos,
                          sf::Color c) {
  auto bounce_vec = frags[ivox].getPosition() - voxPos;
  auto bv_len = hypot(bounce_vec.x, bounce_vec.y);
  auto bounce_unit_vec = bounce_vec / bv_len;
  Vec2 frag_velocity;
  switch (e.type) {
    case EType::BouncyWall:
      //(*frags[ivox].health)--;
      // move entity so it doesn't collide again in this frame
      global::move_entity(*this, bounce_unit_vec * (float)global::bW * 0.6f);
      vel = hypot(vel.x, vel.y) * bounce_unit_vec * 1.2f;
      vel += Vec2((-2.5f + global::rand_engine() % 4) * 0.1f, 0.f);
      frag_velocity = vel;
      break;
    case EType::Enemy:
      frag_velocity = hypot(vel.x, vel.y) * bounce_unit_vec * 1.2f;
      (*frags[ivox].health)--;
      break;
  };
  // if bullet has taken a hit we move it to free frag
  if (*frags[ivox].health == 1) {
    frags[ivox].vel = frag_velocity;
    global::frags_to_move.insert(make_pair(id, frags[ivox].id));
  }
}

void Bullet::collide_with_free_frag(unsigned int vi, const Frag& f) {}


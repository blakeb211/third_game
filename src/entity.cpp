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
Frag::Frag() { }
Frag::Frag(float mX, float mY, sf::Color c = sf::Color::White)
    : vel{0.f, 0.f}, dvel{vel}, health{nullopt} {
      setOrigin(global::bW / 2.f, global::bW / 2.f);
      setSize({global::bW,global::bW});
      move(Vec2(mX, mY));
      setFillColor(c);
    }

// member functions
unsigned int IEntity::get_health() { return frags.size(); }
void IEntity::erase_dead_frags() {}

//
// BouncyWall Definitions
//
// wall is just a non-moving entity with optional health
BouncyWall::BouncyWall(Vec2 start, Vec2 end) {}
void BouncyWall::update(FrameTime ftStep) {}
void BouncyWall::collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) {}

//
// Player Definitions
//
Player::Player() {
  using namespace global;
  // initialize members
  id = global::get_new_entity_id();
  type = EType::Player;
  frags.resize(15);
  builder::add_player_frags(*this);
  healthCutoff = 3;
  vel = Vec2(0.f, 0.f);
  dvel = Vec2(0.f, 0.f);
  sf::Rect<float> hitbox;
  // data
  canShoot = false;
  currTimer = 0.f;
  timerMax = 0.f;
  move_entity(*this, Vec2(10.f, winHeight - 10.f));
}
void Player::update(FrameTime ftStep) {}
void Player::collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) {}
void Player::fire_shot() {
  auto player_pos = (frags[0].getPosition() + frags[frags.size()-1].getPosition() / 2.f);
  global::entity.emplace_back(make_shared<Bullet>(player_pos + Vec2(0.f, -10.f) ));
}

//
// Enemy Definitions
//
Enemy::Enemy(unsigned int enemy_type) {}
void Enemy::fire_shot() {}
void Enemy::update(FrameTime ftStep) {}
void Enemy::collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) {}


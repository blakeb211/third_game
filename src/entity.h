#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
/* */
#include "stdlibs.h"
using Vec2 = sf::Vector2<float>;

enum struct EType {
  Player = 1,
  Bullet = 2,
  BouncyWall = 3,
  BreakableWall = 4,
  Enemy = 5
};

struct Frag : sf::RectangleShape {
  Frag(float mX, float mY, sf::Color c = sf::Color::White);
  // member data
  Vec2 vel;
  Vec2 dvel;
  std::optional<int> health;  // voxel health
};

struct IEntity {
// pure virtuals
  virtual void update(FrameTime ftStep) = 0;
  virtual void collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) = 0;
  unsigned int get_health() final { return vox.size(); }
  void erase_dead_frags() final { }
// data
  EType type;
  std::vector<Frag> frags;
  unsigned int healthCutoff;
  Vec2 vel;
  Vec2 dvel;
  sf::Rect hitbox;
};

struct ICanShoot {  // players and enemies
  // pure virtuals
  // fire_shot
bool canShoot;
float currTimer;
const float timerMax;
};

struct IEnemy {
 // pure virtuals 
 // data
  std::vector<Vec2> path;
  unsigned int currPathPoint;
}

// wall is just a non-moving entity with optional health

struct BouncyWall : IEntity  {                             
  virtual void update(FrameTime ftStep) override; 
  virtual void collideWith(Entity& e, unsigned int ivox, Vec2 voxPos) override;
};

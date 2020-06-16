#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
/* */
#include "stdlibs.h"
using Vec2 = sf::Vector2<float>;
using FrameTime = float;

enum struct EType {
  Player = 1,
  Bullet = 2,
  BouncyWall = 3,
  BreakableWall = 4,
  Enemy = 5
};

struct Frag : sf::RectangleShape {
  Frag(float mX, float mY, sf::Color c);
  // member data
  Vec2 vel;
  Vec2 dvel;
  std::optional<int> health;  // voxel health
};

struct IEntity {
  // pure virtuals
  virtual void update(FrameTime ftStep) = 0;
  virtual void collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) = 0;
  // member functions
  unsigned int get_health(); 
  void erase_dead_frags(); 
  // data
  unsigned int id;
  EType type;
  std::vector<Frag> frags;
  unsigned int healthCutoff;
  Vec2 vel;
  Vec2 dvel;
  sf::Rect<float> hitbox;
};

struct ICanShoot {  // players and enemies
  ICanShoot();
  // pure virtuals
  virtual void fire_shot() = 0;
  // data
  bool canShoot;
  float currTimer;
  const float timerMax;
};

struct IEnemy {
  // pure virtuals
  // add AI functions if necessary
  // data
  std::vector<Vec2> path;
  unsigned int currPathPoint;
};

struct Enemy : IEntity, ICanShoot, IEnemy {
  Enemy(unsigned int enemy_type); 
  void fire_shot();
  void update(FrameTime ftStep);
  void collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos);
};
// wall is just a non-moving entity with optional health

struct BouncyWall : IEntity {
  BouncyWall(Vec2 start, Vec2 end); 
  void update(FrameTime ftStep);
  void collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos);
};

struct Player : IEntity, ICanShoot {
  void update(FrameTime ftStep);
  void collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos);
  void fire_shot();
};

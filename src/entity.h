#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
/* */
#include "stdlibs.h"
using Vec2 = sf::Glsl::Vec2;
using FrameTime = float;
enum struct EType {
  Player = 1,
  Bullet = 2,
  BouncyWall = 3,
  BreakableWall = 4,
  Enemy = 5
};

struct IEntity; 

struct Frag : sf::RectangleShape {
  Frag();
  Frag(float mX, float mY, sf::Color c);
  void update();
  void collide_with(const IEntity& e, Vec2 voxPos);
  // member data
  Vec2 vel;
  Vec2 dvel;
  std::optional<int> health;  // voxel health
  size_t id;
};

struct IEntity {
  // pure virtuals
  virtual void update(FrameTime ftStep) = 0;
  virtual void collide_with_free_frag(const Frag& f) = 0;
  virtual void collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos, sf::Color c) = 0;
  // member functions
  unsigned int get_health(); 
  // data
  unsigned int id;
  EType type;
  std::vector<Frag> frags;
  unsigned int healthCutoff;
  Vec2 vel;
  Vec2 dvel;
  sf::RectangleShape hitbox;
  bool isDead{false};
};

struct ICanShoot {  // players and enemies
  // pure virtuals
  virtual void fire_shot() = 0;
  // data
  bool canShoot;
  float currTimer;
  float timerMax;
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
  void collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos, sf::Color c);
  void collide_with_free_frag(const Frag& f);
};
// wall is just a non-moving entity with optional health

struct BouncyWall : IEntity {
  BouncyWall(Vec2 start, Vec2 end); 
  void update(FrameTime ftStep);
  void collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos, sf::Color c);
  void collide_with_free_frag(const Frag& f);
};

struct Player : IEntity, ICanShoot {
  Player();
  void update(FrameTime ftStep);
  void collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos, sf::Color c);
  void fire_shot();
  void collide_with_free_frag(const Frag& f);
};

struct Bullet : IEntity {
  Bullet(Vec2 pos);
  void update(FrameTime ftStep);
  void collide_with(const IEntity& e, unsigned int ivox, Vec2 voxPos, sf::Color c);
  void collide_with_free_frag(const Frag& f);
};


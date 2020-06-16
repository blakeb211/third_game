#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
/* */
#include "stdlibs.h"
/* */
#include "entity.h"
using Vec2 = sf::Vector2<float>;
using FrameTime = float;

using namespace std;
using namespace sf;

// Frag constructor
Frag::Frag(float mX, float mY, sf::Color c = sf::Color::White)
    : vel{0.f, 0.f}, dvel{vel}, health{nullopt} {}

// member functions
unsigned int IEntity::get_health() { return frags.size(); }
void IEntity::erase_dead_frags() {}

ICanShoot::ICanShoot() : timerMax{3.0f} {}

//
// BouncyWall Definitions
//
// wall is just a non-moving entity with optional health
BouncyWall::BouncyWall(Vec2 start, Vec2 end) { }
void BouncyWall::update(FrameTime ftStep) {}
void BouncyWall::collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) {}

//
// Player Definitions
//
void Player::update(FrameTime ftStep) {}
void collideWith(IEntity& e, unsigned int ivox, Vec2 voxPos) {}
void fire_shot() {}

//
// Enemy Definitions
//
Enemy::Enemy(unsigned int enemy_type) {}
void Enemy::fire_shot() {}
void Enemy::update(FrameTime ftStep) {}
void Enemy::collide_with(IEntity& e, unsigned int ivox, Vec2 voxPos) {}


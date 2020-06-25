//------------------------------------------------------------------------------
// builder.h, Blake Baird
//------------------------------------------------------------------------------
// Defines
//  Voxel struct
//  Builder namespace - Defines relative position and color of voxels
//                      for each Entity.
//------------------------------------------------------------------------------

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "entity.h"
#include "stdlibs.h"

namespace builder {

void add_player_frags(IEntity &e);
void add_bullet1_frags(IEntity &e);
void add_bullet2_frags(IEntity &e);
void add_enemy1_frags(IEntity &e);
void add_enemy2_frags(IEntity &e);
void add_enemy3_frags(IEntity &e);
void add_enemy4_frags(IEntity &e);
void build_level(unsigned int &levelId);
void add_wall_frags(IEntity &e, Vec2 start, Vec2 end, sf::Color c);
void set_frag_health(IEntity &e, unsigned int num);
} // namespace builder

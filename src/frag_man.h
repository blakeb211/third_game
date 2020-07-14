#pragma once
#include "entity.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <vector>
#include <utility>
// frag_man.h :
//
//                    manage vertex array for draw calls
//                    create_entity
//                    move_entity
//                    set_entity_frag_color
//                    set_frag_color
//                    remove_dead_frags from being drawn
//                    remove_dead_entities
// Responsibilites:   pre-allocate a big bag of frags

// Guidelines:        Should drop into current game easily
//                    Only frag manager calls the Frag constructor

namespace frag_man
{

// Globals
// free frag vertex array
inline sf::VertexArray ff_varray{sf::PrimitiveType::Quads, 20'000};

// return Frag from frag pool
std::unique_ptr<Frag> get_new_frag(float mx, float my, sf::Color c);

void create_entity_varray_from_frags(IEntity &e);

} // namespace frag_man

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// frag_man.h :
//
// Responsibilites:   pre-allocate a big bag of frags
//                    manage vertex array for draw calls
//                    create_entity
//                    move_entity
//                    set_entity_frag_color
//                    set_frag_color
//                    remove_dead_frags from being drawn
//                    remove_dead_entities

// Guidelines:        Should drop into current game easily

namespace frag_man
{

// Globals
inline auto sf::VertexArray varray{sf::PrimitiveType::Quads, 20'000};
//inline std::vector<size_t> ff_offset{};

void pre_allocate_frags(size_t count);

} // namespace frag_man

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

inline std::vector<std::pair<size_t, size_t>> id_to_vertex_location;

// return Frag from frag pool
std::unique_ptr<Frag> get_new_frag(float mx, float my, sf::Color c);

void create_entity_varray_from_frags(IEntity &e);

//TODO: Implement these so that frags are erased better and free_frags stays 
//      synced up with frag_man::ff_varray
void swap_free_frag_and_vertices_within_vector(int ff_offset_from, int ff_offset_to);

void erase_free_frags_and_verticies_if_cutoff_hit(int ffCutoff);
// void pre_allocate_frags(size_t count);

} // namespace frag_man

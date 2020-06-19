#pragma once
#include <SFML/System.hpp>
#include <chrono>
#include <random>
#include <memory>
/* */
#include <SFML/Graphics.hpp>
#include "entity.h"
namespace global {
//
// constants
//
inline const unsigned int winWidth = 1024;
inline const unsigned int winHeight = 768;
inline const unsigned int blockWidth = 5;
inline const unsigned int bW = blockWidth;
inline const float ftStep = 16.f;  // fixed update interval in ms
inline const float keyInputStep = ftStep * 3.f;
inline unsigned int lastFPS = 0; 
inline std::default_random_engine rand_engine;
inline std::vector<std::shared_ptr<IEntity>> entity;
inline std::vector<Frag> free_frags;
inline unsigned int entityCounter = 0;
inline std::shared_ptr<IEntity> player_ptr;
inline std::set<std::pair<int,int>> frags_to_move;
//
// free functions
//
void erase_moved_frags_from_entities(); 

// function to move frags that were freed from their entities
void process_set_of_freed_frags(); 
// function to print out ETypes as strings
inline std::ostream & operator<<(std::ostream & Str, EType V); 

// check entities for collisions 
void check_entities_for_collisions(); 

// generate the entity's hitbox for collision checking
void remove_dead_entities(); 

// generate the entity's hitbox for collision checking
void build_hitbox(IEntity& e); 

// move entity's pos variable and all its frags with it
void move_entity(IEntity& e, const Vec2 offset);

// get a new unique entity id
unsigned int get_new_entity_id();

// create a window
std::unique_ptr<sf::RenderWindow> create_window();

// calculate the frames per second using
// the time the frame started
std::pair<float, float> calc_frames_per_second(
    const std::chrono::high_resolution_clock::time_point&);

// check for window close
bool check_for_window_close(sf::RenderWindow& window, sf::Event& event);

// handle user input
bool handle_keyboard_input(float timer, const float maxTime, sf::RenderWindow& window);

// get current date and time as string
const std::string return_current_time_and_date();

// create a log file
std::unique_ptr<std::fstream> create_log_file(const std::string currDateTime);

// calc distance between two vectors 
float calc_dist(const sf::Vector2f &va, const sf::Vector2f &vb);

// return reference to the entity with the given id 
std::shared_ptr<IEntity> get_entity_with_id(unsigned int);
}  // namespace global

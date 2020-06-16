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
const unsigned int winWidth = 1024;
const unsigned int winHeight = 768;
const unsigned int blockWidth = 5;
const unsigned int bW = blockWidth;
const float ftStep = 16.f;  // fixed update interval in ms
const float keyInputStep = ftStep;
inline unsigned int lastFPS = 0; 
inline std::default_random_engine rand_engine;
inline std::vector<std::shared_ptr<IEntity>> entity;
inline unsigned int entityCounter = 0;
//
// free functions
//
// move entity's pos variable and all its frags with it
void move_entity(IEntity& e, const Vec2 offset);

// get a new unique entity id
unsigned int get_new_entity_id();

// create a window
std::unique_ptr<sf::RenderWindow> create_window();

// calculate the frames per second using
// the time the frame started
std::pair<float, float> calc_frames_per_second(
    const std::chrono::high_resolution_clock::time_point);

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

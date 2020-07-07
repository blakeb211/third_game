#pragma once
#include <SFML/System.hpp>
#include <chrono>
#include <memory>
#include <random>
/* */
#include <SFML/Graphics.hpp>

#include "entity.h"
namespace global {
//
// GLOBALS
// 
enum GAME_STATE {
  Menu,
  Pause,
  Level_Screen,
  Game,
  Editor,
};

//
// Global Inline Variables 
//
inline const unsigned int winWidth {1024};
inline const unsigned int winHeight {768};
inline const unsigned int blockWidth {5};
inline const unsigned int & bW {blockWidth};
inline const float ftStep {13.f}; // fixed update interval in ms
inline const float keyInputStep {ftStep * 3.f};
inline unsigned int lastFPS {0};
inline std::default_random_engine rand_engine;
inline std::vector<std::shared_ptr<IEntity>> entity;
inline std::vector<Frag> free_frags;
inline size_t entityCounter {0};
inline size_t fragCounter {0};
inline std::shared_ptr<IEntity> player_ptr;
inline std::set<std::pair<size_t, size_t>> frags_to_free;
inline GAME_STATE state {GAME_STATE::Menu};
inline unsigned int score {0};
inline unsigned int level {0};
inline unsigned int playerHealth {3};
inline sf::Font font; // unitialized
inline std::vector<sf::Text> menu_text;
inline sf::Text txt_score;
inline sf::VertexArray ff_varray{sf::PrimitiveType::Quads, 200};
// inline global colors
inline sf::Color clearscreen_color(128, 128, 115, 255);

//
// free functions
//
//

// make a unit vec from a vector
Vec2 make_unit_vec(Vec2 v);

// calculate entity center if it hasn't been calculated yet, otherwise return it
Vec2 get_center(IEntity& e);

void collide_enemy_hitboxes(IEntity& ei, IEntity& ej);

// generate random integer between a low and high range
int rand_between(int low, int high);
//
// loop of the frags in an entity and set their health
void set_frag_health(IEntity &e, std::optional<unsigned int> h);
// use some criteria to erase free frags
void erase_freed_frags();

// fxn to erase a frag with a given id from an entity if it is present
void erase_frag_with_id(IEntity &e, size_t frag_id);

std::unique_ptr<Frag> get_frag_with_id(IEntity &e, size_t frag_id);
// function to move frags that were freed from their entities
void process_set_of_freed_frags();
// function to print out ETypes as strings
inline std::ostream &operator<<(std::ostream &Str, EType V);

// check entities for collisions
void check_entities_for_collisions();

// check free frags for collisions
void check_free_frags_for_collisions();

// generate the entity's hitbox for collision checking
void remove_dead_entities();

// generate the entity's hitbox for collision checking
void build_hitbox(IEntity &e);

// move entity's pos variable and all its frags with it
void move_entity(IEntity &e, const Vec2 offset);

// rotate entity's frags by offset
void rotate_entity(IEntity &e, const float ang_offset);

// get a new unique entity id
unsigned int get_new_entity_id();

// create a window
std::unique_ptr<sf::RenderWindow> create_window();

// calculate the frames per second using
// the time the frame started
std::pair<float, float>
calc_frames_per_second(const std::chrono::high_resolution_clock::time_point &);

// check for window close
bool check_for_window_close(sf::RenderWindow &window, sf::Event &event);

// handle user input
bool handle_keyboard_input(float &timer, const float maxTime, sf::RenderWindow &window);

// get current date and time as string
const std::string return_current_time_and_date();

// create a log file
std::unique_ptr<std::fstream> create_log_file(const std::string currDateTime);

// calc distance between two vectors
float calc_dist(const sf::Vector2f &va, const sf::Vector2f &vb);

// return reference to the entity with the given id
std::shared_ptr<IEntity> get_entity_with_id(unsigned int);


} // namespace global

#pragma once
#include <SFML/System.hpp>
#include <chrono>
/* */
#include <SFML/Graphics.hpp>

namespace global {
//
// constants
//
const float ftStep = 16.f;  // fixed update interval in ms
const float keyInputStep = ftStep;

//
// free functions
//

// create a window
std::unique_ptr<sf::RenderWindow> create_window();

// calculate the frames per second using
// the time the frame started
std::pair<float, float> calc_frames_per_second(
    const std::chrono::high_resolution_clock::time_point);

// check for window close
bool check_for_window_close(sf::RenderWindow& window, sf::Event& event);

// handle user input
void handle_keyboard_input(float timer, const float maxTime);

// get current date and time as string
const std::string return_current_time_and_date();

// create a log file
std::unique_ptr<std::fstream> create_log_file(const std::string currDateTime);

}  // namespace global

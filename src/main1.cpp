#include "..\include\stdlibs.h"
/*                  */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


using namespace std;
using namespace sf;
typedef chrono::high_resolution_clock high_res_clock;

// free function to create a window
unique_ptr<RenderWindow> create_window() {
  const ContextSettings cs(0, 0, 2, 4, 4, ContextSettings::Default, true);
  auto window = make_unique<RenderWindow>(VideoMode(1024, 768), "Iteration 3",
                                          Style::Close, cs);
  window->setVerticalSyncEnabled(true);  // prevent screen tearing
  return move(window);
}

// free function to calculate the frames per second using
// the time the frame started
pair<float, float> calc_frames_per_second(
    const high_res_clock::time_point timePoint1) {
  auto timePoint2(high_res_clock::now());
  float frametime{chrono::duration_cast<chrono::duration<float, milli>>(
                      timePoint2 - timePoint1)
                      .count()};
  auto fSeconds = frametime / 1000.f;
  auto fps = 1.f / fSeconds;
  return make_pair(roundf(fps), frametime);
}

// free funnction to check for window close
bool check_for_window_close(RenderWindow& window, Event& event) {
  while (window.pollEvent(event)) {
    switch(event.type) {
      case Event::Closed: 
        cerr << "User exited window" << endl;
        window.close();
        return true;
    };
  }
  return false;
}

// get user input
void handle_keyboard_input(float timer, const float maxTime) {
  if (timer < maxTime) return;
  if (Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      cerr << "Esc key pressed" << endl;
  }
  if (Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      cerr << "Left key pressed" << endl;
  }
  if (Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      cerr << "Right key pressed" << endl;
  }
  if (Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      cerr << "Space key pressed" << endl;
  }
  if (Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
      cerr << "LShift key pressed" << endl;
  }
  timer = 0.f;
}

// free function to get current date and time as string
const string return_current_time_and_date() {
  chrono::system_clock::time_point p = chrono::system_clock::now();
  time_t t = chrono::system_clock::to_time_t(p);
  char str[26];
  ctime_s(str, sizeof str, &t);
  // replace spaces and colons with _ and -
  string s(str);
  size_t pos;
  // replace spaces with underscore
  while ((pos = s.find(" ")) != std::string::npos) {
    s.replace(pos, 1, "_");
  }
  // replace colons with underscore  
  while ((pos = s.find(":")) != std::string::npos) {
    s.replace(pos, 1, "_");
  }
  while ((pos = s.find("\n")) != std::string::npos) {
    s.replace(pos, 1, ".");
  }
  return move(s);
}

// free function to create a log file
unique_ptr<fstream> create_log_file(const string currDateTime) {
  string log_file_name = "log-" + currDateTime + "txt";
  auto path = filesystem::current_path();
  cerr << setw(20) << left <<  "Log location" <<  path.string() << endl;
  cerr << setw(20) << left << "Log name" << log_file_name << endl;
  return make_unique<fstream>(log_file_name, ios::out);
}

int main() {
  // Initialization
  auto window = create_window();
  Event event;
  const float ftStep = 16.f;  // fixed update interval in ms
  const float keyInputStep = ftStep; 
  float ftAccum = 0.f;        // frametime accumulator
  float keyTimeAccum = 0.f;
  auto log_file = create_log_file(return_current_time_and_date()); 

  while (window->isOpen()) {
    const auto timePoint1 = high_res_clock::now();

    // Get user input
    if (check_for_window_close(*window, event)) break;
    handle_keyboard_input(keyTimeAccum, keyInputStep);
    // Update phase
    for (; ftAccum >= ftStep; ftAccum -= ftStep) {
      /*  */
    }
    
    // Draw Phase
    window->clear(Color::Black);
    window->display();

    // Frame Timings
    auto timings = calc_frames_per_second(timePoint1);
    auto fps_string = "FPS: " + to_string(static_cast<int>(timings.first));
    window->setTitle(fps_string);
    ftAccum += timings.second;
    keyTimeAccum += timings.second;
    *log_file << fps_string << "\n";  
  }
  log_file->close();
  cerr << "Logfile closing" << endl;
  return 0;
}

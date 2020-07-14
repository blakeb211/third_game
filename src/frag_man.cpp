#include "frag_man.h"
#include "entity.h"
#include "global.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <cassert>
using namespace std;

std::unique_ptr<Frag> frag_man::get_new_frag(float mx, float my, sf::Color c)
{
  // get frag from frag pool
  return make_unique<Frag>(mx, my, c);
}


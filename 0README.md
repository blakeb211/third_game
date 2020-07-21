Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

Author:             Blake Baird, 2020
Philosophy:         K.I.S.S.

Goals
  3 levels with bouncy and absorby walls
  Enemies shoot
  Enemies follow paths
  Sound - same for all levels

TODO:
    add enemies follow paths
    add enemies shoot
    add absorby wall to editor and game
    update screenshots_and_vids folder

Refactoring 
  separate out mathlib
  dot products 
  lerp function 
  unit vector creation

Directory Organization
  src directory for all *.cpp and *.h files
  assets directory for fonts, level data, sounds
  build directory for *.dll and *.exe

Optimization
  Complete
    reduce draw calls 
  Unnecessary so far 
    memory pool
    quadtree (collisions are 1/10th of frametime so this isn't priority)

Engine Additions
  rotating entities
  particle effects - Smoke
  different backgrounds per level 

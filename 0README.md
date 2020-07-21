Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

Author:             Blake Baird, 2020
Philosophy:         K.I.S.S.

Goals
  3 levels
  Enemies shoot
  Sound

Current Feature
  Level editor
    add fixed grid - complete
    save level data

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

Refactoring 
  separate out mathlib
  dot products 
  lerp function 
  unit vector creation

Directory Organization
  src directory for all *.cpp and *.h files
  assets directory for fonts, level data, sounds
  build directory for *.dll and *.exe


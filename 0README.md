KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

*******************************************************************
**  ROOM FOR OPTIMIZATION
**    DONE  reduce draw calls using SFML vertexarray
**    NO    add a memory pool for frags 
**    NO    quadtree
**          collisions are approximately 1/10th or less of a frame
*******************************************************************

******************************
REDUCE DRAW CALLS PLAN
******************************
add a frag_manager class
  done - each entity will be one draw call and 
  done - free frags will be one draw call

Take a step back and refactor

TODO:
  IN GAME LEVEL EDITOR WITH A FIXED GRID
	ADD FIXED GRID
	ADD WRITING TO FILE
    make 3 levels
  Add enemy shooting
  ADD SOUND
  add particle system


ROTATE ENTITY ON GRID
  colliding bullets, enemies 

GOALS
  3 LEVELS
  BOSS

REFACTORING OPTIONS
    separate out mathlib
    DOTPRODUCTS 
    LERP FUNCTION
    make-unit-vector

READ THROUGH 3 OTHER GAME CODES
READ THROUGH MY CODE
READ THROUGH SFML API AGAIN

Study Materials
  read through sfml game project code
  read through sfml api
  2nd half of Moo book

Coding Guidelines 

Directory Org
  src directory for source files 

  Engine changes
    add hit boxes and only check against those -> if those intersect then do voxel-by-voxel checking

NOTES:
  use lldb to debug 



KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

*******************************************************************
**  ROOM FOR OPTIMIZATION
**    reduce draw calls using SFML vertexarray
**    Do not need to use a quadtree for this game because the 
**    collision timings are around 300ms, which is 
**    approximately 1/10th or less of a frame
*******************************************************************

TODO:
  IN GAME LEVEL EDITOR WITH A FIXED GRID
    Fix level editor format and use regex or propertymap
  Add enemy AI 
  ADD SOUND
  add particle system
  Add BOSSES

ROTATE ENTITY ON GRID
  colliding bullets, enemies 

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



KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

*******************************************************************
**  ROOM FOR OPTIMIZATION
**    reduce draw calls using SFML vertexarray
**    make logfile async? - not sure if this would help
**    set O2 level and possibly switch STL algorithms for EASTL or hand-rolled
*******************************************************************


add score system
add level screen
Add enemy AI with raycasts
IN GAME LEVEL EDITOR WITH A FIXED GRID
ADD SOUND
add particle system

ROTATE ENTITY ON GRID
  colliding bullets, enemies 

SCORE
LEVEL SCREEN
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



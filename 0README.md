KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

*******************************************************************
**  CREATING TIMING MANAGER CLASS TO MAKE TIMING EASIER
**  TIMING AND LOGGING - REFACTOR AND CLEAN UP OUTPUT
**  
**  ROOM FOR OPTIMIZATION
**    reduce draw calls using SFML vertexarray
**    make logfile async? - not sure if this would help
**    set O2 level and possibly switch STL algorithms for EASTL or hand-rolled
*******************************************************************



DOTPRODUCTS
ROTATE ENTITY ON GRID
  colliding bullets, entities
ADD A LERP FUNCTION

Add enemy AI with raycasts
ADD PLAYER HEALTH WITH A HEALTHBAR
ADD FADEOUT AND TIMER TO DYING ENEMIES
ADD MORE CONSISTENT COLLISIONS
IN GAME LEVEL EDITOR WITH A FIXED GRID

WALLS
  have a wall building functions
  give walls a self-calculated reflection angle 

READ THROUGH 3 OTHER GAME CODES
READ THROUGH MY CODE
READ THROUGH SFML API AGAIN

separately test a quadtree from codinggame video

WRITE AND TEST AI SYSTEM
NEED LEVELS
ADD SOUND


SETUP CMakeLists.txt to copy the debug dlls 

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



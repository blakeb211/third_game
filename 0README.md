KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want


DOTPRODUCTS
ROTATE ENTITY ON GRID
  colliding bullets, entities
ADD A LERP FUNCTION
FUNCTION TO MOVE ENEMIES OUT OF HITBOX OVERLAP FOR EASIER INITIAL PLACEMENT

REFACTOR - GROUP RELATED METHODS 
CREATING TIMING MANAGER CLASS TO MAKE TIMING EASIER
ADD PLAYER HEALTH WITH A HEALTHBAR
ADD FADEOUT AND TIMER TO DYING ENEMIES
ADD MORE CONSISTENT COLLISIONS
IN GAME LEVEL EDITOR
Add enemy AI with raycasts

WALLS
  have a wall building functions
  give walls a self-calculated reflection angle 

READ THROUGH 3 OTHER GAME CODES
READ THROUGH MY CODE
READ THROUGH SFML API AGAIN

add a quadtree from codinggame video

LOOK INTO QUAD TREES
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
    add a "free voxels" vector that broken off pixels can move to with a timer for their lifetime
    add hit boxes and only check against those -> if those intersect then do voxel-by-voxel checking

NOTES:
  use lldb to debug 



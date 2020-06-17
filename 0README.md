KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

DRAW HITBOXES
KILL BULLETS
REMOVE DEAD ENTITIES

add a quadtree from codinggame video
update cmake and start using it to build

READ THROUGH 3 OTHER GAME CODES
READ THROUGH MY CODE
READ THROUGH SFML API AGAIN


HAVE TO USE AXIS ALIGNED COLLISION DETECTION
LOOK INTO QUAD TREES
WRITE AND TEST COLLISION SYSTEM
WRITE AND TEST AI SYSTEM
WRITE AND TEST FREE PIXELS 
ADD ENEMY EXPLOSIONS
NEED LEVELS
IMPROVE BULLET ROTATION ON BOUNCE
ADD SOUND


SETUP CMakeLists.txt to copy the debug dlls 

Study Materials
  read through sfml game project code
  read through sfml api
  2nd half of Moo book


Coding Guidelines 
  use a lerp fxn
  use pure virtual functions?

Directory Org
  include directory for headers
  src directory for cpp

  Engine changes
    add a "free voxels" vector that broken off pixels can move to with a timer for their lifetime
    add hit boxes and only check against those -> if those intersect then do voxel-by-voxel checking
    add enemy AI with raycasts
    enemies die when certain amount of voxels are gone

NOTES:
  use lldb to debug 



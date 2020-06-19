KEEP IT SIMPLE STUPID

Purpose of project: Re-Write Second Shooter to run faster and have the explosion
                    and enemy AI effects that I want

<<<<<<< HEAD

ADD CMakeFiles\Makefile2 folder and files to github repository

IMPROVE WALL COLLISIONS INCLUDING ROTATE BULLET FRAG BY FRAG
INCLUDE DOTPRODUCTS
=======
When you dereference a shared_ptr and pass it into a function --> what happens to the ref count?
Free Frags array gets deleted
Debug pixels going through walls
add a timer class for the collision section to use in unnamed scopes

>>>>>>> update readme
Bullets RESPOND TO DYING by exploding
ENEMIES EXPLODE

ADD COLLISION MANAGER AND MAKE WORK WITH BULLETS
COLOR FRONT PIXELS OF BULLET1

WALLS
  have a wall building functions
  give walls a self-calculated reflection angle that is applied to things that hit it also only one voxel-voxel
  collision at a time
  need a dot product function

READ THROUGH 3 OTHER GAME CODES
READ THROUGH MY CODE
READ THROUGH SFML API AGAIN

add a quadtree from codinggame video

Note that FTSTEP doesn't need to be passed to the udpate methods
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



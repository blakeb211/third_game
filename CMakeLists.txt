cmake_minimum_required(VERSION 3.17.0)
project (game)
add_executable(game collision_manager.cpp voxel.cpp builder.cpp entity.cpp proto1.cpp)
#include_directories(c:/SFML/include)
#link_directories(c:/SFML/lib)
#set the C++17 standard
set(CMAKE_CXX_STANDARD 17)
SET(CLANG_COMPILE_FLAGS "-v -fuse-ld=lld -std=c++17 -Ic:/SFML/include")
SET(CLANG_LINK_FLAGS "-Lc:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system")
#append compile flags
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CLANG_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${CLANG_LINK_FLAGS}")

#copy sfml dlls if different
add_custom_command(TARGET game POST_BUILD        # Adds a post-build event to game 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/sfml-graphics-2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:game>)                 # <--this is out-file path

add_custom_command(TARGET game POST_BUILD        # Adds a post-build event to game 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/sfml-window-2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:game>)                 # <--this is out-file path

add_custom_command(TARGET game POST_BUILD        # Adds a post-build event to game 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/sfml-system-2.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:game>)                 # <--this is out-file path

add_custom_command(TARGET game POST_BUILD        # Adds a post-build event to game 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/level1_data.txt"      # <--this is in-file
        $<TARGET_FILE_DIR:game>)                 # <--this is out-file path

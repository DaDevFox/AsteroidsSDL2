cmake_minimum_required(VERSION 3.5)

project(AsteroidsSDL)

set (CMAKE_CXX_STANDARD 23)

file(GLOB asteroids_SRC
     "*.h"
     "*.cpp"
     "math/*.h"
     "math/*.cpp"
     "entities/*.h"
     "entities/*.cpp"
     "window/*.h"
     "window/*.cpp")

add_executable(${PROJECT_NAME} ${asteroids_SRC})

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

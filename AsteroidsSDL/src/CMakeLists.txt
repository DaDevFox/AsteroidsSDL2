cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH /cmake)
project(AsteroidsSDL)

set (CMAKE_CXX_STANDARD 23)

file(GLOB asteroids_SRC
     "*.h"
     "*.cpp"
     "math/*.h"
     "math/*.cpp"
     "entities/env/*.h"
     "entities/env/*.cpp"
     "entities/*.h"
     "entities/*.cpp"
     "window/*.h"
     "window/*.cpp")

add_executable(${PROJECT_NAME} ${asteroids_SRC})

INCLUDE(FindPkgConfig)

if(APPLE)
    PKG_CONFIG_PATH = $PKG_CONFIG_PATH:/usr/local/Cellar/
endif()

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

if(UNIX)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
    PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
elseif(APPLE)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED sdl2_image>=2.0.0)
    PKG_SEARCH_MODULE(SDL2TTF REQUIRED sdl2_ttf>=2.0.0)
endif()



INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

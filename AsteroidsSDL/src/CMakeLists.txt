cmake_minimum_required(VERSION 3.5)
project(AsteroidsSDL)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
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
    find_package(SDL2 REQUIRED)
    
    find_path(SDL2_INCLUDE_DIR SDL.h
      HINTS
        ENV SDL2DIR
        ${SDL2_DIR}
        "/usr/local/Cellar/sdl2_image/2.30.2"
      PATH_SUFFIXES SDL2
                    # path suffixes to search inside ENV{SDL2DIR}
                    include/SDL2 include)

    find_path(SDL2_IMAGE_INCLUDE_DIR SDL_image.h
      HINTS
        ENV SDL2IMAGEDIR
        ENV SDL2DIR
        ${SDL2_DIR}
        "/usr/local/Cellar/sdl2_image/2.8.2_1"
      PATH_SUFFIXES SDL2
                    # path suffixes to search inside ENV{SDL2DIR}
                    include/SDL2 include)

        
    find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
      HINTS
        ENV SDL2TTFDIR
        ENV SDL2DIR
        ${SDL2_DIR}
        "/usr/local/Cellar/sdl2_ttf/2.22.0"
      PATH_SUFFIXES SDL2
                    # path suffixes to search inside ENV{SDL2DIR}
                    include/SDL2 include)
endif()

#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

if(UNIX)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
    PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
elseif(APPLE)
    #PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED sdl2_image>=2.0.0)
    #PKG_SEARCH_MODULE(SDL2TTF REQUIRED sdl2_ttf>=2.0.0)
endif()



INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

cmake_minimum_required(VERSION 3.5)
project(AsteroidsSDL)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
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


if(APPLE)
#    find_library(SDL2 SDL2)
#    find_library(SDL2IMAGE SDL2_image)
#    find_library(SDL2TTF sdl2_ttf)

    find_package(SDL2 REQUIRED)
    find_package(SDL2IMAGE REQUIRED)
    find_package(SDL2TTF REQUIRED)
    add_executable(AsteroidsSDL ${asteroids_SRC})
    include_directories(AsteroidsSDL ${SDL2_INCLUDE_DIR})
    include_directories(${SDL2TTF_INCLUDE_DIR})
    include_directories(${SDL2IMAGE_INCLUDE_DIR})
    target_link_libraries(AsteroidsSDL ${SDL2_LIBRARY})
    target_link_libraries(AsteroidsSDL ${SDL2TTF_LIBRARY})
    target_link_libraries(AsteroidsSDL ${SDL2IMAGE_LIBRARY})

#    target_link_libraries(${PROJECT_NAME} "-framework SDL2")
#    target_link_libraries(${PROJECT_NAME} "-framework SDL2_image")
#    target_link_libraries(${PROJECT_NAME} "-framework SDL2_ttf")
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")
else()
    add_executable(${PROJECT_NAME} ${asteroids_SRC})

    INCLUDE(FindPkgConfig)

    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
    PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
endif()


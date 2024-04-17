cmake_minimum_required(VERSION 2.8.4)
set(PROJECT_NAME, AsteroidsSDL)
project(${PROJECT_NAME})

find_package(SDL2 REQUIRED)

find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
find_library(SDL2_LIBRARY NAME SDL2)
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIR})
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
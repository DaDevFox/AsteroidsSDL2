cmake_minimum_required(VERSION 2.8.4)
project(AsteroidsSDL)

set(SDL2_INCLUDE_DIR C:/SDL/SDL2-2.0.3/include)
set(SDL2_LIBRARY C:/SDL/SDL2-2.0.3/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
set(SOURCE_FILES main.cpp)

add_executable(AsteroidsSDL ${SOURCE_FILES})
target_link_libraries(AsteroidsSDL ${SDL2_LIBRARY})

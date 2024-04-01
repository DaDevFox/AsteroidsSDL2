#pragma once
#include "window/RenderWindow.h"
#include "settings.h"

#define WIDTH 680
#define HEIGHT 400

extern int PLAYER_initial_outline_point_count;
extern const int GAME_min_outline_point_count;
extern const int PLAYER_asteroid_id;
extern const int PLAYER_entity_id;

extern int GAME_width;
extern int GAME_height;
extern SDL_Rect GAME_window_rect;

extern const int chunk_size;
extern const int GAME_chunkwise_height;
extern const int GAME_chunkwise_width;

extern int WINDOW_width;
extern int WINDOW_height;

extern TTF_Font* encode_sans_medium;
extern TTF_Font* encode_sans_bold;

extern void* entities;
extern RenderWindow window;
extern float unscaled_delta_time;
extern float delta_time;
extern float time_scaling;

extern bool game_running;

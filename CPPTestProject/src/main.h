#pragma once
#include "window/RenderWindow.h"
#include "settings.h"

#define WIDTH 680
#define HEIGHT 400

extern int GAME_width;
extern int GAME_height;
extern SDL_Rect GAME_window_rect;

extern int WINDOW_width;
extern int WINDOW_height;

extern RenderWindow window;
extern float delta_time;
extern bool game_running;

extern TTF_Font* encode_sans_medium;
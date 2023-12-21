#pragma once
#include <SDL.h>
#include "Entity.h"


extern RectEntity player;

void player_init();
void player_input_update(SDL_Event *running_event);
void player_render_update(RenderWindow* window);
void player_update();
void player_cleanup();


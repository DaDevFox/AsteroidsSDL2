#pragma once
#include <SDL.h>
#include "Entity.h"

extern RectEntity player;

void player_init();
void player_update(SDL_Event *event);
void player_cleanup();


#pragma once
#include <SDL.h>
#include "Entity.h"


extern const int asteroids_count;
extern RectEntity* asteroids;
extern RectEntity player;

void asteroids_init();
void asteroids_input_update(SDL_Event *running_event);
void asteroids_render_update(RenderWindow* window);
void asteroids_update(float delta_time);
void asteroids_cleanup();


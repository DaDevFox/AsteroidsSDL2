#pragma once
#include <SDL.h>
#include "Entity.h"


extern int max_asteroid_radius;

extern const int asteroids_count;
extern Asteroid* asteroids;
extern Asteroid player;

void asteroids_init();
void asteroids_input_update(SDL_Event *running_event);
void asteroids_render_update(RenderWindow* window);
void asteroids_update(float delta_time);
void asteroids_cleanup();


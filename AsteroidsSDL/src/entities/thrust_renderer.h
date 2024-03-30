#pragma once
#include <SDL.h>
#include "../main.h"
#include "Entity.h"

void render_thrust(RenderWindow* window, Entity* entity, const SDL_Color& color, int min_height = 2, int max_height = 6, float random_tick = 6.0F, bool central = false);

void thrust_renderer_cleanup();
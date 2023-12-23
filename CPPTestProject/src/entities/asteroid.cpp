#include "asteroid.h"
#include "../main.h"
#include <math.h>
#include <stdlib.h>

int max_asteroid_radius = 256/2;

int asteroid_variance = 250;

const char* asteroid_texture_path = "./circle.png";
Asteroid player(asteroid_texture_path, 32, 32);

Asteroid *asteroids;
const int asteroids_count = 20;

float speed = 1.0F;

int downup = 0;
int leftright = 0;

bool primary_held;

int mouse_diff_threshold_squared = 125;

void asteroids_init() 
{
	asteroids = (Asteroid*)calloc(asteroids_count, sizeof(Asteroid));
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->init(asteroid_texture_path, 32, 32);
		asteroid->x = GAME_width / 2.0F + (float)(rand() % asteroid_variance);
		asteroid->y = GAME_height / 2.0F + (float)(rand() % asteroid_variance);
	}
}

void asteroids_input_update(SDL_Event *running_event) 
{
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		if (running_event->type == SDL_MOUSEBUTTONDOWN && running_event->button.button == SETTING_primary_mouse_button)
		{
			primary_held = true;
		}

		if (running_event->type == SDL_MOUSEBUTTONUP && running_event->button.button == SETTING_primary_mouse_button)
		{
			primary_held = false;
		}

		int x_diff_to_mouse = window.camera.screen_to_world_x(running_event->motion.x) + asteroid->target_offset_x - asteroid->screen_x;
		int y_diff_to_mouse = window.camera.screen_to_world_y(running_event->motion.y) + asteroid->target_offset_y - asteroid->screen_y;

		// mouse based rotation always
		if (running_event->type == SDL_MOUSEMOTION) 
			asteroid->angle = atan2((double)y_diff_to_mouse, (double)x_diff_to_mouse);
	}
}

void asteroids_render_update(RenderWindow *window)
{
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->render(window);
	}
}

void asteroids_update(float delta_time) 
{
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		if (primary_held)
		{
			int mouse_x;
			int mouse_y;

			SDL_GetMouseState(&mouse_x, &mouse_y);

			int x_diff_to_mouse = window.camera.screen_to_world_x(mouse_x) + asteroid->target_offset_x - asteroid->screen_x;
			int y_diff_to_mouse = window.camera.screen_to_world_y(mouse_y) + asteroid->target_offset_y - asteroid->screen_y;

			float magnitude_squared = x_diff_to_mouse * x_diff_to_mouse + y_diff_to_mouse * y_diff_to_mouse;
			if (magnitude_squared >= mouse_diff_threshold_squared) 
			{
				float step_magnitude = SDL_clamp(magnitude_squared, 0, speed);

				asteroid->angle = atan2((double)y_diff_to_mouse, (double)x_diff_to_mouse);
				asteroid->desired_velocity_x = cos(asteroid->angle) * step_magnitude;
				asteroid->desired_velocity_y = sin(asteroid->angle) * step_magnitude;

				
				asteroid->drag_enabled = false;
			}
		}
		else
			asteroid->drag_enabled = true;

		asteroid->update();
	}
}

void asteroids_cleanup() 
{
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->cleanup();
	}
	free(asteroids);
}

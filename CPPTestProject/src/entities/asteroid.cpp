#include "asteroid.h"
#include "../main.h"
#include <math.h>
#include <stdlib.h>

int asteroid_variance = 250;

const char* asteroid_texture_path = "./circle.png";
RectEntity player(asteroid_texture_path, 32, 32);

RectEntity *asteroids;
const int asteroids_count = 20;

float speed = 1.0F;

int downup = 0;
int leftright = 0;

int towards_mouse = 0;
int mouse_diff_threshold_squared = 125;

void asteroids_init() 
{
	asteroids = (RectEntity*)calloc(asteroids_count, sizeof(RectEntity));
	for (RectEntity* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->init(asteroid_texture_path, 32, 32);
		asteroid->x = GAME_width / 2.0F + (float)(rand() % asteroid_variance);
		asteroid->y = GAME_height / 2.0F + (float)(rand() % asteroid_variance);
	}
}

void asteroids_input_update(SDL_Event *running_event) 
{
	for (RectEntity* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		int x_diff_to_mouse = window.camera.screen_to_world_x(running_event->motion.x) - asteroid->screen_x;
		int y_diff_to_mouse = window.camera.screen_to_world_y(running_event->motion.y) - asteroid->screen_y;
		
		// mouse based movement
		float magnitude_squared = x_diff_to_mouse * x_diff_to_mouse + y_diff_to_mouse * y_diff_to_mouse;
		if (magnitude_squared >= mouse_diff_threshold_squared)
		{
			if (running_event->type == SDL_MOUSEBUTTONDOWN && running_event->button.button == SETTING_primary_mouse_button)
			{
				towards_mouse = 1;
			}
			if (running_event->type == SDL_MOUSEBUTTONUP && running_event->button.button == SETTING_primary_mouse_button)
			{
				towards_mouse = 0;
			}
		}


		// mouse based rotation always
		if (running_event->type == SDL_MOUSEMOTION) 
			asteroid->angle = atan2((double)y_diff_to_mouse, (double)x_diff_to_mouse);
		


		float step_magnitude = SDL_clamp(magnitude_squared * towards_mouse, 0, speed);

		asteroid->desired_velocity_x = cos(asteroid->angle) * step_magnitude;
		asteroid->desired_velocity_y = sin(asteroid->angle) * step_magnitude;
		
		//SDL_Log("asteroid moving: %i", towards_mouse);

		//if (abs((int)asteroid->desired_velocity_x) > 0 || fabs(asteroid->desired_velocity_y) > 0)

		//if (SETTING_keyboard_player_movement)
		//{
		//	player.desired_velocity_x = 0.0F;
		//	player.desired_velocity_y = 0.0F;
		//	if (running_event->type == SDL_KEYDOWN)
		//	{
		//		//const Uint8* keystates = SDL_GetKeyboardState(NULL);
		//		//if (keystates[SDL_SCANCODE_RIGHT])
		//		//	player.desired_velocity_x = speed;
		//		//if (keystates[SDL_SCANCODE_LEFT])
		//		//	player.desired_velocity_x = -speed;
		//		//if (keystates[SDL_SCANCODE_UP])
		//		//	player.desired_velocity_y = -speed;
		//		//if (keystates[SDL_SCANCODE_DOWN])
		//		//	player.desired_velocity_y = speed;
		//
		//		if (running_event->key.keysym.sym == SDLK_DOWN)
		//			downup = -1;
		//		if (running_event->key.keysym.sym == SDLK_UP)
		//			downup = 1;
		//		if (running_event->key.keysym.sym == SDLK_LEFT)
		//			leftright = -1;
		//		if (running_event->key.keysym.sym == SDLK_RIGHT)
		//			leftright = 1;
		//	}
		//	if (running_event->type == SDL_KEYUP)
		//	{
		//		if (running_event->key.keysym.sym == SDLK_DOWN)
		//			downup = 0;
		//		if (running_event->key.keysym.sym == SDLK_UP)
		//			downup = 0;
		//		if (running_event->key.keysym.sym == SDLK_LEFT)
		//			leftright = 0;
		//		if (running_event->key.keysym.sym == SDLK_RIGHT)
		//			leftright = 0;
		//	}
		//	player.desired_velocity_x = leftright * speed;
		//	player.desired_velocity_y = -downup * speed;
		//}
		//else
		//{
		//}
	}
}

void asteroids_render_update(RenderWindow *window)
{
	for (RectEntity* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->render(window);
	}
}

void asteroids_update(float delta_time) 
{
	for (RectEntity* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->update();
	}
}

void asteroids_cleanup() 
{
	for (RectEntity* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->cleanup();
	}
	free(asteroids);
}

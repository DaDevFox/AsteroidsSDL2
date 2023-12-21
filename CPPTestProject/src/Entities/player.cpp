#include "player.h"
#include "../main.h"
#include <math.h>

const char* player_texture_name = "./circle.png";
RectEntity player(player_texture_name, 32, 32);

float speed = 0.10F;

int downup = 0;
int leftright = 0;

int towards_mouse = 0;
int mouse_diff_threshold_squared = 125;

void player_init() 
{
	player.init();
	player.x = 680.0F / 2.0F;
	player.y = 400.0F / 2.0F;
}

void player_input_update(SDL_Event *running_event) 
{
	int x_diff_to_mouse = running_event->motion.x - player.screen_x;
	int y_diff_to_mouse = running_event->motion.y - player.screen_y;

	// mouse based rotation always
	if (running_event->type == SDL_MOUSEMOTION) 
		player.angle = atan2((double)y_diff_to_mouse, (double)x_diff_to_mouse);

	if (SETTING_keyboard_player_movement) 
	{
		player.desired_velocity_x = 0.0F;
		player.desired_velocity_y = 0.0F;
		if (running_event->type == SDL_KEYDOWN)
		{
			//const Uint8* keystates = SDL_GetKeyboardState(NULL);
			//if (keystates[SDL_SCANCODE_RIGHT])
			//	player.desired_velocity_x = speed;
			//if (keystates[SDL_SCANCODE_LEFT])
			//	player.desired_velocity_x = -speed;
			//if (keystates[SDL_SCANCODE_UP])
			//	player.desired_velocity_y = -speed;
			//if (keystates[SDL_SCANCODE_DOWN])
			//	player.desired_velocity_y = speed;

			if (running_event->key.keysym.sym == SDLK_DOWN)
				downup = -1;
			if (running_event->key.keysym.sym == SDLK_UP)
				downup = 1;
			if (running_event->key.keysym.sym == SDLK_LEFT)
				leftright = -1;
			if (running_event->key.keysym.sym == SDLK_RIGHT)
				leftright = 1;
		}

		if (running_event->type == SDL_KEYUP)
		{
			if (running_event->key.keysym.sym == SDLK_DOWN)
				downup = 0;
			if (running_event->key.keysym.sym == SDLK_UP)
				downup = 0;
			if (running_event->key.keysym.sym == SDLK_LEFT)
				leftright = 0;
			if (running_event->key.keysym.sym == SDLK_RIGHT)
				leftright = 0;
		}

		player.desired_velocity_x = leftright * speed;
		player.desired_velocity_y = -downup * speed;
	}
	else 
	{
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

		// FLAG: clamp magnitude would be ideal; differential resultant speed values depending on edge cases rn

		float step_magnitude = SDL_clamp(magnitude_squared * towards_mouse, 0, speed);

		player.desired_velocity_x = cos(player.angle) * step_magnitude;
		player.desired_velocity_y = sin(player.angle) * step_magnitude;
	}

}

void player_render_update(RenderWindow *window)
{
	player.render(window);
}

void player_update() {
	player.update();
}

void player_cleanup() 
{
	player.cleanup();
}

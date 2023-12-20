#include "player.h"
#include "../main.h"
#include <math.h>

const char* player_texture_name = "./circle.png";
RectEntity player(player_texture_name, 32, 32);

void player_init() 
{
	player.init();
	player.x = 680 / 2;
	player.y = 400 / 2;
}

void player_input_update(SDL_Event *running_event) 
{
	if (running_event->type == SDL_KEYDOWN)
	{
		if (running_event->key.keysym.sym == SDLK_RIGHT)
			player.x += 1;
		if (running_event->key.keysym.sym == SDLK_LEFT)
			player.x -= 1;
		if (running_event->key.keysym.sym == SDLK_UP)
			player.y -= 1;
		if (running_event->key.keysym.sym == SDLK_DOWN)
			player.y += 1;
	}
	if (running_event->type == SDL_MOUSEMOTION) 
	{
		int x_diff = running_event->motion.x - player.x;
		int y_diff = running_event->motion.y - player.y;

		player.angle = atan2((double)y_diff, (double)x_diff);
	}


}

void player_render_update(RenderWindow *window)
{
	player.render(window);
}

void player_cleanup() 
{
	player.cleanup();
}

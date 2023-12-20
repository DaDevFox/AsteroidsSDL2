#include "player.h"
#include "main.h"

const char* player_texture_name = "./circle.png";
RectEntity player(player_texture_name, 32, 32);

void player_init() 
{
	player.init();
	player.x = 680 / 2;
	player.y = 400 / 2;
}

void player_update(SDL_Event *event) 
{
	if (event->type == SDL_KEYDOWN) 
	{
		if (event->key.keysym.sym == SDLK_RIGHT)
			player.x += 1;
		if (event->key.keysym.sym == SDLK_LEFT)
			player.x -= 1;
		if (event->key.keysym.sym == SDLK_UP)
			player.y -= 1;
		if (event->key.keysym.sym == SDLK_DOWN)
			player.y += 1;
	}

	player.render(&window);
}

void player_cleanup() 
{
	player.cleanup();
}

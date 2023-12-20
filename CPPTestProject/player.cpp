#include "player.h"

int input_update(SDL_Event *event) {
	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_RIGHT)
			player_rect->x += 1;
		if (e.key.keysym.sym == SDLK_LEFT)
			player_rect->x -= 1;
		if (e.key.keysym.sym == SDLK_UP)
			player_rect->y -= 1;
		if (e.key.keysym.sym == SDLK_DOWN)
			player_rect->y += 1;
	}
}



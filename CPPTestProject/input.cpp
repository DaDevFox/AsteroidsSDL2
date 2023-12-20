#include "input.h"

int INPUT_frame_update(SDL_Event event) {
	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_RIGHT)
			player_rect.x += speed;
		if (e.key.keysym.sym == SDLK_LEFT)
			player_rect.x -= speed;
		if (e.key.keysym.sym == SDLK_UP)
			player_rect.y -= speed;
		if (e.key.keysym.sym == SDLK_DOWN)
			player_rect.y += speed;
	}
}



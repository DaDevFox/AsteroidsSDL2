#include <SDL.h>
#include <SDL_image.h>
#include "game.h"

#include "player.h"
#include "RenderWindow.h"
	
#undef main // needed for cpp compilation

#define WIDTH 680
#define HEIGHT 400

const char* window_title = "Asteroids";

bool game_running;

int texture_width = 128;
int texture_height = 128;

// window frame
RenderWindow window(window_title, WIDTH, HEIGHT);

// resources
const char *player_texture_name = "./circle.png";
SDL_Texture player_texture;

int game_init() {
	game_running = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		SDL_Log("Error on SDL_Init: %s\n", SDL_GetError());
		return 0;
	}

	return 1;
}

void draw_update() {
	window.draw();
}

void game_loop(SDL_Event *running_event) 
{
	while (SDL_PollEvent(running_event))
	{
		if (running_event->type == SDL_QUIT)
			game_running = false;

		input_update(running_event);
		draw_update();

		SDL_Delay(10);
	}
}

int main() {
	if (!game_init())
		exit(1);

	SDL_Event running_event;

	window.init();
	while (game_running) 
	{
		game_loop(&running_event);
	}

	IMG_Quit();
	SDL_Quit();

	return 0;
}
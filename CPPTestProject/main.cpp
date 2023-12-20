#include <SDL.h>
#include <SDL_image.h>

#include "main.h"
#include "player.h"
#include "RenderWindow.h"
	
#undef main // needed for cpp compilation

const char* window_title = "Asteroids";
RenderWindow window(window_title, WIDTH, HEIGHT);
bool game_running;

/// <summary>
/// Inits SDL2 and required systems. returns false if failure occurs
/// </summary>
/// <returns></returns>
bool game_init() {
	game_running = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
	{
		SDL_Log("Error on SDL_Init: %s\n", SDL_GetError());
		return false;
	}
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) 
	{
		SDL_Log("Error on IMG_Init: %s\n", SDL_GetError());
		return false;
	}
	if (TTF_Init() < 0) 
	{
		SDL_Log("Error on TTF_Init: %s\n", SDL_GetError());
		return false;
	}

	player_init();

	return true;
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
	}


	player_update(running_event);
	draw_update();

	window.render_line(0, 0, 400, 400, 255, 255, 255, 255);

	SDL_Delay(10);
}

void game_cleanup() {

	player_cleanup();
	IMG_Quit();
	SDL_Quit();
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

	game_cleanup();

	return 0;
}
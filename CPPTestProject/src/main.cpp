#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <any>
#include "main.h"
#include "Entities/player.h"

#undef main // needed for cpp compilation

const char* window_title = "Asteroids";
RenderWindow window(window_title, WIDTH, HEIGHT);

bool game_running;

float delta_time;

Uint32 last_tick;
Uint32 current_tick;

SDL_Event running_event;

// resources
TTF_Font *encode_sans_medium;

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

	encode_sans_medium = TTF_OpenFont("./EncodeSans-Medium.ttf", 18);
	if (encode_sans_medium == NULL)
	{
		SDL_Log("Error loading encode sans medium font: %s\n", SDL_GetError());
		return false;
	}

	player_init();

	return true;
}

void input_update() {
	player_input_update(&running_event);
}

void render_update() {
	player_render_update(&window);

	SDL_Color color = 
	{
		255, 
		255,
		255,
		255
	};


	char output[7];
	if (abs(player.velocity_x) > 0 || abs(player.velocity_y) > 0)
		strcpy_s(output, 7, "moving");
	else
		strcpy_s(output, 7,  "   ");


	window.render_centered(WIDTH / 2.0F, 50.0F, output, encode_sans_medium, color);
}

void update() {
	current_tick = SDL_GetTicks();
	delta_time = (float)((current_tick - last_tick));

	player_update();

	last_tick = current_tick;
}


void game_loop() 
{
	while (SDL_PollEvent(&running_event))
	{
		if (running_event.type == SDL_QUIT)
			game_running = false;
		
		input_update();
	}

	window.clear();
	render_update();
	window.draw();

	update();
}

void game_cleanup() {

	player_cleanup();
	IMG_Quit();
	SDL_Quit();
}

int main() {
	if (!game_init())
		exit(1);

	

	window.init();
	while (game_running) 
	{
		game_loop();
	}

	game_cleanup();

	return 0;
}
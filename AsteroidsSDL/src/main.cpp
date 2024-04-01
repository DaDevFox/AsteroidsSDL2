#include <iostream>
#include <string>

#include <SDL_image.h>
#include <SDL_ttf.h>

#include "main.h"
#include "entities/asteroid.h"
#include "entities/ship.h"

#undef main // needed for cpp compilation

const int PLAYER_asteroid_id = 0;
const int PLAYER_entity_id = GAME_ship_count + PLAYER_asteroid_id;

const int chunk_size = ASTEROID_maximum_radius;
const int GAME_chunkwise_height = GAME_height / chunk_size;
const int GAME_chunkwise_width = GAME_width / chunk_size;
const int GAME_min_outline_point_count = 100;
int PLAYER_initial_outline_point_count = 0;

int WINDOW_height;
int WINDOW_width;

int GAME_height = 1024;
int GAME_width = 1024;

SDL_Rect GAME_window_rect = { 0, 0, GAME_width, GAME_height };

void* entities;

const char* window_title = "Asteroids";
RenderWindow window(window_title, INITIAL_WIDTH, INITIAL_HEIGHT);

bool game_running;

float delta_time;
float unscaled_delta_time;
float time_scaling = 1.0F;

Uint32 last_tick;
Uint32 current_tick;

SDL_Event running_event;

SDL_Color game_frame_color{ 120, 120, 120, 255 };

// resources
TTF_Font* encode_sans_medium;
TTF_Font* encode_sans_bold;

void resize(void);

/// <summary>
/// Inits SDL2 and required systems. returns false if failure occurs
/// </summary>
/// <returns></returns>
bool game_init()
{
	game_running = true;
	srand(12);

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

	resize();

	window.init();
	window.camera.teleport(GAME_width / 2.0F, GAME_height / 2.0F);

	encode_sans_medium = TTF_OpenFont("./EncodeSans-Medium.ttf", 18);
	if (encode_sans_medium == nullptr)
	{
		SDL_Log("Error loading encode sans medium font: %s\n", SDL_GetError());
		return false;
	}

	encode_sans_bold = TTF_OpenFont("./EncodeSans-Bold.ttf", 18);
	if (encode_sans_bold == nullptr)
	{
		SDL_Log("Error loading encode sans bold font: %s\n", SDL_GetError());
		return false;
	}

	entities_init();

	return true;
}

void window_update()
{
	switch (running_event.window.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
		resize();
		break;
	}
}

void resize()
{
	window.get_info(&WINDOW_width, &WINDOW_height);
}

void input_update()
{
	player_input_update(&running_event);
	window.ui.input_update(&running_event);
	window.camera.input_update(&running_event);
}

void render_game_frame()
{
	window.render_line(0, 0, GAME_width, 0, game_frame_color);
	window.render_line(GAME_width, 0, GAME_width, GAME_height, game_frame_color);
	window.render_line(GAME_width, GAME_height, 0, GAME_height, game_frame_color);
	window.render_line(0, GAME_height, 0, 0, game_frame_color);
}

void render_update()
{
	render_game_frame();

	ships_render_update(&window);
	asteroids_render_update(&window);
	window.ui.render_update(&window);

	if (DEBUG_mode)
		window.render_all_deferred();
}

void update()
{
	current_tick = SDL_GetTicks();
	unscaled_delta_time = (float)((current_tick - last_tick));
	delta_time = unscaled_delta_time * time_scaling;


	window.camera.update(delta_time);
	ships_update(delta_time);
	asteroids_update(delta_time);


	last_tick = current_tick;
}


void game_loop()
{
	//printf("%i", ((Entity*)entities)[32].outline_point_count);
	while (SDL_PollEvent(&running_event))
	{
		if (running_event.type == SDL_QUIT)
			game_running = false;

		if (running_event.type == SDL_WINDOWEVENT)
			window_update();

		input_update();
	}

	window.clear();
	render_update();
	window.draw();

	update();
}

void game_cleanup()
{

	entities_cleanup();
	IMG_Quit();
	SDL_Quit();
}

int main()
{
	if (!game_init())
		exit(1);

	while (game_running)
	{
		game_loop();
	}

	game_cleanup();

	return 0;
}
#include <SDL.h>
#include <SDL_image.h>
#include "game.h"
#include "input.h"
	
#undef main // needed for cpp compilation

#define WIDTH 680
#define HEIGHT 400

int texture_width = 128;
int texture_height = 128;
const char *player_texture_name = "./circle.png";

SDL_Renderer* renderer = nullptr;
SDL_Window* window = NULL;


void game_init() {
	game_running = true;
}

void game_loop() {

}

int main() {
	game_init();



	SDL_Texture* player_texture = NULL;

	int speed = 3;
	float deltaTime = 0.0f;

	SDL_Rect player_rect = {
		WIDTH/2,
		HEIGHT/2,
		texture_width,
		texture_height
	};
	const SDL_Rect* player_rect_ptr = &player_rect;

	SDL_Event e;

	if (init(&window, &renderer, &player_texture) < 0) 
	{
		SDL_Log("error initializing");
		return 1;
	}


	while (game_running) 
	{
		game_loop();
		while(SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				game_running = false;
			
			INPUT_frame_update(e);
			
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderCopy(renderer, player_texture, NULL, player_rect_ptr);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(player_texture);
	IMG_Quit();
	SDL_Quit();

	return 0;
}

int init(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **player_texture) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
	{
		SDL_Log("SDL Error on init: %s\n", SDL_GetError());
		return -1;
	}

	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP, window, renderer) < 0)
	{
		SDL_Log("SDL Error on create window and renderer: %s\n", SDL_GetError());
		return -1;
	}
	if (window == NULL)
	{
		SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return -1;
	}
	if (renderer == NULL)
	{
		SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return -1;
	}


	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) 
	{
		SDL_Log("Error on IMG_Init; IMG_INIT_PNG flag not initialized: %s\n", SDL_GetError());
		return -1;
	}	

	return init_textures(*renderer, player_texture);
}

int init_textures(SDL_Renderer *renderer, SDL_Texture **player_texture) {
	*player_texture = IMG_LoadTexture(renderer, player_texture_name);

	if (*player_texture == NULL) 
	{
		SDL_Log("Error loading texture %s: %s\n", player_texture_name, SDL_GetError());
		return -1;
	}

	return 0;
}
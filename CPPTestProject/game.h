#include <SDL.h>

bool game_running;

int init(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** player_texture);
int init_textures(SDL_Renderer* renderer, SDL_Texture** player_texture);
#include "ship.h"
#include "../main.h"
#include <iostream>

SDL_Texture* ship_texture;

void render_fovs();

void ships_init()
{
	ship_texture = window.load_texture(RESOURCE_ship_texture_path);

	int i = 0;

	int ship_padding = 20;
	int row_count = 4;
	int ship_size = 10;

	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		ship->texture = ship_texture;

		ship->x = (GAME_width >> 1) - ship_padding * GAME_ship_count + (i * ship_padding);
		ship->y = (GAME_height >> 1) - ship_padding * (GAME_ship_count / row_count) + (i / row_count * ship_padding);

		ship->w = ship_size;
		ship->h = ship_size;

		std::cout << "initializing ship" << std::endl;

		i++;
	}
}

void ships_update(float delta_time)
{
}

void render_fovs(RenderWindow* window)
{
	double fov = M_PI / 3.0;
	double granularity = 0.05;
	float radius = 50.0F;

	//TODO: expensive; use texture instead???

	//for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	//{
	//	for (double theta = ship->rotation - fov / 2.0; theta < ship->rotation + fov / 2.0; theta += granularity)
	//	{
	//		for (float i = 0.0F; i < radius; i++)
	//		{
	//			window->render_rect(ship->x + (ship->w >> 1) + i * cosf(theta), ship->y + (ship->h >> 1) + i * sinf(theta), 1.0F, 1.0F, { 100, 100, 100, 255 });

	//		}
	//	}
	//}
}

void ships_render_update(RenderWindow* window)
{
	render_fovs(window);

	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		ship->render(window);
	}
}


void ships_cleanup()
{
	SDL_DestroyTexture(ship_texture);
}
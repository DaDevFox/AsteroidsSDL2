#include "ship.h"
#include "../main.h"
#include <iostream>
#include <vector>

SDL_Texture* ship_texture;

void render_fovs();

std::vector<SDL_Point> search_positions;

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

		ship->x = (GAME_width >> 1) - ship_padding * GAME_ship_count + (i % row_count * ship_padding);
		ship->y = (GAME_height >> 1) - ship_padding * (GAME_ship_count / row_count) + (i / row_count * ship_padding);

		ship->w = ship_size;
		ship->h = ship_size;

		std::cout << "initializing ship" << std::endl;

		i++;
	}
}

SDL_Point default_position(int i) {
	int radius = GAME_width >> 2;
	return { (int)((float)(GAME_width >> 1) + cosf((float)i / (float)GAME_ship_count * 2.0F * PI) * (float)radius), (int)((float)(GAME_height >> 1) + sinf((float)i / (float)GAME_ship_count * 2.0F * PI) * (float)radius) };
}

void generate_search_positions()
{
	if (search_positions.size() == 0)
		for (int i = 0; i < GAME_ship_count; i++)
			search_positions.push_back(default_position(i));
}

void ships_update(float delta_time)
{
	static float timer = 0.0F;
	float search_tick = 40.0F;

	if (timer > search_tick) {
		generate_search_positions();
		timer = 0.0F;
	}

	int i = 0;
	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		if (search_positions.size() == 0)
			continue;

		SDL_Point pos = search_positions[i];
		float x = (float)pos.x - ship->x;
		float y = (float)pos.y - ship->y;
		float theta = atan2f(y, x);

		float distance = sqrtf(x * x + y * y);

		ship->desired_velocity_x = cosf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);
		ship->desired_velocity_y = sinf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);

		// TODO: asteroid collision avoidance
		ship->update();
		i++;
	}

	timer += delta_time;
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
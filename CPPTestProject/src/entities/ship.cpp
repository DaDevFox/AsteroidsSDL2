#include "ship.h"
#include "../main.h"
#include <iostream>
#include <vector>

SDL_Texture* ship_texture;

void render_fovs();
bool run_ship_avoidance(Entity* ship, float multiplier, float* vel_x, float* vel_y);
void generate_ship_outline(Entity* ship);

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

		ship->drag_enabled = false;
		ship->movement_windup_speed = 0.001F;

		generate_ship_outline(ship);

		std::cout << "ship initialized" << std::endl;

		i++;
	}
}

void generate_ship_outline(Entity* ship) {
	std::vector<SDL_Point> outline;

	// edge rows
	for (int x = 1; x < ship->w - 1; x++)
	{
		outline.push_back({ x, 0 });
		outline.push_back({ x, ship->h });
	}

	// edge columns
	for (int y = 1; y < ship->h - 1; y++)
	{
		outline.push_back({ 0, y });
		outline.push_back({ ship->w, y });
	}

	// corners
	outline.push_back({ 0, 0 });
	outline.push_back({ 0, ship->h });
	outline.push_back({ ship->w, 0 });
	outline.push_back({ ship->w, ship->h });

	if (outline.size() < 4 * SETTING_MAX_POLYGON_VERTICES)
	{
		for (int i = 0; i < outline.size(); i++)
			ship->outline[i] = outline.at(i);
		ship->outline_point_count = outline.size();
	}
	else
		SDL_Log("ERROR: outline buffer overflow for ship.\n");
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
	float search_tick = 100.0F;

	if (timer > search_tick)
	{
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

		float critical_distance = 1.0F;

		float vel_x = cosf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);
		float vel_y = sinf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);



		if (run_ship_avoidance(ship, 0.005F, &vel_x, &vel_y) || distance > critical_distance)
		{
			ship->desired_velocity_x = vel_x;
			ship->desired_velocity_y = vel_y;
		}
		else
		{
			ship->x = pos.x;
			ship->y = pos.y;
			ship->velocity_x = 0.0F;
			ship->velocity_y = 0.0F;
			ship->desired_velocity_x = 0.0F;
			ship->desired_velocity_y = 0.0F;
		}

		float crit_vel = 0.2F;

		if (distance > crit_vel)
			ship->rotation = atan2(ship->velocity_y, ship->velocity_x);
		else
			ship->rotation = 0.0;

		ship->update();
		i++;
	}

	timer += delta_time;
}

bool run_ship_avoidance(Entity* ship, float multiplier, float* vel_x, float* vel_y)
{
	bool result = false;
	std::set<int> to_check;

	float x = ship->x;
	float y = ship->y;

	float speed_per_rad_increase = 0.8F;
	int look_radius = 2 + (sqrtf(ship->velocity_x * ship->velocity_x + ship->velocity_y * ship->velocity_y) / speed_per_rad_increase);
	/*if (look_radius > 2)
		printf("lr: %i \n", look_radius);*/

	int floor_y = (int)y / tile_size - look_radius;
	int ceil_y = (int)y / tile_size + look_radius;
	int left_x = (int)x / tile_size - look_radius;
	int right_x = (int)x / tile_size + look_radius;
	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, chunk_height); curr_y++)
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, chunk_width); curr_x++)
		{
			int chunk = curr_x + (GAME_width / tile_size) * curr_y;
			if (collision_check_grid[chunk] == nullptr)
				continue;
			std::set<int> curr_set = *(collision_check_grid[chunk]);
			to_check.insert(curr_set.begin(), curr_set.end());
		}
	}

	for (int id : to_check) {
		if (id == ship->id)
			continue;


		Entity* other = Entity::active[id];
		window.render_rect(other->x, other->y, other->w, other->h, { 0, 0, 255, 255 });


		*vel_x -= (other->x - x) * multiplier;
		*vel_y -= (other->y - y) * multiplier;
		result = true;
	}

	return result;
}

void render_fovs(RenderWindow* window)
{
	double fov = M_PI / 3.0;
	double granularity = 0.0125;
	float radius = 100.0F;

	//TODO: expensive; use texture instead???

	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		for (double theta = ship->rotation - fov / 2.0; theta < ship->rotation + fov / 2.0; theta += granularity)
		{
			for (float i = 0.0F; i < radius; i++)
			{
				window->render_rect(ship->x + i * cosf(theta), ship->y + i * sinf(theta), 1.0F, 1.0F, { 100, 100, 100, 255 });
			}
		}
	}
}

void ships_render_update(RenderWindow* window)
{
	//render_fovs(window);

	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		ship->render(window);
	}
}


void ships_cleanup()
{
	SDL_DestroyTexture(ship_texture);
}
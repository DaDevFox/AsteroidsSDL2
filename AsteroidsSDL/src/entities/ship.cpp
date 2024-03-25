#include "ship.h"
#include "asteroid.h"
#include "../main.h"
#include <iostream>
#include <vector>
#include <time.h>

SDL_Texture* ship_texture;
SDL_Texture* laser_beam_texture;
SDL_Texture* highlighter_beam_texture;


void render_fovs(RenderWindow* window);
bool run_ship_avoidance(Entity* ship, float multiplier, float* vel_x, float* vel_y);
void generate_ship_outline(Entity* ship);
void search_for_targets();

void select_targets(Entity* ship);

std::vector<SDL_Point> target_positions;
std::vector<std::vector<int>*> shadowing_targets;
int* ship_targets;
float* ship_attack_timers;

void ships_init()
{
	ship_targets = new int[GAME_ship_count];
	ship_attack_timers = new float[GAME_ship_count];
	for (int i = 0; i < GAME_ship_count; i++)
	{
		ship_targets[i] = 0;
		ship_attack_timers[i] = 0.0F;
	}

	ship_texture = window.load_texture(RESOURCE_ship_texture_path);
	laser_beam_texture = window.load_texture(RESOURCE_laser_beam_texture_path);
	highlighter_beam_texture = window.load_texture(RESOURCE_highlighter_beam_texture_path);

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

	search_for_targets();
}

void generate_ship_outline(Entity* ship)
{
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

	if (outline.size() < 4 * SETTING_MAX_POINT_COUNT)
	{
		for (int i = 0; i < outline.size(); i++)
			ship->outline[i] = outline.at(i);
		ship->outline_point_count = outline.size();
	}
	else
		SDL_Log("ERROR: outline buffer overflow for ship.\n");
}

SDL_Point default_position(int i)
{
	int radius = GAME_width >> 2;
	return { (int)((float)(GAME_width >> 1) + cosf((float)i / (float)GAME_ship_count * 2.0F * PI) * (float)radius), (int)((float)(GAME_height >> 1) + sinf((float)i / (float)GAME_ship_count * 2.0F * PI) * (float)radius) };
}

Entity* raycast(float origin_x, float origin_y, float theta, float max_dist, int ignore_id, SDL_Point* hit);

int last_chunk_id = 0;

void search_for_targets()
{
	if (target_positions.size() == 0)
	{
		for (int i = 0; i < GAME_ship_count; i++)
		{
			target_positions.push_back(default_position(i));
			shadowing_targets.push_back(new std::vector<int>());
		}
		return;
	}

	int i = GAME_ship_count;
	int chunk = last_chunk_id;
	while (i > 0)
	{
		int check_radius = 2;

		int floor_y = chunk / GAME_chunkwise_width - check_radius;
		int ceil_y = chunk / GAME_chunkwise_width + check_radius;
		int left_x = chunk % GAME_chunkwise_width - check_radius;
		int right_x = chunk % GAME_chunkwise_width + check_radius;

		if (collision_check_grid[chunk] == nullptr)
		{
			chunk++;
			chunk %= GAME_chunkwise_height * GAME_chunkwise_width;
			continue;
		}

		i--;
		shadowing_targets[i]->clear();

		for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, GAME_chunkwise_height); curr_y++)
		{
			for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, GAME_chunkwise_width); curr_x++)
			{
				int curr_chunk = curr_x + (GAME_chunkwise_width)*curr_y;
				if (collision_check_grid[curr_chunk] == nullptr)
					continue;

				std::set<int>* curr_set = (collision_check_grid[curr_chunk]);
				shadowing_targets[i]->insert(shadowing_targets[i]->end(), curr_set->begin(), curr_set->end());
			}
		}

		chunk++;
		chunk %= GAME_chunkwise_height * GAME_chunkwise_width;
	}

	last_chunk_id = chunk;
}

void update_target_position(int i);

void update_target_position(int i)
{
	float x_accumulate = 0.0F;
	float y_accumulate = 0.0F;
	for (int id : *shadowing_targets[i])
	{
		Asteroid* asteroid = ((Asteroid*)entities + id);
		x_accumulate += asteroid->x;
		y_accumulate += asteroid->y;
	}

	x_accumulate /= (float)shadowing_targets[i]->size();
	y_accumulate /= (float)shadowing_targets[i]->size();
	target_positions[i] = { (int)x_accumulate, (int)y_accumulate };
}

void ships_update(float delta_time)
{
	static float timer = 0.0F;
	float search_tick = 15.0F;

	if (timer > search_tick)
	{
		search_for_targets();
		timer = 0.0F;
	}

	int i = 0;
	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		if (DEBUG_mode && DEBUG_ships_fire_at_will)
			select_targets(ship);
		if (ship_attack_timers[i] > 0.0F)
		{
			Entity* target = ((Entity*)entities) + ship_targets[i];

			// cooling down
			if (ship_attack_timers[i] < SHIP_attack_cooldown_time)
			{
				ship->velocity_x = 0.0F;
				ship->velocity_y = 0.0F;
				ship->desired_velocity_x = ship->velocity_x;
				ship->desired_velocity_y = ship->velocity_y;
			}
			// currently attacking (animate projectile)
			else if (ship_attack_timers[i] < SHIP_attack_cooldown_time + SHIP_attack_time)
			{
				// slight recoil
				float diff_x = target->x - ship->x;
				float diff_y = target->y - ship->y;
				float recoil_percentage = 0.00001F * (float)(rand() % 11); // maximum recoil of 0.01% of distance 

				ship->velocity_x = -diff_x * recoil_percentage;
				ship->velocity_y = -diff_y * recoil_percentage;
				ship->desired_velocity_x = ship->velocity_x;
				ship->desired_velocity_y = ship->velocity_y;
			}
			// charging up
			else
			{
				// rotate towards target; stay in place
				ship->rotation = atan2(target->y - ship->y, target->x - ship->x);

				ship->velocity_x = 0.0F;
				ship->velocity_y = 0.0F;
				ship->desired_velocity_x = ship->velocity_x;
				ship->desired_velocity_y = ship->velocity_y;
			}


			float old_attack_timer = ship_attack_timers[i];

			ship_attack_timers[i] -= (delta_time / 1000.0F);

			if (old_attack_timer > SHIP_attack_cooldown_time &&
				ship_attack_timers[i] < SHIP_attack_cooldown_time)
			{
				// who the raycast hit affects (could be diff from target)
				SDL_Point hit;
				Entity* effected = raycast(ship->x, ship->y, ship->rotation, SHIP_max_attack_range, ship->id, &hit);

				if (effected == NULL)
					continue;

				SDL_Log("hit something; splitting; %i", effected->id);
				((Asteroid*)effected)->split(hit.x, hit.y, 10.0F);
			}
		}
		else
		{
			// follow target behavior
			if (target_positions.size() == 0)
				continue;

			update_target_position(i);

			SDL_Point pos = target_positions[i];
			float x = (float)pos.x - ship->x;
			float y = (float)pos.y - ship->y;
			float theta = atan2f(y, x);

			float distance = sqrtf(x * x + y * y);

			float critical_distance = 1.0F;

			float vel_x = cosf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);
			float vel_y = sinf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);



			if (run_ship_avoidance(ship, 0.0075F, &vel_x, &vel_y) || distance > critical_distance)
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
		}

		ship->update();
		i++;
	}

	timer += delta_time / 1000.0F; // timer is in seconds
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

	int floor_y = (int)y / chunk_size - look_radius;
	int ceil_y = (int)y / chunk_size + look_radius;
	int left_x = (int)x / chunk_size - look_radius;
	int right_x = (int)x / chunk_size + look_radius;
	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, GAME_chunkwise_height); curr_y++)
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, GAME_chunkwise_width); curr_x++)
		{
			int chunk = curr_x + (GAME_width / chunk_size) * curr_y;
			if (collision_check_grid[chunk] == nullptr)
				continue;
			std::set<int> curr_set = *(collision_check_grid[chunk]);
			to_check.insert(curr_set.begin(), curr_set.end());
		}
	}

	for (int id : to_check)
	{
		if (id == ship->id)
			continue;


		Entity* other = Entity::active[id];
		//window.render_rect(
		//	other->x, other->y,
		//	(float)other->w, (float)other->h, { 0, 0, 255, 255 });


		*vel_x -= (other->x - x) * multiplier;
		*vel_y -= (other->y - y) * multiplier;
		result = true;
	}

	return result;
}

void render_fovs(RenderWindow* window)
{
	double fov = M_PI / 3.0;
	double granularity = 0.0005;
	float radius = 100.0F;

	//TODO: texture???

	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		// TODO: multithread/multiprocess
		// TODO: OpenGL: https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideoopengl.html

		double theta = ship->rotation - fov / 2.0;

		float cos_theta = cosf(theta);
		float sin_theta = sinf(theta);
		for (float i = 0.0F; i < radius; i++)
		{
			window->render_rect(ship->x + i * cos_theta, ship->y + i * sin_theta, 1.0F, 1.0F, { 100, 100, 100, 255 });
		}

		theta = ship->rotation + fov / 2.0;
		cos_theta = cosf(theta);
		sin_theta = sinf(theta);
		for (float i = 0.0F; i < radius; i++)
		{
			window->render_rect(ship->x + i * cos_theta, ship->y + i * sin_theta, 1.0F, 1.0F, { 100, 100, 100, 255 });
		}

		double theta_max = ship->rotation + fov / 2.0;
		for (double theta = ship->rotation - fov / 2.0; theta < theta_max; theta += granularity)
		{
			window->render_rect(ship->x + cosf(theta) * radius, ship->y + sinf(theta) * radius, 1.0F, 1.0F, { 100, 100, 100, 255 });
		}
	}
}

void select_targets(Entity* ship)
{
	std::set<int> to_check;
	if (ship_attack_timers[ship->id] > 0.0F)
		return;

	float x = ship->x;
	float y = ship->y;

	float speed_per_rad_increase = 0.8F;
	int check_radius = 2;

	int floor_y = (int)y / chunk_size - check_radius;
	int ceil_y = (int)y / chunk_size + check_radius;
	int left_x = (int)x / chunk_size - check_radius;
	int right_x = (int)x / chunk_size + check_radius;
	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, GAME_chunkwise_height); curr_y++)
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, GAME_chunkwise_width); curr_x++)
		{
			int chunk = curr_x + (GAME_width / chunk_size) * curr_y;
			if (collision_check_grid[chunk] == nullptr)
				continue;
			std::set<int> curr_set = *(collision_check_grid[chunk]);
			to_check.insert(curr_set.begin(), curr_set.end());
		}
	}

	for (int id : to_check)
	{
		if (id == ship->id)
			continue;

		if (((Entity*)entities)[id].outline_point_count > 50)
		{
			ship_targets[ship->id] = id;
			ship_attack_timers[ship->id] = SHIP_attack_time + SHIP_attack_targetting_time + SHIP_attack_cooldown_time;
			return;
		}
	}


}

void ships_render_update(RenderWindow* window)
{
	render_fovs(window);

	float base_beam_width = 30;
	float variance_percentage = 0.6F;
	int blips = 2;

	int i = 0;
	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		if (ship_attack_timers[i] > 0.0F)
		{
			// what the ship is trying to hit
			Entity* target = ((Entity*)entities) + ship_targets[i];

			// who the raycast hit affects (could be diff from target)
			SDL_Point hit;
			Entity* effected = raycast(ship->x, ship->y, ship->rotation, SHIP_max_attack_range, ship->id, &hit);

			float diff_x = (effected != NULL ? effected->x : target->x) - ship->x;
			float diff_y = (effected != NULL ? effected->y : target->y) - ship->y;

			int beam_width = base_beam_width;
			float height = SDL_min(sqrtf(diff_x * diff_x + diff_y * diff_y), SHIP_max_attack_range);

			// cooling down
			if (ship_attack_timers[i] < SHIP_attack_cooldown_time)
			{
			}
			// currently attacking (animate projectile)
			else if (ship_attack_timers[i] < SHIP_attack_cooldown_time + SHIP_attack_time)
			{
				float value_normalized = sinf((ship_attack_timers[i] - SHIP_attack_cooldown_time) / SHIP_attack_time * 2.0F * PI * (float)blips);
				value_normalized = value_normalized * 0.5F + 0.5F;

				beam_width = base_beam_width * variance_percentage + base_beam_width * (1.0F - variance_percentage) * value_normalized;
				// rotates around (0,0) aka origin for beam
				window->render_rotate(0, 0, 0, 0,
					ship->screen_x, ship->screen_y - (beam_width >> 1), height, beam_width,
					0, beam_width >> 1, ship->rotation,
					laser_beam_texture);
			}
			// charging up
			else
			{
				//window->render_rotate(0, 0, 0, 0, ship->screen_x, ship->screen_y - (beam_width >> 1), height, beam_width, 0, beam_width >> 1, ship->rotation, highlighter_beam_texture);
			}

			/*char buffer[16] = "";
			sprintf_s(buffer, "%.4f", ship_attack_timers[i]);
			window->render_centered_world(ship->x, ship->y, buffer, encode_sans_medium, { 255, 255, 255, 255 });*/

			// decerements time in ships_update() func already (above is mirror functionality for render stage)
		}

		if (DEBUG_mode && DEBUG_ship_targets)
		{
			float size = 5.0F;
			window->render_rect((float)target_positions[i].x - size * 0.5F, (float)target_positions[i].y - size * 0.5F, size, size, { 255, 255, 0, 255 });
		}

		ship->render(window);
		i++;
	}
}

Entity* check_overlap(float x, float y, int ignore_id);

Entity* raycast(float origin_x, float origin_y, float theta, float max_dist, int ignore_id, SDL_Point* hit)
{
	float resolution = 1.0F;
	int x, y;
	int x_prev = 0, y_prev = 0;
	for (float j = 0; j <= max_dist; j += resolution)
	{
		x = (int)(cosf(theta) * j) + origin_x;
		y = (int)(sinf(theta) * j) + origin_y;

		if (DEBUG_mode && DEBUG_display_entity_outlines)
			window.render_rect((float)x, (float)y, 1.0F, 1.0F, SDL_Color{ 255, 0, 0, 255 });

		if (x_prev == x && y_prev == y)
			continue;

		Entity* other;
		if ((other = check_overlap(x, y, ignore_id)))
		{
			*hit = { x, y };
			return other;
		}

		x_prev = x;
		y_prev = y;
	}

	return nullptr;
}

Entity* check_overlap(float x, float y, int ignore_id)
{
	std::set<int> to_check;

	float speed_per_rad_increase = 0.8F;
	int check_radius = 1;

	int floor_y = (int)y / chunk_size - check_radius;
	int ceil_y = (int)y / chunk_size + check_radius;
	int left_x = (int)x / chunk_size - check_radius;
	int right_x = (int)x / chunk_size + check_radius;
	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, GAME_chunkwise_height); curr_y++)
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, GAME_chunkwise_width); curr_x++)
		{
			int chunk = curr_x + (GAME_width / chunk_size) * curr_y;
			if (collision_check_grid[chunk] == nullptr)
				continue;
			std::set<int> curr_set = *(collision_check_grid[chunk]);
			to_check.insert(curr_set.begin(), curr_set.end());
		}
	}

	for (int id : to_check)
	{
		if (id == ignore_id)
			continue;
		Entity* other = Entity::active[id];
		if (!other->in_bounds(x, y))
			continue;

		for (int i = 0; i < other->outline_point_count; i++)
		{
			if ((int)x - (other->screen_x - (other->w >> 1)) == other->outline[i].x &&
				(int)y - (other->screen_y - (other->h >> 1)) == other->outline[i].y)
				return other;
		}
	}

	return nullptr;
}

void ships_cleanup()
{
	SDL_DestroyTexture(ship_texture);
	SDL_DestroyTexture(laser_beam_texture);
	SDL_DestroyTexture(highlighter_beam_texture);
	delete[] ship_targets;
	delete[] ship_attack_timers;
}
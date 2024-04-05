#include "ship.h"
#include "asteroid.h"
#include "../main.h"
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <time.h>
#include "thrust_renderer.h"

SDL_Texture* ship_texture;
SDL_Texture* pip_texture;
SDL_Texture* laser_beam_texture;
SDL_Texture* highlighter_beam_texture;


void render_fovs(RenderWindow* window);
void render_notice_bars(RenderWindow* window);
bool run_ship_avoidance(Entity* ship, float multiplier, float* vel_x, float* vel_y);
void generate_ship_outline(Entity* ship);
void search_for_targets();
Entity* check_overlap(float x, float y, int ignore_id);

void render_dotted_line(RenderWindow* window, Entity* ship, Entity* asteroid, float signed_end = 0.0F, const SDL_Color& color = { 255, 255, 255, 255 });
void render_dotted_circle(RenderWindow* window, Entity* asteroid, float radius = -1.0F, const SDL_Color& color = { 255, 255, 255, 255 });

void ship_check_states(int i);

// notice data
std::vector<std::map<int, float>*> notice_timers;

// active pursuit data
std::vector<SDL_Point> target_positions;
std::vector<std::set<int>*> shadowing_targets;

float* ship_warn_timers;
int* ship_targets;
float* ship_attack_timers;

int* ship_healths;
float* ship_health_damaged_timers;

float notice_for_auto_warn = 10.0F;
float notice_for_auto_attack = 15.0F;

float vel_to_notice_scaling = 4.0F;
float accel_to_notice_scaling = 15.0F;
float notice_decay_scaling = 2.0F;

int get_health(Entity* ship)
{
	if (ship->id >= GAME_ship_count)
		return -1;
	return ship_healths[ship->id];
}

void ships_init()
{
	ship_targets = new int[GAME_ship_count];
	ship_warn_timers = new float[GAME_ship_count];
	ship_attack_timers = new float[GAME_ship_count];

	ship_healths = new int[GAME_ship_count];
	ship_health_damaged_timers = new float[GAME_ship_count];

	for (int i = 0; i < GAME_ship_count; i++)
	{
		ship_targets[i] = 0;
		ship_warn_timers[i] = 0.0F;
		ship_attack_timers[i] = 0.0F;
		ship_healths[i] = SHIP_initial_health;
		ship_health_damaged_timers[i] = 0.0F;
		notice_timers.push_back(new std::map<int, float>());
	}

	ship_texture = window.load_texture(RESOURCE_ship_texture_path);
	pip_texture = window.load_texture(RESOURCE_pip_texture_path);
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

		ship->drag = 0.0F;
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

void set_ship_shadowing_chunk(int shipID, int chunk);

void set_ship_shadowing_chunk(int shipID, int chunk)
{
	int check_radius = 2;

	int floor_y = chunk / GAME_chunkwise_width - check_radius;
	int ceil_y = chunk / GAME_chunkwise_width + check_radius;
	int left_x = chunk % GAME_chunkwise_width - check_radius;
	int right_x = chunk % GAME_chunkwise_width + check_radius;

	shadowing_targets[shipID]->clear();

	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, GAME_chunkwise_height); curr_y++)
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, GAME_chunkwise_width); curr_x++)
		{
			int curr_chunk = curr_x + (GAME_chunkwise_width)*curr_y;
			if (collision_check_grid[curr_chunk] == nullptr)
				continue;

			std::set<int>* curr_set = (collision_check_grid[curr_chunk]);
			for (int id : *curr_set)
				if (id >= GAME_ship_count) // if it's not a ship
					shadowing_targets[shipID]->insert(id);
		}
	}
}

void search_for_targets()
{
	if (target_positions.size() == 0)
	{
		for (int i = 0; i < GAME_ship_count; i++)
		{
			target_positions.push_back(default_position(i));
			shadowing_targets.push_back(new std::set<int>());
		}
		return;
	}

	int i = GAME_ship_count;
	int chunk = last_chunk_id;
	while (i > 0)
	{
		if (ship_warn_timers[i] > 0.0F)
		{
			i--;
			continue;
		}

		if (collision_check_grid[chunk] == nullptr)
		{
			chunk++;
			chunk %= GAME_chunkwise_height * GAME_chunkwise_width;
			continue;
		}

		i--;
		set_ship_shadowing_chunk(i, chunk);

		chunk++;
		chunk %= GAME_chunkwise_height * GAME_chunkwise_width;
	}

	last_chunk_id = chunk;
}

void update_target_position(int i);

void update_target_position(int i)
{
	Entity* ship = (Entity*)entities + i;

	// charging behavior
	if (ship_attack_timers[i] >= SHIP_attack_cooldown_time + SHIP_attack_time)
	{
		Entity* target = (Entity*)entities + ship_targets[i];
		target_positions[i] = {
			(int)(target->x + target->velocity_x * (SHIP_attack_targetting_time + SHIP_attack_time)),
			(int)(target->y + target->velocity_x * (SHIP_attack_targetting_time + SHIP_attack_time))
		};
		return;
	}

	float x_accumulate = 0.0F;
	float y_accumulate = 0.0F;
	for (int id : *shadowing_targets[i])
	{
		if (id < GAME_ship_count)
			continue;

		Asteroid* asteroid = ((Asteroid*)entities + id);
		x_accumulate += asteroid->x;
		y_accumulate += asteroid->y;

		if (ship_warn_timers[i] > 0.0F)
		{
			x_accumulate += asteroid->velocity_x * (SHIP_attack_targetting_time + SHIP_attack_time);
			x_accumulate += asteroid->velocity_y * (SHIP_attack_targetting_time + SHIP_attack_time);
		}
	}

	x_accumulate /= (float)shadowing_targets[i]->size();
	y_accumulate /= (float)shadowing_targets[i]->size();
	target_positions[i] = { (int)x_accumulate, (int)y_accumulate };
}

void ships_tick_warning(int i);

void ship_tick_attack(int i);

void tick_notices();

void ships_update(float delta_time)
{
	static float timer = 0.0F;
	float search_tick = 15.0F;

	if (timer > search_tick)
	{
		search_for_targets();
		timer = 0.0F;
	}

	tick_notices();

	int i = 0;
	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		if (ship_healths[i] <= 0)
		{
			// only run physical update; no behavior for dead ship
			ship->update();
			i++;
			continue;
		}

		//if (DEBUG_mode && DEBUG_ships_fire_at_will)
		ship_check_states(i);

		SDL_Point pos = target_positions[i];
		float x = (float)pos.x - ship->x;
		float y = (float)pos.y - ship->y;
		float distance = sqrtf(x * x + y * y);

		float crit_vel = 0.2F;

		if (distance > crit_vel)
			ship->desired_rotation = atan2(ship->velocity_y, ship->velocity_x);
		else
			ship->desired_rotation = 0.0;

		if (ship_attack_timers[i] > 0.0F)
			ship_tick_attack(i);

		if (ship_warn_timers[i] > 0.0F)
			ships_tick_warning(i);

		if (ship_health_damaged_timers[i] > 0.0F)
			ship_health_damaged_timers[i] -= (delta_time / 1000.0F);
		else
			ship_health_damaged_timers[i] = 0.0F;

		// follow target behavior
		if (target_positions.size() == 0)
		{
			i++;
			continue;
		}

		update_target_position(i);



		float theta = atan2f(y, x);



		float critical_distance = 1.0F;

		float vel_x = cosf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);
		float vel_y = sinf(theta) * SDL_clamp(SHIP_speed_maximum, 0, distance);

		if (run_ship_avoidance(ship, 0.0001F, &vel_x, &vel_y) || distance > critical_distance)
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


		ship->update();
		i++;
	}

	timer += delta_time / 1000.0F; // timer is in seconds
}

// decays notices by scaled time
void tick_notices()
{
	int i = 0;
	for (std::map<int, float>* ptr : notice_timers)
	{
		for (auto iter = ptr->begin(); iter != ptr->end(); iter++)
		{
			// short to 0 if ship dies
			if (ship_healths[i] > 0 && (*iter).second > 0.0F)
			{
				ptr->at((*iter).first) -= delta_time / 1000.0F * notice_decay_scaling;
			}
			else
				ptr->at((*iter).first) = 0.0F;
		}
		i++;
	}
}

void ship_check_states(int i)
{
	Entity* ship = (Entity*)entities + i;

	std::set<int> to_check;
	if (ship_attack_timers[ship->id] > 0.0F) // if attacking skip
		return;


	float x = ship->x;
	float y = ship->y;

	int awareness_radius = 2;

	int floor_y = (int)y / chunk_size - awareness_radius;
	int ceil_y = (int)y / chunk_size + awareness_radius;
	int left_x = (int)x / chunk_size - awareness_radius;
	int right_x = (int)x / chunk_size + awareness_radius;
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
		if (id < GAME_ship_count)
			continue;

		Entity* other = ((Entity*)entities) + id;
		float vel_x = other->velocity_x;
		float vel_y = other->velocity_y;
		float desired_vel_x = other->desired_velocity_x - vel_x;
		float desired_vel_y = other->desired_velocity_y - vel_y;

		const float crit_vel_notice = 0.06F;
		const float crit_accel_notice = 0.03F;

		if (notice_timers[i]->find(id) == notice_timers[i]->end())
			notice_timers[i]->insert({ id, 0.0F });

		if (desired_vel_x * desired_vel_x + desired_vel_y * desired_vel_y >= crit_accel_notice * crit_accel_notice)
			notice_timers[i]->at(id) = SDL_min(notice_timers[i]->at(id) + (delta_time / 1000.0F) * accel_to_notice_scaling, notice_for_auto_attack);
		else if (vel_x * vel_x + vel_y * vel_y >= crit_vel_notice * crit_vel_notice)
			notice_timers[i]->at(id) = SDL_min(notice_timers[i]->at(id) + (delta_time / 1000.0F) * vel_to_notice_scaling, notice_for_auto_attack);


		if (notice_timers[i]->at(id) >= notice_for_auto_warn && ship_warn_timers[i] <= 0.0F)
		{
			set_ship_shadowing_chunk(i, other->collision_chunk);
			ship_warn_timers[i] = SHIP_warning_time;
			notice_timers[i]->at(id) = 0.0F;
			continue;
		}


		if ((*shadowing_targets[ship->id]).find(id) == (*shadowing_targets[ship->id]).end())
			continue;

		// check pursued attack targets if in range
		const float attack_crit_vel_general = 0.1F;
		const float attack_crit_vel_player = 0.05F;

		if (notice_timers[i]->at(id) + 2.0F >= notice_for_auto_attack || (DEBUG_mode && DEBUG_ships_fire_at_will))
		{
			ship_warn_timers[i] = SHIP_warning_time;
			ship_targets[ship->id] = id;
			ship_attack_timers[ship->id] = SHIP_attack_time + SHIP_attack_targetting_time + SHIP_attack_cooldown_time;
			notice_timers[i]->at(id) = 0.0F;
			return;
		}
	}
}

void ship_tick_attack(int i)
{
	Entity* ship = (Entity*)entities + i;
	Entity* target = ((Entity*)entities) + ship_targets[i];

	float diff_x = target->x - (target->w >> 1) + target->center_x - ship->x;
	float diff_y = target->y - (target->h >> 1) + target->center_y - ship->y;

	// cooling down
	if (ship_attack_timers[i] < SHIP_attack_cooldown_time)
	{
		/*ship->velocity_x = 0.0F;
		ship->velocity_y = 0.0F;
		ship->desired_velocity_x = ship->velocity_x;
		ship->desired_velocity_y = ship->velocity_y;*/
	}
	// currently attacking (animate projectile)
	else if (ship_attack_timers[i] < SHIP_attack_cooldown_time + SHIP_attack_time)
	{
		// slight recoil
		float recoil_percentage = 0.0001F * (float)(rand() % 11); // maximum recoil of 0.01% of distance 
		ship->desired_rotation = atan2(diff_y, diff_x);

		ship->desired_velocity_x = -diff_x * recoil_percentage;
		ship->desired_velocity_y = -diff_y * recoil_percentage;
	}
	// charging to target
	else
	{
		// rotate towards target; stay in place
		float attack_theta = atan2(target->y - (target->h >> 1) + target->center_y - ship->y, target->x - (target->w >> 1) + target->center_x - ship->x);
		float vel_theta = atan2(ship->velocity_y, ship->velocity_x);

		float signed_dist_normalized = diff_x * diff_x + diff_y * diff_y - SHIP_max_attack_range * SHIP_max_attack_range;
		if (signed_dist_normalized > 0.0F)
			signed_dist_normalized = 1.0F / (1.0F + signed_dist_normalized);
		else
			signed_dist_normalized = 1.0F;

		ship->desired_rotation = attack_theta * signed_dist_normalized + vel_theta * (1.0F - signed_dist_normalized);
	}

	bool in_range = diff_x * diff_x + diff_y * diff_y <= SHIP_max_attack_range * SHIP_max_attack_range;

	// only tick down if in range while chasing (while attacking tick regardless)
	if (in_range || ship_attack_timers[i] <= SHIP_attack_cooldown_time + SHIP_attack_time)
	{
		float old_attack_timer = ship_attack_timers[i];

		ship_attack_timers[i] -= (delta_time / 1000.0F);

		if (old_attack_timer > SHIP_attack_cooldown_time &&
			ship_attack_timers[i] < SHIP_attack_cooldown_time)
		{
			// who the raycast hit affects (could be diff from target)
			SDL_Point hit;
			Entity* effected = raycast(ship->x, ship->y, ship->rotation, SHIP_max_attack_range, ship->id, &hit);

			if (effected == NULL || effected->id < GAME_ship_count)
				return;

			SDL_Log("hit something; splitting; %i", effected->id);
			((Asteroid*)effected)->split(hit.x, hit.y, 10.0F);
		}
	}
}

void ships_tick_warning(int i)
{
	if (ship_warn_timers[i] > 0.0F)
		ship_warn_timers[i] -= (delta_time / 1000.0F);
	else
		ship_warn_timers[i] = 0.0F;
}

bool run_ship_avoidance(Entity* ship, float multiplier, float* vel_x, float* vel_y)
{
	bool result = false;
	std::set<int> to_check;

	float x = ship->x;
	float y = ship->y;

	float speed_per_rad_increase = 0.8F;
	int look_radius = 2 + (sqrtf(ship->velocity_x * ship->velocity_x + ship->velocity_y * ship->velocity_y) / speed_per_rad_increase);

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

void render_health(RenderWindow* window, Entity* ship)
{
	int id = ship->id;
	if (ship_health_damaged_timers[id] <= 0.0F)
		return;

	float pip_height = 3.0F;
	float pip_width = 3.0F;
	float padding_per_pip = 1.0F;
	float opacity = 0.0F;

	// pulses at start; solid opacity in middle; fade out at end
	if (ship_health_damaged_timers[id] <= SHIP_health_damaged_fadetime)
		opacity = ship_health_damaged_timers[id] / SHIP_health_damaged_fadetime;
	else if (ship_health_damaged_timers[id] <= SHIP_health_damaged_fadetime + SHIP_health_damaged_showtime)
		opacity = 1.0F;
	else
	{
		float progress = 1.0F - ((ship_health_damaged_timers[id] - SHIP_health_damaged_fadetime - SHIP_health_damaged_showtime) / SHIP_health_damaged_pulsetime);
		float theta = progress * PI * 2 * SHIP_health_damaged_pulses;
		opacity = sinf(theta) * 0.5F + 0.5F;
	}

	float x = ship->x - (pip_width + 2 * padding_per_pip) * SHIP_initial_health;
	for (int i = 0; i < SHIP_initial_health; i++)
	{
		int alpha = (Uint8)(opacity * 255.0F * (i >= ship_healths[ship->id] ? 0.6F : 1.0F));

		window->render_alphamod(0, 0, 64, 64, (int)(x + padding_per_pip), (int)(ship->y - (ship->h >> 1) - pip_height), pip_width, pip_height, pip_texture, alpha);
		x += pip_width + 2 * padding_per_pip;
	}
}

void render_notice_bars(RenderWindow* window)
{
	std::map<int, float> accumulated;

	if (SETTING_all_targetting_indicators)
	{
		for (std::map<int, float>* ptr : notice_timers)
		{
			for (auto iter = ptr->begin(); iter != ptr->end(); iter++)
			{
				if (accumulated.find((*iter).first) != accumulated.end())
				{
					if ((*iter).second > accumulated[((*iter).first)])
						accumulated[((*iter).first)] = (*iter).second;
					continue;
				}

				accumulated.insert(*iter);
			}
		}

		SDL_Color color_bg = { 100, 100, 100, 60 };

		SDL_Color color_good = { 255, 255, 255, 255 };
		SDL_Color color_warned = { 255, 255, 0, 255 };
		SDL_Color color_attacking = { 255, 0, 0, 255 };

		for (std::pair<int, float> pair : accumulated)
		{
			if (pair.second <= 0.0F)
				continue;

			if (!SETTING_all_targetting_indicators && pair.first != PLAYER_entity_id)
				continue;

			// render notices
			Entity* entity = ((Entity*)entities + pair.first);
			SDL_Color color;
			float progress;

			bool* warning_ship = new bool[GAME_ship_count];
			bool warned = false;
			for (int i = 0; i < GAME_ship_count; i++)
			{
				// search for living, warning ships
				if (shadowing_targets[i]->find(pair.first) != shadowing_targets[i]->end() && ship_warn_timers[i] >= 0.0F && ship_healths[i] > 0)
				{
					warning_ship[i] = true;
					warned = true;
				}
				else
					warning_ship[i] = false;
			}
			if (warned)
			{
				progress = SDL_min(pair.second / notice_for_auto_attack, 1.0F);
				color = SDL_Color{ (unsigned char)((float)color_warned.r * (1.0F - progress) + (float)color_attacking.r * progress),
				(unsigned char)((float)color_warned.g * (1.0F - progress) + (float)color_attacking.g * progress),
				(unsigned char)((float)color_warned.b * (1.0F - progress) + (float)color_attacking.b * progress),
				255 };


				for (int i = 0; i < GAME_ship_count; i++)
				{
					if (ship_attack_timers[i] >= SHIP_attack_cooldown_time && ship_healths[i] > 0)
						render_dotted_line(window, (Entity*)entities + i, (Entity*)entities + ship_targets[i], 0.0F, { 255, 0, 0, 255 });
					else if (warning_ship[i])
						render_dotted_line(window, (Entity*)entities + i, (Entity*)entities + pair.first, 10.0F + (float)((Entity*)entities + pair.first)->outline_point_count / (2.0F * PI), { 255, 255, 0, 255 });
				}
			}
			else
			{
				progress = SDL_min(pair.second / notice_for_auto_warn, 1.0F);
				color = SDL_Color{ (unsigned char)((float)color_good.r * (1.0F - progress) + (float)color_warned.r * progress),
				(unsigned char)((float)color_good.g * (1.0F - progress) + (float)color_warned.g * progress),
				(unsigned char)((float)color_good.b * (1.0F - progress) + (float)color_warned.b * progress),
				255 };
			}

			delete[] warning_ship;

			int bar_screen_width = 30;
			int bar_screen_height = 3;
			float world_x_offset = 0.0F, world_y_offset = ((float)entity->outline_point_count / (2 * PI)) - 0.3F; // rough radius approxomation
			int screen_origin_x, screen_origin_y;

			window->camera.world_to_screen(entity->x + world_x_offset, entity->y - (entity->h >> 1) + world_y_offset, &screen_origin_x, &screen_origin_y);



			window->render_rect(screen_origin_x - (bar_screen_width >> 1), screen_origin_y - (bar_screen_height >> 1), bar_screen_width, bar_screen_height, color_bg);
			window->render_rect(screen_origin_x - (bar_screen_width >> 1), screen_origin_y - (bar_screen_height >> 1), (int)(progress * (float)bar_screen_width), bar_screen_height, color);
		}
	}
	else
	{
		SDL_Color color_bg = { 100, 100, 100, 60 };

		SDL_Color color_good = { 255, 255, 255, 255 };
		SDL_Color color_warned = { 255, 255, 0, 255 };
		SDL_Color color_attacking = { 255, 0, 0, 255 };

		int ship_id = 0;
		for (std::map<int, float>* ptr : notice_timers)
		{
			for (auto iter = ptr->begin(); iter != ptr->end(); iter++)
			{
				std::pair<int, float> pair = *iter;

				if (pair.second <= 0.0F)
					continue;

				if (pair.first != PLAYER_entity_id)
					continue;

				// render notice
				Entity* player = ((Entity*)entities + PLAYER_entity_id);
				Entity* ship = ((Entity*)entities + ship_id);
				SDL_Color color;
				float progress;

				bool warned = false;
				// check if living, warning ship focuses player
				if (shadowing_targets[ship_id]->find(PLAYER_entity_id) != shadowing_targets[ship_id]->end() && ship_warn_timers[ship_id] > 0.0F && ship_healths[ship_id] > 0)
					warned = true;

				if (warned)
				{
					progress = SDL_min(pair.second / notice_for_auto_attack, 1.0F);
					color = SDL_Color{ (unsigned char)((float)color_warned.r * (1.0F - progress) + (float)color_attacking.r * progress),
					(unsigned char)((float)color_warned.g * (1.0F - progress) + (float)color_attacking.g * progress),
					(unsigned char)((float)color_warned.b * (1.0F - progress) + (float)color_attacking.b * progress),
					255 };


					if (ship_attack_timers[ship_id] >= SHIP_attack_cooldown_time && ship_healths[ship_id] > 0)
						render_dotted_line(window, ship, player, 0.0F, { 255, 0, 0, 255 });
					else
						render_dotted_line(window, ship, player, 10.0F + (float)(player->outline_point_count / (2.0F * PI)), { 255, 255, 0, 255 });

				}
				else
				{
					progress = SDL_min(pair.second / notice_for_auto_warn, 1.0F);
					color = SDL_Color{ (unsigned char)((float)color_good.r * (1.0F - progress) + (float)color_warned.r * progress),
					(unsigned char)((float)color_good.g * (1.0F - progress) + (float)color_warned.g * progress),
					(unsigned char)((float)color_good.b * (1.0F - progress) + (float)color_warned.b * progress),
					255 };
				}

				int bar_screen_width = 30;
				int bar_screen_height = 5;
				float world_x_offset = 0.0F;
				float world_y_offset = 5.0F; // rough radius approxomation
				int screen_origin_x, screen_origin_y;

				window->camera.world_to_screen(ship->x + world_x_offset, ship->y + (ship->h >> 1) + world_y_offset, &screen_origin_x, &screen_origin_y);

				window->render_rect(screen_origin_x - (bar_screen_width >> 1), screen_origin_y - (bar_screen_height >> 1), bar_screen_width, bar_screen_height, color_bg);
				window->render_rect(screen_origin_x - (bar_screen_width >> 1), screen_origin_y - (bar_screen_height >> 1), (int)(progress * (float)bar_screen_width), bar_screen_height, color);
			}
			ship_id++;
		}
	}
}


void render_dotted_circle(RenderWindow* window, Entity* asteroid, float radius, const SDL_Color& color)
{
	if (radius == -1.0F)
		radius = asteroid->outline_point_count / (2 * PI);

	int line_ammo = 5;
	int line_rest = -10;

	int curr_line = line_rest;

	float resolution = 0.03F;
	int origin_x = asteroid->x - (asteroid->w >> 1) + asteroid->center_x;
	int origin_y = asteroid->y - (asteroid->h >> 1) + asteroid->center_y;

	int x, y;
	int x_prev = 0, y_prev = 0;
	for (float theta = 0; theta <= 2 * PI; theta += resolution)
	{
		x = (int)(cosf(theta) * radius) + origin_x;
		y = (int)(sinf(theta) * radius) + origin_y;

		if (curr_line > 0)
			window->render_rect((float)x, (float)y, 1.0F, 1.0F, color);

		/*if (x_prev == x && y_prev == y)
			continue;*/

		if (curr_line < 0)
		{
			curr_line++;

			if (curr_line == 0)
				curr_line = line_ammo;
		}
		else if (curr_line > 0)
		{
			curr_line--;

			if (curr_line == 0)
				curr_line = line_rest;
		}

		x_prev = x;
		y_prev = y;
	}
}

void render_dotted_line(RenderWindow* window, Entity* ship, Entity* asteroid, float signed_end, const SDL_Color& color)
{
	float diff_y = asteroid->y - (asteroid->h >> 1) + asteroid->center_y - ship->y;
	float diff_x = asteroid->x - (asteroid->w >> 1) + asteroid->center_x - ship->x;
	float dist = sqrtf(diff_y * diff_y + diff_x * diff_x) - signed_end;

	float theta = atan2(diff_y, diff_x);

	SDL_Point hit;

	int line_ammo = 5;
	int line_rest = -10;

	int curr_line = line_rest;

	float resolution = 1.0F;
	int x, y;
	int x_prev = 0, y_prev = 0;
	for (float j = 0; j <= dist; j += resolution)
	{
		x = (int)(cosf(theta) * j) + ship->x;
		y = (int)(sinf(theta) * j) + ship->y;

		if (curr_line > 0)
			window->render_rect((float)x, (float)y, 1.0F, 1.0F, color);

		if (x_prev == x && y_prev == y)
			continue;

		Entity* other;
		if ((other = check_overlap(x, y, ship->id)))
			break;

		if (curr_line < 0)
		{
			curr_line++;

			if (curr_line == 0)
				curr_line = line_ammo;
		}
		else if (curr_line > 0)
		{
			curr_line--;

			if (curr_line == 0)
				curr_line = line_rest;
		}

		x_prev = x;
		y_prev = y;
	}


}

void ships_render_update(RenderWindow* window)
{
	if (DEBUG_mode && DEBUG_ship_targets)
		render_fovs(window);

	float base_beam_width = 30;
	float variance_percentage = 0.6F;
	int blips = 2;

	int i = 0;
	for (Entity* ship = (Entity*)entities; ship < (Entity*)entities + GAME_ship_count; ship++)
	{
		if (ship_healths[i] <= 0)
		{
			// TODO: render_rotate_alphamod
			// only run physical update; no behavior for dead ship
			ship->render(window);
			render_health(window, ship);
			i++;
			continue;
		}

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
				render_thrust(window, target, { 255, 0, 0, 255 }, 3, 4, 26.0F, true);
			}
			// charging up
			else
			{
				//window->render_rotate(0, 0, 0, 0, ship->screen_x, ship->screen_y - (beam_width >> 1), height, beam_width, 0, beam_width >> 1, ship->rotation, highlighter_beam_texture);
				render_thrust(window, target, { 255, 0, 0, 255 }, 3, 4, 26.0F, true);
			}

			/*char buffer[16] = "";
			sprintf_s(buffer, "%.4f", ship_attack_timers[i]);
			window->render_centered_world(ship->x, ship->y, buffer, encode_sans_medium, { 255, 255, 255, 255 });*/

			// decerements time in ships_update() func already (above is mirror functionality for render stage)


		}
		else if (ship_warn_timers[i] > 0.0F && ship_healths[i] > 0)
		{
			for (int id : *shadowing_targets[i])
			{
				if (notice_timers[i]->find(id) == notice_timers[i]->end() || notice_timers[i]->at(id) <= 0.0F)
					continue;

				Entity* other = (Entity*)entities + id;
				if (other->id != PLAYER_entity_id)
					render_thrust(window, other, { 255, 255, 0, 255 }, 3, 4, 26.0F, true);
				else
					render_dotted_circle(window, other, 10.0F + (float)other->outline_point_count / (2.0F * PI), { 255, 255, 0, 255 });
			}
		}
		//else
			//render_thrust(window, ship, { 0, 255, 255, 255 });

		if (DEBUG_mode && DEBUG_ship_targets)
		{
			float text_height = 30.0F;
			float size = 20.0F;
			window->render_rect((float)target_positions[i].x - size * 0.5F, (float)target_positions[i].y - size * 0.5F, size, size, { 255, 255, 0, 255 });

			if (ship_health_damaged_timers[i] > 0.0F)
			{
				char health_text[8] = "";
				snprintf(health_text, 8, "%.3f", ship_health_damaged_timers[i]);
				window->render_centered_world((float)target_positions[i].x, (float)target_positions[i].y - size * 0.5F - text_height, health_text, encode_sans_medium, { 0, 255, 0, 255 });
				text_height += 30.0F;
			}

			if (ship_warn_timers[i] > 0.0F)
			{
				char warn_text[8] = "";
				snprintf(warn_text, 8, "%.3f, %.3f", ship_warn_timers[i], ship_attack_timers[i]);
				window->render_centered_world((float)target_positions[i].x, (float)target_positions[i].y - size * 0.5F - text_height, warn_text, encode_sans_medium, { 255, 0, 255, 255 });
			}
		}


		ship->render(window);
		render_health(window, ship);
		i++;
	}
	render_notice_bars(window);
}

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

void alert_ship_warning(Entity* ship, Entity* alertee)
{
	set_ship_shadowing_chunk(ship->id, alertee->collision_chunk);
	//if (shadowing_targets[ship->id]->find(alertee->id) != shadowing_targets[ship->id]->end()) // if alertee is in shadowing targets
	//	ship_warn_timers[ship->id] += SHIP_warning_time;
	//else
	ship_warn_timers[ship->id] = SHIP_warning_time;
}

void ship_damage(Entity* ship, int amount)
{
	if (ship_health_damaged_timers[ship->id] <= 0.0F) // hacky invincibility timer
	{
		ship_healths[ship->id] -= amount;
		ship_health_damaged_timers[ship->id] = SHIP_health_damaged_fadetime + SHIP_health_damaged_showtime + SHIP_health_damaged_pulsetime;
	}
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
	SDL_DestroyTexture(pip_texture);
	SDL_DestroyTexture(laser_beam_texture);
	SDL_DestroyTexture(highlighter_beam_texture);
	for (int i = 0; i < GAME_ship_count; i++)
	{
		delete shadowing_targets[i];
		delete notice_timers[i];
	}

	delete[] ship_targets;
	delete[] ship_warn_timers;
	delete[] ship_attack_timers;
}
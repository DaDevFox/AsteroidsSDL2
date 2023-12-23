#include "Entity.h"
#include "../main.h"
#include <string.h>
#include <unordered_map>
#include <set>
#include "asteroid.h"
#include <set>
#include <queue>

const Uint32 asteroid_color_raw = 0xFFFFFFFF;
const Uint32 blank_space_color = 0x00000000;

const int max_asteroid_target_variance = 100;
const int tile_size = max_asteroid_radius;
const int chunk_height = GAME_height / tile_size;
const int chunk_width = GAME_width / tile_size;

std::unordered_map<int, std::set<Entity*>*> collision_check_grid;

int hash_entity(Entity* entity) {
	return ((int)entity->x + (GAME_width) * ((int)entity->y / tile_size)) / tile_size;
}

void Entity::init() 
{

}

void Entity::move()
{
	x += velocity_x * delta_time;
	y += velocity_y * delta_time;

	if (x > GAME_width)
		x -= GAME_width;
	if (x < 0)
		x += GAME_width;
	if (y > GAME_height)
		y -= GAME_height;
	if (y < 0)
		y += GAME_width;

	velocity_x = velocity_x + (desired_velocity_x - velocity_x) * delta_time * movement_windup_speed;
	velocity_y = velocity_y + (desired_velocity_y - velocity_y) * delta_time * movement_windup_speed;

	if (drag_enabled) 
	{
		desired_velocity_x *= (1.0F - drag);
		desired_velocity_y *= (1.0F - drag);
	}

	screen_x = (int)x;
	screen_y = (int)y;
}

void Entity::update_collision_chunk()
{
	int curr_chunk = hash_entity(this);
	if (collision_chunk != curr_chunk)
	{
		if (collision_check_grid[collision_chunk] != NULL)
			(*collision_check_grid[collision_chunk]).erase(this);
		if (collision_check_grid[curr_chunk] == NULL)
			collision_check_grid[curr_chunk] = new std::set<Entity*>();
		collision_check_grid[curr_chunk]->insert(this);

		collision_chunk = curr_chunk;
	}
}

bool collision_between(Entity* a, Entity* b) {
	if (!(a->point_count || b->point_count))
		return false;

	/*set<SDL_Point> used;
	for (SDL_Point* i = a->outline; i < a->outline + a->point_count; i++)
		used.insert({ a->screen_x + i->x, a->screen_y + i->y });

	for (SDL_Point* i = b->outline; i < b->outline + b->point_count; i++)
		if (!used.insert({ b->screen_x + i->x, b->screen_y + i->y }).second)
			return true;*/

	return false;
}

void Entity::check_collisions() 
{
	std::set<Entity*> to_check;
	
	int floor_y = (int)y / tile_size - 1;
	int ceil_y = (int)y / tile_size + 1;
	int left_x = (int)x / tile_size - 1;
	int right_x = (int)x / tile_size + 1;
	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, chunk_height); curr_y++) 
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, chunk_width); curr_x++)
		{
			int chunk = ((int)curr_x + (GAME_width) * ((int)curr_y / tile_size)) / tile_size;
			if (collision_check_grid[chunk] == NULL)
				continue;
			std::set<Entity*> curr_set = *(collision_check_grid[chunk]);
			to_check.insert(curr_set.begin(), curr_set.end());
		}
	}

	for (Entity* other : to_check) 
	{
		if (collision_between(this, other))
		{
			velocity_x = velocity_x - (desired_velocity_x - velocity_x) * delta_time * movement_windup_speed;
			velocity_y = velocity_y - (desired_velocity_y - velocity_y) * delta_time * movement_windup_speed;

			x -= velocity_x * delta_time;
			y -= velocity_y * delta_time;

			screen_x = (int)x;
			screen_y = (int)y;

			return;
		}
	}
}


void Entity::update()
{
	move();
	update_collision_chunk();
	check_collisions();
}




Asteroid::Asteroid(const char* texture_file_path, int w, int h) {
	init(texture_file_path, w, h);
}

void Asteroid::init(const char* texture_file_path, int w, int h) {
	x = 0.0F;
	y = 0.0F;
	velocity_x = 0.0F;
	velocity_y = 0.0F;
	desired_velocity_x = 0.0F;
	desired_velocity_y = 0.0F;
	
	angle = 0.0F;

	movement_windup_speed = 0.005f;
	mass = 1.0F;
	drag = 0.000025F;

	screen_x = 0;
	screen_y = 0;

	target_offset_x = rand() % max_asteroid_target_variance - max_asteroid_target_variance/2;
	target_offset_y = rand() % max_asteroid_target_variance - max_asteroid_target_variance / 2;

	//texture = window.load_texture(texture_file_path);
	this->w = w;
	this->h = h;

	generate();
}


int pixel_to_index(int x, int y, int w) {
	return x + y * w;
}

std::pair<int, int> index_to_pixel(int idx, int w) {
	return { idx % w, idx / w };
}

void Asteroid::generate() 
{
	SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(temp_surface);

	std::queue<int> open;
	std::set<int> visited;
	open.push(pixel_to_index(w / 2, h / 2, w));

	Uint32* buffer = (Uint32*)temp_surface->pixels;

	while (!open.empty())
	{
		std::pair<int, int> curr = index_to_pixel(open.front(), w);
		if (curr.first < 0 || curr.first >= w || curr.second < 0 || curr.second >= h)
		{
			open.pop();
			continue;
		}
		*(buffer + open.front()) = asteroid_color_raw;

		open.pop();

		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first + 1, curr.second, w)) == visited.end())
			open.push(pixel_to_index(curr.first + 1, curr.second, w));
		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first - 1, curr.second, w)) == visited.end())
			open.push(pixel_to_index(curr.first - 1, curr.second, w));
		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first, curr.second + 1, w)) == visited.end())
			open.push(pixel_to_index(curr.first, curr.second + 1, w));
		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first, curr.second - 1, w)) == visited.end())
			open.push(pixel_to_index(curr.first, curr.second - 1, w));
	}

	std::vector<SDL_Point> outline;
	for (int curr_x = 0; curr_x < w; curr_x++) 
	{
		for (int curr_y = 0; curr_y < h; curr_y++)
		{
			if (*(buffer + pixel_to_index(curr_x, curr_y, w)) != asteroid_color_raw)
				continue;

			if (curr_x + 1 < w && 
				curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y - 1, w)) == blank_space_color)
				outline.push_back({ curr_x + 1, curr_y - 1 });
			if (curr_x + 1 < w &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == blank_space_color)
				outline.push_back({ curr_x + 1, curr_y });
			if (curr_x + 1 < w &&
				curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y + 1, w)) == blank_space_color)
				outline.push_back({ curr_x + 1, curr_y + 1 });
			if (curr_x - 1 > 0 &&
				curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y + 1, w)) == blank_space_color)
				outline.push_back({ curr_x - 1, curr_y + 1 });
			if (curr_x - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == blank_space_color)
				outline.push_back({ curr_x - 1, curr_y });
			if (curr_x - 1 > 0 &&
				curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y - 1, w)) == blank_space_color)
				outline.push_back({ curr_x - 1, curr_y });
			if (curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == blank_space_color)
				outline.push_back({ curr_x, curr_y + 1 });
			if (curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == blank_space_color)
				outline.push_back({ curr_x, curr_y - 1 });
		}
	}

	if (outline.size() < SETTING_MAX_POLYGON_VERTICES)
	{
		for (int i = 0; i < outline.size(); i++) 
		{
			this->outline[i] = outline.at(i);
		}
	}
	else
		SDL_Log("ERROR: outline buffer overflow for asteroid.\n");

	SDL_UnlockSurface(temp_surface);

	texture = window.create_texture_from_surface(temp_surface);

	SDL_FreeSurface(temp_surface);
}

void Asteroid::cleanup() {
	SDL_DestroyTexture(texture);
}

bool Asteroid::in_bounds(int screen_x, int screen_y) {
	return
		screen_x >= (this->screen_x - w / 2) && screen_x <= (this->screen_x + w / 2) &&
		screen_y >= (this->screen_y - h / 2) && screen_y <= (this->screen_y + h / 2);
}

void Asteroid::render(RenderWindow *window) 
{
	//char str[8];/*
	//sprintf_s(str, "%i", hash_entity(this));
	//SDL_Color color = 
	//{
	//	255, 255, 255, 255 
	//};
	//window->render_centered_world(x, y - (float)h/2 - 10.0F, str, encode_sans_medium, color);*/

	window->render_rotate(0, 0, 0, 0, screen_x - w/2, screen_y - h/2, w, h, angle, texture);
}
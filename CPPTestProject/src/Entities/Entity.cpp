#include "../main.h"
#include <string.h>
#include <map>
#include "asteroid.h"
#include <set>
#include <unordered_set>
#include <queue>
#include <vector>
#include <iostream>

const int tile_size = ASTEROID_maximum_radius;
const int chunk_height = GAME_height / tile_size;
const int chunk_width = GAME_width / tile_size;

std::map<int, std::set<int>*> collision_check_grid;

int Entity::curr_id = 0;

std::vector<Entity*> Entity::active;

std::set<int> scratch;

//EntityManager::EntityManager() {
//	registry = new std::vector<Entity>();
//}
//
//EntityManager::EntityManager(int count) {
//	registry = new std::vector<Entity>();
//	registry->reserve(count);
//	for(int i = 0; i < count; i++)
//		create_entity();
//}
//
//EntityManager::~EntityManager() {
//	delete registry;
//}
//
//Entity& EntityManager::create_entity() {
//	registry->emplace_back();
//
//	return registry->back();
//}
//
////Asteroid& EntityManager::create_asteroid() {
////
////}
//
//Entity& EntityManager::get_entity(int id) {
//	return (*registry)[id];
//}

//Asteroid& EntityManager::get_asteroid(int id) {
//
//}

int hash(int x, int y);

int hash_entity(Entity* entity) {
	return ((int)entity->x + (GAME_width) * ((int)entity->y / tile_size)) / tile_size;
}

Entity::Entity() :
	id(curr_id++),

	x(0.0F),
	y(0.0F),
	w(512),
	h(512),

	velocity_x(0.0F),
	velocity_y(0.0F),

	desired_velocity_x(0.0F),
	desired_velocity_y(0.0F),

	drag_enabled(true),

	screen_x(0),
	screen_y(0),

	center_x(0),
	center_y(0),

	movement_windup_speed(0.005f),
	drag(0.0001F),
	mass(1.0F),

	texture(nullptr),
	collision_chunk(0),

	outline_point_count(0),
	outline()
{
	active.push_back(this);
	std::cout << "entity active list count: " << active.size() << std::endl;
}

Entity::Entity(const Entity& copy)
	: id(copy.curr_id),

	x(copy.x),
	y(copy.y),
	w(copy.w),
	h(copy.h),

	velocity_x(copy.velocity_x),
	velocity_y(copy.velocity_y),

	desired_velocity_x(copy.desired_velocity_x),
	desired_velocity_y(copy.desired_velocity_y),

	drag_enabled(copy.drag_enabled),

	screen_x(copy.screen_x),
	screen_y(copy.screen_y),

	center_x(copy.center_x),
	center_y(copy.center_y),

	movement_windup_speed(copy.movement_windup_speed),
	drag(copy.drag),
	mass(copy.mass),

	texture(copy.texture),
	collision_chunk(copy.collision_chunk),

	outline_point_count(copy.outline_point_count),
	outline()
{
	std::cout << "copied entity!" << std::endl;
}

void Entity::init() 
{

}

void Entity::move()
{
	velocity_x = velocity_x + (desired_velocity_x - velocity_x) * delta_time * movement_windup_speed;
	velocity_y = velocity_y + (desired_velocity_y - velocity_y) * delta_time * movement_windup_speed;

	if (drag_enabled) 
	{
		desired_velocity_x *= (1.0F - drag);
		desired_velocity_y *= (1.0F - drag);
	}

	x += velocity_x * delta_time;
	y += velocity_y * delta_time;

	if (x > GAME_width)
		x = (int)x % GAME_width;
	if (x < 0)
		x += GAME_width;
	if (y > GAME_height)
		y = (int) y % GAME_height;
	if (y < 0)
		y += GAME_width;

	screen_x = (int)x;
	screen_y = (int)y;
}

void Entity::update_collision_chunk()
{
	int curr_chunk = hash_entity(this);
	if (collision_chunk != curr_chunk)
	{
		if (collision_check_grid[collision_chunk] != nullptr)
			(*collision_check_grid[collision_chunk]).erase(id);
		if (collision_check_grid[curr_chunk] == nullptr)
			collision_check_grid[curr_chunk] = new std::set<int>();

		collision_check_grid[curr_chunk]->insert(id);
		collision_chunk = curr_chunk;
	}

	window.render_rect((screen_x / tile_size) * tile_size, (screen_y / tile_size) * tile_size, tile_size, tile_size, {100, 100, 100, 20});
}

bool collision_between(Entity* a, Entity* b, SDL_Point *hit) {
	if (!(a->outline_point_count || b->outline_point_count))
		return false;
	
	scratch.clear();
	
	int ax = a->screen_x - (a->w >> 1);
	int ay = a->screen_y - (a->h >> 1);
	int bx = b->screen_x - (b->w >> 1);
	int by = b->screen_y - (b->h >> 1);


	for (SDL_Point* i = a->outline; i < a->outline + a->outline_point_count; i++)
	{
		scratch.insert(hash(ax + i->x, ay + i->y));
		window.render_point(ax + i->x, ay + i->y, {0, 0, 255, 255});
	}
	
	for (SDL_Point* i = b->outline; i < b->outline + b->outline_point_count; i++)
	{
		int hashed = hash(bx + i->x, by + i->y);
		//for(int idx = 0; idx < scratch.size(); idx++)
		//	if(scratch[idx] == hashed)//TODO: sorting + binary search for vector
		if(scratch.find(hashed) != scratch.end())
		{
			*hit = SDL_Point{ bx + i->x, by + i->y };
			return true;
		}
	}

	return false;
}

int hash(int x, int y) {
	return x + GAME_width * y;
}

void Entity::check_collisions() 
{
	std::set<int> to_check;
	
	int floor_y = (int)y / tile_size - 1;
	int ceil_y = (int)y / tile_size + 1;
	int left_x = (int)x / tile_size - 1;
	int right_x = (int)x / tile_size + 1;
	for (int curr_y = SDL_max(floor_y, 0); curr_y < SDL_min(ceil_y, chunk_height); curr_y++) 
	{
		for (int curr_x = SDL_max(left_x, 0); curr_x < SDL_min(right_x, chunk_width); curr_x++)
		{
			int chunk = curr_x + (GAME_width/tile_size) * curr_y;
			if (collision_check_grid[chunk] == nullptr)
				continue;
			std::set<int> curr_set = *(collision_check_grid[chunk]);
			to_check.insert(curr_set.begin(), curr_set.end());
		}
	}

	char text[8];
	sprintf_s(text, "%i", to_check.size());
	window.render_centered_world(x, y, text, encode_sans_medium, {255, 255, 255, 255});

	for (int otherID : to_check) 
	{
		if(otherID == id)
			continue;

		Entity* other = active[otherID];
		SDL_Point hit;
		if (collision_between(this, other, &hit))
		{
			// EXPENSIVE IKIK
			float other_x = other->x + (other->center_x);
			float other_y = other->y + (other->center_y);

			float this_x = x + (center_x);
			float this_y = y + (center_y);

			float diff_x = other_x - this_x;
			float diff_y = other_y - this_y;

			float magnitude = sqrt(diff_x * diff_x + diff_y * diff_y);
			diff_x /= magnitude;
			diff_y /= magnitude;

			float explosive_force = 0.001F;

			other->velocity_x *= diff_x;
			other->velocity_y *= diff_y;

			other->velocity_x += diff_x * explosive_force * delta_time;
			other->velocity_y += diff_y * explosive_force * delta_time;

			velocity_x *= -diff_x;
			velocity_y *= -diff_y;

			velocity_x += -diff_x * explosive_force * delta_time;
			velocity_y += -diff_y * explosive_force * delta_time;

			desired_velocity_x = velocity_x;
			desired_velocity_y = velocity_y;

			other->desired_velocity_x = other->velocity_x;
			other->desired_velocity_y = other->velocity_y;

			other->x -= other->velocity_x * delta_time * explosive_force;
			other->y -= other->velocity_y * delta_time * explosive_force;

			x -= velocity_x * delta_time * explosive_force;
			y -= velocity_y * delta_time * explosive_force;



			// full equation for perfectly elastic collision: 
			// https://phys.libretexts.org/Courses/Joliet_Junior_College/Physics_201_-_Fall_2019v2/Book%3A_Custom_Physics_textbook_for_JJC/09%3A_Linear_Momentum_and_Collisions/9.16%3A_Collisions#:~:text=If%20two%20particles%20are%20involved,m1)v2i.

			/*float velocity_x_after = 
				( (mass - other->mass) * velocity_x
				+ (2 * other->mass)    * other->velocity_x ) 
				/ (mass + other->mass);
			
			float velocity_y_after = 
				( (mass - other->mass) * velocity_y
				+ (2 * other->mass)    * other->velocity_y ) 
				/ (mass + other->mass);*/

			//// undo movement into collision

			// conduct extra separation movement



			/*desired_velocity_x = velocity_x_after;
			desired_velocity_y = velocity_y_after;

			velocity_x = velocity_x_after;
			velocity_y = velocity_y_after;*/			

			return;
		}
	}
}


void Entity::render(RenderWindow* window)
{
	if(DEBUG_entity_outlines)
		window->render_rect_outline(screen_x - (w >> 1), screen_y - (h >> 1), w, h, { 100, 100, 100, 100 });
	window->render(0, 0, 0, 0, screen_x - (w >> 1), screen_y - (h >> 1), w, h, texture);
	
}

void Entity::update()
{
	move();
	update_collision_chunk();
	check_collisions();
}

void entities_init() {
	entities = new Entity[GAME_ship_count + GAME_asteroid_count];



	asteroids_init();
}

void entities_cleanup() {
	//TODO

	asteroids_cleanup();

	delete[] entities;
}


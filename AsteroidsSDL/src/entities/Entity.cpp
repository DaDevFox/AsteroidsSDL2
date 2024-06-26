#include "../main.h"
#include <map>
#include "asteroid.h"
#include <set>
#include <vector>
#include <iostream>
#include "ship.h"

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

int hash_entity(Entity* entity)
{
	return ((int)entity->x + (GAME_width) * ((int)entity->y / chunk_size)) / chunk_size;
}

bool Entity::in_bounds(float world_x, float world_y) const
{
	return
		world_x >= (x - (float)(w >> 1)) && world_x <= (x + (float)(w >> 1)) &&
		world_y >= (y - (float)(h >> 1)) && world_y <= (y + (float)(h >> 1));
}

Entity::Entity() :
	id(curr_id++),

	x(0.0F),
	y(0.0F),
	w(SETTING_ENTITY_DIMENSION),
	h(SETTING_ENTITY_DIMENSION),

	velocity_x(0.0F),
	velocity_y(0.0F),

	desired_velocity_x(0.0F),
	desired_velocity_y(0.0F),

	screen_x(0),
	screen_y(0),

	center_x(0),
	center_y(0),

	movement_windup_speed(0.005f),
	rotation_windup_speed(0.02f),
	drag(SETTING_default_entity_drag),
	mass(1.0F),

	desired_rotation(0.0),
	rotation(0.0),

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

	screen_x(copy.screen_x),
	screen_y(copy.screen_y),

	center_x(copy.center_x),
	center_y(copy.center_y),

	rotation_windup_speed(copy.rotation_windup_speed),
	movement_windup_speed(copy.movement_windup_speed),
	drag(copy.drag),
	mass(copy.mass),

	desired_rotation(copy.desired_rotation),
	rotation(copy.rotation),

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

void Entity::on_collision(Entity* other, int collision_x, int collision_y)
{
	if (other->id == PLAYER_entity_id) // other is asteroid
	{
		float crit_vel = 0.02F;
		if (velocity_x * velocity_x + velocity_y * velocity_y >= crit_vel * crit_vel)
			ship_damage(this, 1);
	}
}

void Entity::move()
{
	velocity_x = velocity_x + (desired_velocity_x - velocity_x) * delta_time * movement_windup_speed;
	velocity_y = velocity_y + (desired_velocity_y - velocity_y) * delta_time * movement_windup_speed;

	rotation = rotation + (desired_rotation - rotation) * delta_time * rotation_windup_speed;

	desired_velocity_x *= (1.0F - drag);
	desired_velocity_y *= (1.0F - drag);


	x += velocity_x * delta_time;
	y += velocity_y * delta_time;

	if (x > GAME_width)
		x = (int)x % GAME_width;
	if (x < 0)
		x += GAME_width;
	if (y > GAME_height)
		y = (int)y % GAME_height;
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

	window.render_rect((screen_x / chunk_size) * chunk_size, (screen_y / chunk_size) * chunk_size, chunk_size, chunk_size, { 100, 100, 100, 20 });
}

bool collision_between(Entity* a, Entity* b, SDL_Point* hit)
{
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
		window.render_point(ax + i->x, ay + i->y, { 0, 0, 255, 255 });
	}

	for (SDL_Point* i = b->outline; i < b->outline + b->outline_point_count; i++)
	{
		int hashed = hash(bx + i->x, by + i->y);
		//for(int idx = 0; idx < scratch.size(); idx++)
		//	if(scratch[idx] == hashed)//TODO: sorting + binary search for vector
		if (scratch.find(hashed) != scratch.end())
		{
			*hit = SDL_Point{ bx + i->x, by + i->y };
			return true;
		}
	}

	return false;
}

int hash(int x, int y)
{
	return x + GAME_width * y;
}

void Entity::check_collisions()
{
	std::set<int> to_check;

	int floor_y = (int)y / chunk_size - 1;
	int ceil_y = (int)y / chunk_size + 1;
	int left_x = (int)x / chunk_size - 1;
	int right_x = (int)x / chunk_size + 1;
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

	for (int otherID : to_check)
	{
		if (otherID == id)
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

			on_collision(other, hit.x, hit.y);

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
	if (DEBUG_mode && (DEBUG_display_entity_outlines || DEBUG_wireframe_mode))
	{
		window->render_rect_outline(screen_x - (w >> 1), screen_y - (h >> 1), w, h, { 100, 100, 100, 100 });
		for (int i = 0; i < outline_point_count; i++)
			window->render_rect((float)(screen_x - (w >> 1) + outline[i].x), (float)(screen_y - (h >> 1) + outline[i].y), 1.0F, 1.0F, { 0, 0, 255, 255 });
	}

	if (DEBUG_mode && DEBUG_wireframe_mode && id == GAME_ship_count + GAME_asteroid_count - 1)
		window->render_rect_outline(screen_x - (w >> 1), screen_y - (h >> 1), w, h, { 255, 0, 0, 255 });

	if (DEBUG_mode && DEBUG_entity_rotations)
	{
		char str[20] = "";
		snprintf(str, 20, "%.4f", rotation);
		window->render_centered_world(x, y + 50.0F, str, encode_sans_medium, { 255, 255, 255, 255 });
	}

	if (DEBUG_mode && DEBUG_chunk_gridlines)
	{
		char str[20] = "";
		snprintf(str, 20, "%i", collision_chunk);
		window->render_centered_world(x, y + 50.0F, str, encode_sans_medium, { 255, 255, 255, 255 });
	}

	if ((DEBUG_mode && DEBUG_wireframe_mode))
		return;

	if (rotation == 0.0)
		window->render(0, 0, 0, 0, screen_x - (w >> 1), screen_y - (h >> 1), w, h, texture);
	else
		window->render_rotate(0, 0, 0, 0, screen_x - (w >> 1), screen_y - (h >> 1), w, h, rotation, texture);

}

void Entity::update()
{
	move();
	update_collision_chunk();
	check_collisions();
}

void entities_init()
{
	if (sizeof(Entity) != sizeof(Asteroid))
	{
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, "entity and asteroid size mismatch; continuous memory block allocation impossible");
		return;
	}

	entities = malloc((GAME_ship_count + GAME_asteroid_pool_size) * sizeof(Entity));
	new(entities) Entity[GAME_ship_count];
	new((Entity*)entities + GAME_ship_count) Asteroid[GAME_asteroid_pool_size];

	ships_init();
	asteroids_init();
}

void entities_cleanup()
{
	//TODO

	ships_cleanup();
	asteroids_cleanup();

	free(entities);
}


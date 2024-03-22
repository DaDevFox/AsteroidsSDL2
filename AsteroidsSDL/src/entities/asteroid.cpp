#define INNER_ASTEROID_OUTLINES

#include "../main.h"
#include "asteroid.h"

#include <queue>
#include <set>
#include <map>

#include <random>
#include <iostream>

float mass_per_pixel = (ASTEROID_maximum_mass - ASTEROID_minimum_mass) / (4.0F * ASTEROID_maximum_radius * ASTEROID_maximum_radius);
const char* asteroid_texture_path = "./circle.png";


int controlled_asteroid = 0;

#pragma region deprecated

float speed = 1.0F;

int downup = 0;
int leftright = 0;

bool primary_held;

int mouse_diff_threshold_squared = 125;

const int max_asteroid_target_variance = 100;

#pragma endregion

/*

CONCEPT:

you are an asteroid in an asteroid field
most asteroids cannot control their movement (initial velocity + drag and collisions)
you are sentient, and can move

the 'players' (ships) hunt you, but you can see their field of view and know they can't differentiate you from other asteroids
unless they see you move
knock them from behind to destroy them
survive and destroy all players by the time limit

*/

/*

phys





*/

Asteroid::Asteroid()
{
	init(ASTEROID_maximum_radius * 2, ASTEROID_maximum_radius * 2);
}

void Asteroid::init(int w, int h)
{
	this->w = w;
	this->h = h;

	generate();
}


int pixel_to_index(int x, int y, int w)
{
	return x + y * w;
}

std::pair<int, int> index_to_pixel(int idx, int w)
{
	return { idx % w, idx / w };
}

void Asteroid::rand_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count)
{
	std::queue<int> open;
	std::set<int> visited;

	open.push(pixel_to_index((w >> 1), (h >> 1), w));

	*pixel_count = 0;
	*leftmost_x = w;
	*leftmost_y = h;

	while (!open.empty())
	{
		std::pair<int, int> curr = index_to_pixel(open.front(), w);
		if (curr.first < 0 || curr.first >= w || curr.second < 0 || curr.second >= h)
		{
			open.pop();
			continue;
		}

		*(buffer + open.front()) = GAME_asteroid_color_raw;
		(*pixel_count)++;
		open.pop();

		if (curr.first < *leftmost_x)
		{
			*leftmost_x = curr.first;
			*leftmost_y = curr.second;
		}

		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first + 1, curr.second, w)) == visited.end())
			open.push(pixel_to_index(curr.first + 1, curr.second, w));
		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first - 1, curr.second, w)) == visited.end())
			open.push(pixel_to_index(curr.first - 1, curr.second, w));
		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first, curr.second + 1, w)) == visited.end())
			open.push(pixel_to_index(curr.first, curr.second + 1, w));
		if (rand() % 4 == 0 && visited.find(pixel_to_index(curr.first, curr.second - 1, w)) == visited.end())
			open.push(pixel_to_index(curr.first, curr.second - 1, w));
	}
}


void Asteroid::circle_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count)
{
	*pixel_count = 0;
	int reach_max_variance = 10;

	int octaves = 1;
	int min_reach_initial = 5;
	int max_reach_initial = ((2 * ASTEROID_maximum_radius) / 5) + (rand() % (2 * reach_max_variance + 1) - reach_max_variance);
	int variance = 1;

	float theta;
	// lines per circle
	float resolution = 360.0F;
	float step = 2.0F * PI / resolution;

	float bridge_resolution = (2.0F * PI) / 90.0F;
	float bridge_current = 0.0F;

	int center_x = w / 2;
	int center_y = h / 2;


	int reach = rand() % (max_reach_initial - min_reach_initial) + min_reach_initial;
	int reach_initial = reach;

	for (theta = 0; theta < 2.0F * octaves * PI; theta += step)
	{
		if (bridge_current > bridge_resolution)
			reach += rand() % (2 * variance + 1) - variance;

		reach = SDL_max(SDL_min(reach, max_reach_initial), 0);

		int curr_reach = reach + (reach_initial - reach) * (theta / (2.0F * PI));
		for (int i = 0; i < curr_reach; i++)
		{
			int offset_x = cosf(theta) * i;
			int offset_y = sinf(theta) * i;

			*(buffer + pixel_to_index(center_x + offset_x, center_y + offset_y, w)) = GAME_asteroid_color_raw;
			pixel_count++;
		}

		bridge_current += theta;
	}
}

void Asteroid::generate()
{
	SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(temp_surface); // locks the surface from outside read/writing; so we can write to it

	Uint32* buffer = (Uint32*)temp_surface->pixels;

	int pixels;
	int leftmost_x;
	int leftmost_y;

	circle_expand_fill(buffer, &leftmost_x, &leftmost_y, &pixels);

	point_count = pixels;
	mass = ASTEROID_minimum_mass + mass_per_pixel * pixels;

	create_outline(buffer);

	SDL_UnlockSurface(temp_surface); // allow external read/writes when finished writing

	texture = window.create_texture_from_surface(temp_surface);

	SDL_FreeSurface(temp_surface);
}

bool const operator<(const SDL_Point a, const SDL_Point b)
{
	return hash(a.x, a.y) - hash(b.x, b.y) > 0;
}

bool const operator>(const SDL_Point a, const SDL_Point b)
{
	return hash(a.x, a.y) - hash(b.x, b.y) < 0;
}

bool const operator==(const SDL_Point a, const SDL_Point b)
{
	return hash(a.x, a.y) - hash(b.x, b.y) == 0;
}

bool const operator!=(const SDL_Point a, const SDL_Point b)
{
	return hash(a.x, a.y) - hash(b.x, b.y) != 0;
}

SDL_Point operator+(SDL_Point a, SDL_Point b)
{
	return { a.x + b.x, a.y + b.y };
}

SDL_Point operator/(SDL_Point a, SDL_Point b)
{
	return { a.x / b.x, a.y / b.y };
}

SDL_Point operator/(SDL_Point a, int b)
{
	return { a.x / b, a.y / b };
}

bool outline_contains(SDL_Point* outline, int outline_point_count, SDL_Point point);

void Asteroid::fill_pixels_from_outline(Asteroid* asteroid)
{
	SDL_DestroyTexture(asteroid->texture);
	SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(temp_surface); // locks the surface from outside read/writing; so we can write to it

	Uint32* buffer = (Uint32*)temp_surface->pixels;

	// ADD OUTLINE POINTS
	std::set<SDL_Point> added;
	if (asteroid->outline_point_count > 0)
	{
		added.insert(asteroid->outline, asteroid->outline + asteroid->outline_point_count);

		// get start pos for flood fill (CoM of asteroid)
		// DANGER: designed for circular asteroids only, IE asteroids with no/few protrusions
		SDL_Point start = { 0, 0 };
		for (int i = 0; i < asteroid->outline_point_count; i++)
			start = start + asteroid->outline[i];
		start = start / asteroid->outline_point_count;


		// FLOOD FILL TO ADD INTERIOR POINTS
		std::set<SDL_Point> open_set;
		open_set.insert(start);
		while (open_set.size() > 0)
		{
			SDL_Point current = *open_set.begin();
			//TODO: figure out why it doesn't fill above (+x, +y) start point
			if (current.y >= ASTEROID_maximum_radius * 2 || current.y < 0 || current.x < 0 || current.x > ASTEROID_maximum_radius * 2)
			{
				open_set.erase({ current.x, current.y });
				continue;
			}

			if (added.find(SDL_Point{ current.x, current.y + 1 }) == added.end()
				&& open_set.find(SDL_Point{ current.x, current.y + 1 }) == open_set.end()
				&& current.y + 1 < ASTEROID_maximum_radius * 2) // neighbor is not in added set or open set
				open_set.insert(SDL_Point{ current.x, current.y + 1 });
			if (added.find(SDL_Point{ current.x, current.y - 1 }) == added.end()
				&& open_set.find(SDL_Point{ current.x, current.y - 1 }) == open_set.end()
				&& current.y - 1 > 0)
				open_set.insert(SDL_Point{ current.x, current.y - 1 });
			if (added.find(SDL_Point{ current.x + 1, current.y }) == added.end()
				&& open_set.find(SDL_Point{ current.x + 1, current.y }) == open_set.end()
				&& current.x + 1 < ASTEROID_maximum_radius * 2)
				open_set.insert(SDL_Point{ current.x + 1, current.y });
			if (added.find(SDL_Point{ current.x - 1, current.y }) == added.end()
				&& open_set.find(SDL_Point{ current.x - 1, current.y }) == open_set.end()
				&& current.x - 1 > 0)
				open_set.insert(SDL_Point{ current.x - 1, current.y });

			added.insert(current);
			open_set.erase(current);
		}
	}

	// WRITE ALL ADDED 
	auto iterator = added.begin();
	while (iterator != added.end())
	{
		*(buffer + pixel_to_index(iterator->x, iterator->y, w)) = GAME_asteroid_color_raw;

		iterator++;
	}

	SDL_UnlockSurface(temp_surface); // allow external read/writes when finished writing

	asteroid->texture = window.create_texture_from_surface(temp_surface);

	SDL_FreeSurface(temp_surface);
}

// TODO: remove debug flag
int splitflag = 0;

bool outline_contains(SDL_Point* outline, int outline_point_count, SDL_Point point)
{
	for (SDL_Point* current = outline; current < outline + outline_point_count; current++)
		if (current->x == point.x && current->y == point.y)
			return true;
	return false;
}


Asteroid* Asteroid::split(float collision_x, float collision_y, float collision_rel_velocity)
{
	SDL_Point start;
	SDL_Point end;

	Asteroid* created = split_separate_init(collision_x, collision_y, &start, &end);
	std::vector<SDL_Point> outline_bridge;

	split_bridge_outline(this, start, end, &outline_bridge);
	split_bridge_outline(created, start, end, &outline_bridge);

	fill_pixels_from_outline(this);
	fill_pixels_from_outline(created);

	SDL_Log("created asteroid: %i; added %zu", created->outline_point_count, outline_bridge.size());

	created->x = x + 1;
	created->y = y + 1;

	created->velocity_x = velocity_x;
	created->velocity_y = velocity_y;
	created->desired_velocity_x = desired_velocity_x;
	created->desired_velocity_y = desired_velocity_y;

	SDL_Log("split asteroid idx: %i; has %i points; origin index %i", created->id, created->outline_point_count, id);
	return created;
}

Asteroid* Asteroid::split_separate_init(float collision_x, float collision_y, SDL_Point* start, SDL_Point* end)
{
	int collision_pixel_x = (int)collision_x - (screen_x - (w >> 1));
	int collision_pixel_y = (int)collision_y - (screen_y - (h >> 1));

	int contact_idx = -1;

	for (int i = 0; i < outline_point_count; i++)
		if (outline[i].x == collision_pixel_x && outline[i].y == collision_pixel_y)
			contact_idx = i;

	if (contact_idx == -1)
		return nullptr;

	Asteroid* created = append_asteroid_to_pool();

	int split_endpoint = 0;
	while (split_endpoint < outline_point_count
		&& ((split_endpoint == contact_idx || outline[split_endpoint].x == outline[contact_idx].x)
			|| (outline[split_endpoint].x - collision_pixel_x) * (outline[split_endpoint].x - collision_pixel_x) + (outline[split_endpoint].y - collision_pixel_y) * (outline[split_endpoint].y - collision_pixel_y) < (int)((double)outline_point_count / (2.0 * M_PI)) * (int)((double)outline_point_count / (2.0 * M_PI))))
		split_endpoint++;

	if (split_endpoint == outline_point_count)
		while (split_endpoint == outline_point_count || !(split_endpoint != contact_idx && outline[split_endpoint].x != outline[contact_idx].x))
			split_endpoint = rand() % outline_point_count;

	SDL_Log("split command: (%i, %i) to (%i, %i)", collision_pixel_x, collision_pixel_y, outline[split_endpoint].x, outline[split_endpoint].y);

	int ax = collision_pixel_x;
	int ay = collision_pixel_y;
	int bx = outline[split_endpoint].x;
	int by = outline[split_endpoint].y;

	*start = { ax, ay };
	*end = { bx, by };

	// use y > mx + b to sort points to either resultant asteroid
	float m = (float)(by - ay) / (float)(bx - ax);
	int b = (int)((float)ay - (float)ax * m);
	int divider = 0;
	auto comparator = [m, b](SDL_Point self, SDL_Point other)->bool
		{
			bool self_in = ((float)self.y > (m * (float)self.x + (float)b));
			bool other_in = ((float)other.y > (m * (float)other.x + (float)b));

			return self_in > other_in;
		};

	std::priority_queue < SDL_Point, std::vector<SDL_Point>, decltype(comparator)> queue(comparator);

	for (int i = 0; i < outline_point_count; i++)
	{
		bool in = (outline[i].y > (int)(m * (float)outline[i].x + (float)b));
		//SDL_Log("p (%i, %i): %i", outline[i].x, outline[i].y, in ? 1 : 0);
		if (!in)
			divider++;
		queue.push(outline[i]);
	}


	created->outline_point_count = divider;
	for (int i = 0; i < divider; i++)
	{
		created->outline[i] = (SDL_Point)queue.top();
		queue.pop();
	}

	outline_point_count = queue.size();
	int i = 0;
	while (!queue.empty())
	{
		outline[i] = queue.top();
		queue.pop();
		i++;
	}

	return created;
}

void Asteroid::split_bridge_outline(Asteroid* asteroid, const SDL_Point& start, const SDL_Point& end, std::vector<SDL_Point>* outline_additions)
{
	//std::vector<SDL_Point> outline_additions;

	if (outline_additions->size() == 0)
	{
		int cragginess_resolution = 10;
		int diff_x = (end.x - start.x);
		int diff_y = (end.y - start.y);

		float total_distance = (float)(diff_x * diff_x + diff_y * diff_y);
		total_distance = sqrtf(total_distance);
		int steps = (int)total_distance / cragginess_resolution;

		int curr_x = start.x;
		int curr_y = start.y;

		int variance = 3;

		for (int i = 1; i <= steps; i++)
		{
			int next_x = start.x + (int)((float)diff_x * ((float)i / (float)steps));
			int next_y = start.y + (int)((float)diff_y * ((float)i / (float)steps));

			if (next_x != end.x && next_y != end.y)
			{
				next_x += rand() % variance;
				next_y += rand() % variance;
			}
			else
				SDL_Log("reached end on step %d of %d for asteroid w/ id %d; (%d, %d) to (%d, %d)", i, steps, asteroid->id, curr_x, curr_y, next_x, next_y);

			// raster traverse from curr to next
			int distance = (next_x - curr_x) * (next_x - curr_x) + (next_y - curr_y) * (next_y - curr_y);
			distance = SDL_sqrtf((float)distance);
			float theta = atan2f((next_y - curr_y), (next_x - curr_x));

			int x_prev = -1;
			int y_prev = -1;
			int x, y;

			float resolution = 0.5F;

			for (float j = 0; j <= distance + 2; j += resolution)
			{
				x = (int)(cosf(theta) * j) + curr_x;
				y = (int)(sinf(theta) * j) + curr_y;

				if (x_prev == x && y_prev == y)
					continue;

				outline_additions->push_back({ x, y });

				if (!(x_prev == -1 || y_prev == -1) && x_prev != x && y_prev != y) // if it's self perfect diagonal
					outline_additions->push_back({ x,  y_prev }); // add corner point (could also be x_prev, y)

				x_prev = x;
				y_prev = y;
			}

			curr_x = next_x;
			curr_y = next_y;
		}
	}

	if (asteroid->outline_point_count + outline_additions->size() >= 4 * SETTING_MAX_POINT_COUNT)
	{
		SDL_Log("Outline buffer overflow for asteroid id %i", id);
		return;
	}

	for (int i = asteroid->outline_point_count; i < asteroid->outline_point_count + outline_additions->size(); i++)
		asteroid->outline[i] = (*outline_additions)[i - asteroid->outline_point_count];


	asteroid->outline_point_count += outline_additions->size();
}



Asteroid* append_asteroid_to_pool()
{
	return (Asteroid*)entities + GAME_ship_count + GAME_asteroid_count++;
}


void Asteroid::create_outline(Uint32* buffer)
{
#ifndef INNER_ASTEROID_OUTLINES

	std::vector<SDL_Point> outline;

#pragma region deprecated

	// wind clockwise
	//visited.clear();
	//open.push(pixel_to_index(leftmost_x, leftmost_y, w));
	//std::pair<int, int> previous;
	//std::pair<int, int> curr = {-1, -1};
	//while (!open.empty()) 
	//{
	//	bool curr_outline = false;
	//	previous = curr;
	//	curr = index_to_pixel(open.front(), w);
	//	int curr_x = curr.first;
	//	int curr_y = curr.second;
	//	open.pop();

	//	// loop complete! (literally/geometrically lol)
	//	if (outline.size() > 0 && curr_x == leftmost_x && curr_y == leftmost_y) 
	//		break;
	//	

	//	if (visited.find(pixel_to_index(curr_x, curr_y, w)) != visited.end())
	//		continue;

	//	if (*(buffer + pixel_to_index(curr_x, curr_y, w)) != GAME_asteroid_color_raw)
	//		continue;

	//	if (curr_x + 1 < w &&
	//		curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y - 1, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	if (curr_x + 1 < w &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	if (curr_x + 1 < w &&
	//		curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y + 1, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	if (curr_x - 1 > 0 &&
	//		curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y + 1, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	if (curr_x - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	if (curr_x - 1 > 0 &&
	//		curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y - 1, w)) == GAME_blank_space_color)
	//		curr_outline = true;;

	//	if (curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	if (curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == GAME_blank_space_color)
	//		curr_outline = true;

	//	visited.insert(pixel_to_index(curr_x, curr_y, w));
	//	
	//	if (!curr_outline) 
	//	{
	//		curr = previous; // backtrack! we made an oopsie!!

	//		if (!open.empty())
	//			open.pop();
	//		open.push(pixel_to_index(curr.first, curr.second, w));
	//		continue;
	//	}

	//	outline.push_back({ curr_x, curr_y });

	//	if (curr_x - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == GAME_asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x - 1, curr_y, w)) == visited.end())
	//	{
	//		open.push(pixel_to_index(curr_x - 1, curr_y, w));
	//		continue;
	//	}


	//	if (curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == GAME_asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x, curr_y - 1, w)) == visited.end())
	//	{
	//		open.push(pixel_to_index(curr_x, curr_y - 1, w));
	//		continue;
	//	}

	//	if (curr_x + 1 < w &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == GAME_asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x + 1, curr_y, w)) == visited.end())
	//	{ 
	//		open.push(pixel_to_index(curr_x + 1, curr_y, w));
	//		continue;
	//	}

	//	if (curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == GAME_asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x, curr_y + 1, w)) == visited.end())
	//	{
	//		open.push(pixel_to_index(curr_x, curr_y + 1, w));
	//		continue;
	//	}
	//}

#pragma endregion

	int sum_x = 0;
	int sum_y = 0;
	int count = 0;

	//2
	for (int curr_x = 0; curr_x < w; curr_x++)
	{
		for (int curr_y = 0; curr_y < h; curr_y++)
		{
			if (*(buffer + pixel_to_index(curr_x, curr_y, w)) != GAME_asteroid_color_raw)
				continue;

			sum_x += curr_x;
			sum_y += curr_y;
			count++;

			//outline.push_back({curr_x + 1, curr_y - 1});

			if (curr_x + 1 < w &&
				curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y - 1, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x + 1, curr_y - 1 });

			if (curr_x + 1 < w &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x + 1, curr_y });

			if (curr_x + 1 < w &&
				curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y + 1, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x + 1, curr_y + 1 });

			if (curr_x - 1 > 0 &&
				curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y + 1, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x - 1, curr_y + 1 });

			if (curr_x - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x - 1, curr_y });

			if (curr_x - 1 > 0 &&
				curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y - 1, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x - 1, curr_y });

			if (curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x, curr_y + 1 });

			if (curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == GAME_blank_space_color)
				outline.push_back({ curr_x, curr_y - 1 });

		}
	}

	// TODO: wrap outline clockwise first (for left normals; counterclockwise for right normals)
	// start floodfill at leftmost with upward bias???

	this->center_x = sum_x / count;
	this->center_y = sum_y / count;

	if (outline.size() < 4 * SETTING_MAX_POLYGON_VERTICES)
	{
		for (int i = 0; i < outline.size(); i++)
			this->outline[i] = outline.at(i);
		outline_point_count = outline.size();
	}
	else
		SDL_Log("ERROR: outline buffer overflow for asteroid.\n");

#else // INNER OUTLINES

	std::vector<SDL_Point> outline;

	int sum_x = 0;
	int sum_y = 0;
	int count = 0;

	for (int curr_x = 0; curr_x < w; curr_x++)
	{
		for (int curr_y = 0; curr_y < h; curr_y++)
		{
			if (*(buffer + pixel_to_index(curr_x, curr_y, w)) != GAME_asteroid_color_raw)
				continue;

			sum_x += curr_x;
			sum_y += curr_y;
			count++;

			//outline.push_back({curr_x + 1, curr_y - 1});

			if (curr_x + 1 < w &&
				curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y - 1, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_x + 1 < w &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_x + 1 < w &&
				curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x + 1, curr_y + 1, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_x - 1 > 0 &&
				curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y + 1, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_x - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_x - 1 > 0 &&
				curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x - 1, curr_y - 1, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_y + 1 < h &&
				*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

			if (curr_y - 1 > 0 &&
				*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == GAME_blank_space_color)
			{
				outline.push_back({ curr_x, curr_y });
				continue;
			}

		}
	}

	this->center_x = sum_x / count;
	this->center_y = sum_y / count;

	if (outline.size() < 4 * SETTING_MAX_POINT_COUNT)
	{
		int i = 0;
		for (SDL_Point point : outline)
		{
			this->outline[i] = point;
			i++;
		}


		outline_point_count = outline.size();
	}
	else
		SDL_Log("ERROR: outline buffer overflow for asteroid.\n");


#endif
}

void Asteroid::cleanup()
{
	SDL_DestroyTexture(texture);
}

//void Asteroid::render(RenderWindow* window)
//{
//	//char str[8];
//	//sprintf_s(str, "%i", hash_entity(this));
//	//SDL_Color color = 
//	//{
//	//	255, 255, 255, 255 
//	//};
//	//window->render_centered_world(x, y - (float)h/2 - 10.0F, str, encode_sans_medium, color);
//
//	/*window->render(0, 0, 0, 0, screen_x - (w >> 1), screen_y - (h >> 1), w, h, texture);
//}

void asteroids_init()
{
	int i = 0;
	for (Asteroid* asteroid = (Asteroid*)entities + GAME_ship_count; asteroid < (Asteroid*)entities + GAME_ship_count + GAME_asteroid_count; asteroid++)
	{
		asteroid->x = GAME_width / 2.0F + (float)(rand() % ASTEROID_startpos_variance);
		asteroid->y = GAME_height / 2.0F + (float)(rand() % ASTEROID_startpos_variance);

		asteroid->velocity_x = (float)((float)rand() / (float)RAND_MAX) * 2 * ASTEROID_startspeed_maximum - ASTEROID_startspeed_maximum * 0.5F;
		asteroid->velocity_y = (float)((float)rand() / (float)RAND_MAX) * 2 * ASTEROID_startspeed_maximum - ASTEROID_startspeed_maximum * 0.5F;
		asteroid->velocity_x /= asteroid->mass;
		asteroid->velocity_y /= asteroid->mass;
		asteroid->desired_velocity_x = asteroid->velocity_x;
		asteroid->desired_velocity_y = asteroid->velocity_y;
		asteroid->drag_enabled = true;

		//asteroid->id = i;

		asteroid->init(SETTING_ENTITY_DIMENSION, SETTING_ENTITY_DIMENSION);

		std::cout << "asteroid initialized w/ main index " << asteroid->id << std::endl;
		i++;
	}
}

static bool clicking = true;

void player_input_update(SDL_Event* running_event)
{
	//if (running_event->type == SDL_EventType::SDL_KEYUP && running_event->key.keysym.sym == SDL_KeyCode::SDLK_x)
	//{
	//	int mouse_x, mouse_y;
	//	SDL_GetMouseState(&mouse_x, &mouse_y);
	//	float mouse_world_x = window.camera.screen_to_world_x(mouse_x);
	//	float mouse_world_y = window.camera.screen_to_world_y(mouse_y);

	//	int chunk = ((int)mouse_world_x + GAME_width * ((int)mouse_world_y / chunk_size)) / chunk_size;
	//	//printf("(%i, %i) -> (%f, %f) c: %i\n", mouse_x, mouse_y, mouse_world_x, mouse_world_y, chunk);
	//	if (collision_check_grid[chunk])
	//	{
	//		std::set<int> set = *(collision_check_grid[chunk]);
	//		//printf("%zu\n", set.size());
	//		for (int i : set)
	//			if (Entity::active[i]->in_bounds(mouse_world_x, mouse_world_y))
	//				((Asteroid*)Entity::active[i])->split(mouse_world_x, mouse_world_y, 10.0F);
	//	}
	//}


	// Direct controlled asteroid
	//for (Asteroid* asteroid = (Asteroid*)entities + GAME_ship_count; asteroid < (Asteroid*)entities + GAME_ship_count + GAME_asteroid_count; asteroid++)
	//{
		/*if (asteroid->asteroid_id != controlled_asteroid)
			continue;*/

			// ESC to deselect 
	if (running_event->type == SDL_EventType::SDL_KEYUP && running_event->key.keysym.sym == SDL_KeyCode::SDLK_ESCAPE)
	{
		controlled_asteroid = -1;
	}

	// CLICK to set heading
	if (running_event->type == SDL_MOUSEBUTTONDOWN && running_event->button.button == SETTING_primary_mouse_button)
		clicking = true;
	else if (running_event->type == SDL_MOUSEBUTTONUP && running_event->button.button == SETTING_primary_mouse_button)
		clicking = false;

	// mouse based rotation always
	//if (running_event->type == SDL_MOUSEMOTION) 
	//	player->angle = atan2((double)y_diff_to_mouse, (double)x_diff_to_mouse);


//}
}

inline bool operator==(const SDL_Color& self, const SDL_Color& b)
{
	return self.r == b.r && self.g == b.g && self.b == b.b && self.a == b.a;
}

int* thrust_columns = NULL;

int thrust_outline_thickness = 2;
float thrust_elapsed = 0.0F;
const int thrust_max_height = 6;
const int thrust_min_height = 2;
float thrust_random_tick = 6.0F;
const int thrust_numColumns = 10;
std::random_device rand_device;
std::uniform_int_distribution<int> dist(thrust_min_height, thrust_max_height);

void asteroids_render_update(RenderWindow* window)
{

	if (thrust_columns == NULL)
		thrust_columns = new int[thrust_numColumns];

	if (clicking)
	{
		thrust_elapsed += delta_time;
		if (thrust_elapsed > thrust_random_tick)
		{
			for (int i = 0; i < thrust_numColumns; i++)
			{
				thrust_columns[i] = dist(rand_device);
			}

			thrust_elapsed = 0.0F;
		}


		// Select player asteroid
		Asteroid* player = ((Asteroid*)entities + GAME_ship_count + 1);

		float vel_magnitude = sqrt((player->desired_velocity_x * player->desired_velocity_x) + (player->desired_velocity_y * player->desired_velocity_y));
		float vel_normalized_x = player->desired_velocity_x / vel_magnitude;
		float vel_normalized_y = player->desired_velocity_y / vel_magnitude;

		for (int i = 0; i < player->outline_point_count; i++)
		{
			SDL_Point point = player->outline[i];
			int size = thrust_outline_thickness * thrust_columns[(int)((float)thrust_numColumns * ((float)i / (float)player->outline_point_count))];
			int size_x = size * vel_normalized_x;
			int size_y = size * vel_normalized_y;
			window->render_rect(
				player->x + (float)(point.x - (player->w >> 1) - (size_x)),
				player->y + (float)(point.y - (player->h >> 1) - (size_y)),
				(float)size_x, (float)size_y, PLAYER_thrusting_outline_color);
		}
	}

	for (Asteroid* asteroid = (Asteroid*)entities + GAME_ship_count; asteroid < (Asteroid*)entities + GAME_ship_count + GAME_asteroid_count; asteroid++)
	{
		/*char text[8];
		sprintf_s(text, "%.1d", asteroid->outline_point_count);
		window->render_centered_world(asteroid->x - asteroid->w, asteroid->y - asteroid->h, text, encode_sans_medium, {255, 255, 255, 255});

		int ax = asteroid->screen_x - asteroid->w/2;
		int ay = asteroid->screen_y - asteroid->h/2;
		for (SDL_Point* i = asteroid->outline; i < asteroid->outline + asteroid->outline_point_count; i++)
		{
			int wx = i->x + ax;
			int wy = i->y + ay;

			window->render_point(wx, wy, { 0, 0, 255, 255 });
		}*/


		asteroid->render(window);
	}
}

void asteroids_update(float delta_time)
{

	static int heading_x;
	static int heading_y;

	if (clicking)
	{
		// Select player asteroid
		Asteroid* player = ((Asteroid*)entities + GAME_ship_count + 1);

		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);

		heading_x = window.camera.screen_to_world_x(mouse_x);
		heading_y = window.camera.screen_to_world_y(mouse_y);

		/*int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);*/

		float x_diff = (heading_x - player->x);
		float y_diff = (heading_y - player->y);

		float magnitude = sqrtf(x_diff * x_diff + y_diff * y_diff);
		x_diff /= magnitude;
		y_diff /= magnitude;

		x_diff *= PLAYER_controlspeed_maximum;
		y_diff *= PLAYER_controlspeed_maximum;



		player->desired_velocity_x += (x_diff - player->desired_velocity_x) * delta_time * player->movement_windup_speed;
		player->desired_velocity_y += (y_diff - player->desired_velocity_y) * delta_time * player->movement_windup_speed;
	}

	// TODO: no player control X seconds after collision
	for (Asteroid* asteroid = (Asteroid*)entities + GAME_ship_count; asteroid < (Asteroid*)entities + GAME_ship_count + GAME_asteroid_count; asteroid++)
	{
		asteroid->update();
	}
}

void asteroids_cleanup()
{
	for (Asteroid* asteroid = (Asteroid*)entities + GAME_ship_count; asteroid < (Asteroid*)entities + GAME_ship_count + GAME_asteroid_count; asteroid++)
	{
		asteroid->cleanup();
	}
	delete[] thrust_columns;
}

void Asteroid::on_collision(Entity* other, int collision_x, int collision_y)
{}
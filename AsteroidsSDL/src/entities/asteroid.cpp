#define INNER_ASTEROID_OUTLINES

#include "../main.h"
#include "asteroid.h"
#include "ship.h"
#include "env/thrust_renderer.h"

#include <queue>
#include <set>
#include <map>

#include <random>
#include <iostream>

float mass_per_pixel = (ASTEROID_maximum_mass - ASTEROID_minimum_mass) / (4.0F * ASTEROID_maximum_radius * ASTEROID_maximum_radius);
const char* asteroid_texture_path = "./circle.png";



#pragma region deprecated

//int /*controlled_asteroid*/ = 0;
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
	std::set<SDL_Point> visited;
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
		int prev_x = w, prev_y = h;
		for (int i = 0; i < curr_reach; i++)
		{
			int offset_x = cosf(theta) * i;
			int offset_y = sinf(theta) * i;

			if (prev_x == offset_x && prev_y == offset_y)
				continue;

			if (!visited.contains({ offset_x, offset_y }))
			{
				*(buffer + pixel_to_index(center_x + offset_x, center_y + offset_y, w)) = GAME_asteroid_color_raw;
				(*pixel_count)++;
				visited.insert({ offset_x, offset_y });
			}

			prev_x = offset_x;
			prev_y = offset_y;
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

	if (id == PLAYER_entity_id)
	{
		PLAYER_initial_outline_point_count = point_count;
		debug_log("player (id=%d) initial point count: %d\n", id, point_count);
	}
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

int Asteroid::fill_interior_from_outline(Asteroid* asteroid, std::set<SDL_Point>& added)
{
	added.clear();

	int original_size = 0;

	// to find start point:
	/** softwareengineering.stackexchange.com/questions/226578/selecting-a-point-inside-a-non-convex-polygon
	Compute the bounding box
	Cast a ray from one corner in the direction of the opposite corner, or from the center of a bounding box edge to the opposite edge center.
	Then, a point exactly between the first and second intersection will be inside the polygon.
	*/
	if (asteroid->outline_point_count > 0)
	{
		// ADD OUTLINE POINTS
		added.insert(asteroid->outline, asteroid->outline + asteroid->outline_point_count);
		original_size = added.size(); // size of outline EXCLUDING duplicates

		// get start pos for flood fill (CoM of asteroid)
		// DANGER: designed for circular asteroids only, IE asteroids with no/few protrusions
		SDL_Point start = { 0, 0 };
		for (int i = 0; i < asteroid->outline_point_count; i++)
			start = start + asteroid->outline[i];
		start = start / asteroid->outline_point_count;
		asteroid->center_x = start.x;
		asteroid->center_y = start.y;

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
				&& current.y + 1 < ASTEROID_maximum_radius * 2) // neighbor is not in points points or open points
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

		debug_log("%zu\n", added.size());
	}

	const int max_size = (asteroid->w - 1) * (asteroid->h - 1);
	if (added.size() >= max_size - asteroid->point_count)  //approxomation for square 'overfill' scenario
		return -1;

	return added.size() - original_size; // return number of interior points (excluding outline)
}

void Asteroid::fill_pixels(Asteroid* asteroid, std::set<SDL_Point>& points)
{
	SDL_DestroyTexture(asteroid->texture);
	SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(temp_surface); // locks the surface from outside read/writing; so we can write to it

	Uint32* buffer = (Uint32*)temp_surface->pixels;

	// WRITE ALL ADDED 
	auto iterator = points.begin();
	while (iterator != points.end())
	{
		*(buffer + pixel_to_index(iterator->x, iterator->y, w)) = GAME_asteroid_color_raw;

		iterator++;
	}

	asteroid->point_count = points.size();
	SDL_UnlockSurface(temp_surface); // allow external read/writes when finished writing

	asteroid->texture = window.create_texture_from_surface(temp_surface);

	SDL_FreeSurface(temp_surface);
}

bool outline_contains(SDL_Point* outline, int outline_point_count, SDL_Point point)
{
	for (SDL_Point* current = outline; current < outline + outline_point_count; current++)
		if (current->x == point.x && current->y == point.y)
			return true;
	return false;
}


Asteroid* Asteroid::split(float collision_x, float collision_y, float collision_rel_velocity, bool specify_endpoint, float specified_theta)
{
	const int max_retries = 100;
	const int min_interior_points = 5;

	SDL_Point start;
	SDL_Point end;

	int retries = 0;
	int moved_points = 0;
	Asteroid* created = split_init();

	std::set<SDL_Point> floodfill_points_self;
	std::set<SDL_Point> floodfill_points_created;
	std::vector<SDL_Point> outline_bridge;
	int debug_offset = 0;

	do
	{
		debug_log("Outline point count: %d\n", outline_point_count);
		if (DEBUG_master)
			for (int i = 0; i < outline_point_count; i++)
				window.render_pixel_deferred(10.0F, x - (w >> 1) + outline[i].x + w * debug_offset, y - (h >> 1) + outline[i].y, { 255, 255, 255, 0 });

		resolve_endpoint(collision_x, collision_y, &start, &end, specify_endpoint, specified_theta);
		if (end.x == -1 || end.y == -1)
			return created;

		debug_log("Start: (%d, %d); End: (%d, %d)", start.x, start.y, end.x, end.y);
		separate_outlines(start, end, created, &moved_points);
		if (DEBUG_master)
		{
			for (int i = 0; i < outline_point_count; i++)
				window.render_pixel_deferred(10.0F, x - (w >> 1) + outline[i].x + w * debug_offset, y + outline[i].y, { 255, 255, 0, 0 });
			for (int i = 0; i < created->outline_point_count; i++)
				window.render_pixel_deferred(10.0F, x - (w >> 1) + created->outline[i].x + w * debug_offset, y + created->outline[i].y, { 0, 255, 255, 0 });
		}

		split_bridge_outline(this, start, end, &outline_bridge);
		split_bridge_outline(created, start, end, &outline_bridge);
		if (DEBUG_master)
		{
			for (int i = 0; i < outline_point_count; i++)
				window.render_pixel_deferred(10.0F, x - (w >> 1) + outline[i].x + w * debug_offset, y + (h >> 1) + outline[i].y, { 255, 255, 0, 0 });
			for (int i = 0; i < created->outline_point_count; i++)
				window.render_pixel_deferred(10.0F, x - (w >> 1) + created->outline[i].x + w * debug_offset, y + (h >> 1) + created->outline[i].y, { 0, 255, 255, 0 });
		}

		// points are stored in sets; sizes considerably lower b/c duplicates
		int self_points = fill_interior_from_outline(this, floodfill_points_self);
		int created_points = fill_interior_from_outline(created, floodfill_points_created);

		debug_log("Filled interior points: %d/%d for original; %d/%d for created", self_points, outline_point_count, created_points, created->outline_point_count);
		// always includes the -1 failure case in retry condition
		if (self_points < min_interior_points || created_points < min_interior_points)
		{
			retries++;
			undo_separate_outlines(created, moved_points + outline_bridge.size() /* need outline point count of entire shell of other asteroid */, outline_bridge.size());
			outline_bridge.clear();
			if (DEBUG_master)
				debug_offset++;
			continue;
		}

		fill_pixels(this, floodfill_points_self);
		fill_pixels(created, floodfill_points_created);
		break;
	}
	while (retries < max_retries);


	debug_log("created asteroid: %i; added %zu points, retrying %i times due to inadequate asteroid shapes and sizes or overflows", created->outline_point_count, outline_bridge.size(), retries);


	debug_log("split asteroid idx: %i; has %i points; origin index %i", created->id, created->outline_point_count, id);
	return created;
}

void get_line(float ax, float ay, float bx, float by, float* m, int* b)
{
	*m = (float)(by - ay) / (float)(bx - ax);
	*b = (int)((float)ay - (float)ax * (*m));
}

bool Asteroid::check_valid_split_line(int start_idx, int end_idx)
{
	float ax = outline[start_idx].x, ay = outline[start_idx].y;
	float bx = outline[end_idx].x, by = outline[end_idx].y;
	if ((start_idx == end_idx || outline[end_idx].x == outline[start_idx].x)
		|| (bx - ax) * (bx - ax) + (by - ay) * (by - ay) < (int)((float)outline_point_count / (3.0F * PI)) * (int)((float)outline_point_count / (3.0F * PI)))
		return false;

	float m;
	int b;
	get_line(ax, ay, bx, by, &m, &b);

	// raycast; check for interceding points (skip if there are; non(perfectly)convex shape selected)
	int count = 0;
	int last_x = -1, last_y = -1;
	int x_prev = w, y_prev = h;
	int x_initial = (int)(-b / m), y_initial = 0.0F;
	int x = x_initial, y = y_initial;
	float theta = atan2f(ay, ax - x_initial);
	bool inside_outline = false; // keeps track of current transit state

	for (float dist = 0.0F; dist < 1.41F * (float)w; dist += 0.5F)
	{
		x = (int)(cosf(theta) * dist) + x_initial;
		y = (int)(sinf(theta) * dist) + y_initial;

		if (x_prev == x && y_prev == y || x <= 0 || y <= 0 || x > w || y > h)
			continue;

		if (outline_contains(outline, outline_point_count, { x, y }))
		{
			if (!inside_outline)
			{
				count++;
				inside_outline = true;
			}
			last_x = x;
			last_y = y;
		}
		else if (inside_outline)
			inside_outline = false;

		x_prev = x;
		y_prev = y;
	}

	printf("hit count: %d\n", count);

	return count == 2;
}

void Asteroid::resolve_endpoint(float collision_x, float collision_y, SDL_Point* start, SDL_Point* end, bool specify_theta, float specified_theta)
{
	int collision_pixel_x = (int)collision_x - (screen_x - (w >> 1));
	int collision_pixel_y = (int)collision_y - (screen_y - (h >> 1));

	int contact_idx = -1;

	for (int i = 0; i < outline_point_count; i++)
		if (outline[i].x == collision_pixel_x && outline[i].y == collision_pixel_y)
			contact_idx = i;

	if (contact_idx == -1)
	{
		if (DEBUG_master)
		{
			DEBUG_mode = true;
			DEBUG_wireframe_mode = true;
			time_scaling = 0.0F;
		}
		SDL_LogError(0, "Catastrophic: hit point (%d, %d) outside outline of receiving entity (ID=%d)", collision_pixel_x, collision_pixel_y, id);
		*start = { -1, -1 };
		*end = { -1, -1 };
		return;
	}

	int split_endpoint = 0;

	if (!specify_theta)
	{
		std::default_random_engine e(2);

		std::vector<int> points;
		for (int i = 0; i < outline_point_count; i++)
			points.push_back(i);
		std::shuffle(points.begin(), points.end(), e);

		while (points.size() > 1)
		{
			split_endpoint = points.back();
			points.pop_back();

			if (((split_endpoint == contact_idx || outline[split_endpoint].x == outline[contact_idx].x)
				||
				(outline[split_endpoint].x - collision_pixel_x) * (outline[split_endpoint].x - collision_pixel_x) + (outline[split_endpoint].y - collision_pixel_y) * (outline[split_endpoint].y - collision_pixel_y)
				< (int)((float)outline_point_count / (3.0F * PI)) * (int)((float)outline_point_count / (3.0F * PI))))
				continue;

			if (!check_valid_split_line(contact_idx, split_endpoint))
				continue;

			break;
		}
	}
	else
	{
		// find point first matching theta in spray arc range
		float spray_arc = 0.05F;
		while (split_endpoint < outline_point_count
			&& ((split_endpoint == contact_idx || outline[split_endpoint].x == outline[contact_idx].x)
				|| SDL_abs(atan2f(outline[split_endpoint].y - collision_pixel_y, outline[split_endpoint].x - collision_pixel_x) - specified_theta) > spray_arc))
			split_endpoint++;

		if (split_endpoint == outline_point_count)
			while (split_endpoint == outline_point_count || !(split_endpoint != contact_idx && outline[split_endpoint].x != outline[contact_idx].x))
				split_endpoint = rand() % outline_point_count;
	}

	int ax = collision_pixel_x;
	int ay = collision_pixel_y;
	int bx = outline[split_endpoint].x;
	int by = outline[split_endpoint].y;

	*start = { ax, ay };
	*end = { bx, by };
}

void Asteroid::undo_separate_outlines(Asteroid* other, int separated_point_count, int outline_bridge_count)
{
	// exclude bridges (back to original outline if taken collectively)
	separated_point_count -= outline_bridge_count;
	outline_point_count -= outline_bridge_count;

	// and unsort points from two shapes back into single original
	for (int i = 0; i < SDL_max(outline_point_count, separated_point_count); i++)
	{
		if (outline_point_count < separated_point_count)
		{
			if (i < outline_point_count)
				outline[i + separated_point_count] = outline[i];
			if (i < separated_point_count)
				outline[i] = other->outline[i];
		}
		else if (i < separated_point_count)
			outline[i + outline_point_count] = other->outline[i];
	}


	outline_point_count = outline_point_count + separated_point_count;
	other->outline_point_count = 0;
}

/// <summary>
/// Splits outlines based on the line between start and end; separated_point_count points are moved (selected noncontinuously from original outline) from original asteroid outline to other asteroid outline; outline_point_count of original is truncated to reflect removal of these
/// </summary>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="other"></param>
/// <param name="separated_point_count"></param>
void Asteroid::separate_outlines(const SDL_Point& start, const SDL_Point& end, Asteroid* other, int* separated_point_count)
{
	int ax = start.x;
	int ay = start.y;
	int bx = end.x;
	int by = end.y;


	// use y > mx + b_main to sort points to either resultant asteroid
	//float m_main = (float)(by - ay)/* / (float)(bx - ax);
	//int b_main = (int)((float)ay - */(float)ax * m_main);
	int divider = 0;

	float epsilon = 0.1F;

	float m_main, m_outer_low, m_outer_high;
	int b_main, b_outer_low, b_outer_high;

	float cx = (ax + bx) * 0.5F;
	float cy = (ay + by) * 0.5F;

	get_line(ax, ay, bx, by, &m_main, &b_main);

	get_line(ax, ay, ax - epsilon * (ay - cy), ay + epsilon * (ax - cx), &m_outer_low, &b_outer_low);
	get_line(bx, by, bx - epsilon * (by - cy), by + epsilon * (bx - cx), &m_outer_high, &b_outer_high);

	auto compute_region = [&](SDL_Point point)->bool
		{
			float x = point.x;
			float y = point.y;

			bool halfspace_1 = y > m_main * x + b_main;
			bool halfspace_2, halfspace_3;
			if (b_outer_low < b_outer_high)
			{
				halfspace_2 = y > m_outer_low * x + (float)b_outer_low;
				halfspace_3 = y < m_outer_low * x + (float)b_outer_high;
			}
			else
			{
				halfspace_2 = y < m_outer_low * x + (float)b_outer_low;
				halfspace_3 = y > m_outer_low * x + (float)b_outer_high;
			}

			return halfspace_1;
		};

	auto comparator = [compute_region, ax, ay, bx, by, m_main, b_main](SDL_Point self, SDL_Point other)->bool
		{
			bool self_in = compute_region(self);
			bool other_in = compute_region(other);

			return self_in > other_in;
		};

	std::priority_queue < SDL_Point, std::vector<SDL_Point>, decltype(comparator)> queue(comparator);

	for (int i = 0; i < outline_point_count; i++)
	{
		bool in = (outline[i].y > (int)(m_main * (float)outline[i].x + (float)b_main));
		//debug_log("p (%i, %i): %i", outline[i].x, outline[i].y, in ? 1 : 0);
		if (!in)
			divider++;
		queue.push(outline[i]);
	}


	other->outline_point_count = divider;
	*separated_point_count = divider;
	for (int i = 0; i < divider; i++)
	{
		other->outline[i] = (SDL_Point)queue.top();
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
}

Asteroid* Asteroid::split_init()
{
	Asteroid* created = append_asteroid_to_pool();

	created->outline_point_count = 0;

	created->x = x + 1;
	created->y = y + 1;

	created->velocity_x = velocity_x;
	created->velocity_y = velocity_y;
	created->desired_velocity_x = desired_velocity_x;
	created->desired_velocity_y = desired_velocity_y;

	return created;
}

void Asteroid::split_bridge_outline(Asteroid* asteroid, const SDL_Point& start, const SDL_Point& end, std::vector<SDL_Point>* outline_additions)
{
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
				debug_log("reached end on step %d of %d for asteroid w/ id %d; (%d, %d) to (%d, %d)", i, steps, asteroid->id, curr_x, curr_y, next_x, next_y);

			// raster traverse from curr to next
			int distance = (next_x - curr_x) * (next_x - curr_x) + (next_y - curr_y) * (next_y - curr_y);
			distance = SDL_sqrtf((float)distance);
			float theta = atan2f((next_y - curr_y), (next_x - curr_x));

			int x_prev = asteroid->w; // unreachable start point chosen (so none culled for first iteration)
			int y_prev = asteroid->h;
			int x, y;

			float resolution = 0.5F;

			for (float j = 0; j <= distance + 2; j += resolution)
			{
				x = (int)(cosf(theta) * j) + curr_x;
				y = (int)(sinf(theta) * j) + curr_y;

				if (x_prev == x && y_prev == y)
					continue;

				outline_additions->push_back({ x, y });

				if (x_prev == x || y_prev == y) // if it's a perfect diagonal
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
		debug_log("Outline buffer overflow for asteroid id %i", id);
		return;
	}

	for (int i = asteroid->outline_point_count; i < asteroid->outline_point_count + outline_additions->size(); i++)
		asteroid->outline[i] = (*outline_additions)[i - asteroid->outline_point_count];


	asteroid->outline_point_count += outline_additions->size();
}

void Asteroid::remove_bridge_outline(Asteroid* asteroid, const SDL_Point& start, const SDL_Point& end, std::vector<SDL_Point>* outline_additions)
{
	asteroid->outline_point_count -= outline_additions->size();
}

Asteroid* append_asteroid_to_pool()
{
	if (GAME_asteroid_count == GAME_asteroid_pool_size)
		return nullptr;

	return new ((Asteroid*)entities + GAME_ship_count + GAME_asteroid_count++) Asteroid();
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

	//	outline.push_back({curr_x, curr_y });

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
		debug_log("ERROR: outline buffer overflow for asteroid.\n");

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
		debug_log("ERROR: outline buffer overflow for asteroid.\n");


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

		//asteroid->id = i;

		asteroid->init(SETTING_ENTITY_DIMENSION, SETTING_ENTITY_DIMENSION);

		std::cout << "asteroid initialized w/ main index " << asteroid->id << std::endl;
		i++;
	}
}

static bool clicking = false;

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
	//		std::points<int> points = *(collision_check_grid[chunk]);
	//		//printf("%zu\n", points.size());
	//		for (int i : points)
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
	/*if (running_event->type == SDL_EventType::SDL_KEYUP && running_event->key.keysym.sym == SDL_KeyCode::SDLK_ESCAPE)
	{
		controlled_asteroid = -1;
	}*/

	// CLICK to points heading
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

/*int* thrust_columns = NULL;

int thrust_outline_thickness = 2;
float thrust_elapsed = 0.0F;
const int thrust_max_height = 6;
const int thrust_min_height = 2;
float thrust_random_tick = 6.0F;
const int thrust_numColumns = 10;
std::random_device rand_device;
std::uniform_int_distribution<int> dist(thrust_min_height, thrust_max_height);*/

//void render_thrust(RenderWindow* window, Entity* entity, const SDL_Color& color)
//{
//	if (thrust_columns == NULL)
//		thrust_columns = new int[thrust_numColumns];
//
//	thrust_elapsed += delta_time;
//	if (thrust_elapsed > thrust_random_tick)
//	{
//		for (int i = 0; i < thrust_numColumns; i++)
//		{
//			thrust_columns[i] = dist(rand_device);
//		}
//
//		thrust_elapsed = 0.0F;
//	}
//
//	float vel_magnitude = sqrt((entity->desired_velocity_x * entity->desired_velocity_x) + (entity->desired_velocity_y * entity->desired_velocity_y));
//	float vel_normalized_x = entity->desired_velocity_x / vel_magnitude;
//	float vel_normalized_y = entity->desired_velocity_y / vel_magnitude;
//
//	for (int i = 0; i < entity->outline_point_count; i++)
//	{
//		SDL_Point point = entity->outline[i];
//		int size = thrust_outline_thickness * thrust_columns[(int)((float)thrust_numColumns * ((float)i / (float)entity->outline_point_count))];
//		int size_x = size * vel_normalized_x;
//		int size_y = size * vel_normalized_y;
//		window->render_rect(
//			entity->x + (float)(point.x - (entity->w >> 1) - (size_x)),
//			entity->y + (float)(point.y - (entity->h >> 1) - (size_y)),
//			(float)size_x, (float)size_y, color);
//	}
//}

void asteroids_render_update(RenderWindow* window)
{
	static float elapsed = 0.0F;
	const float clip_length = 1.0F;
	if (clicking)
	{
		// Select player asteroid
		Asteroid* player = ((Asteroid*)entities + PLAYER_entity_id);

		render_thrust(window, player, PLAYER_thrusting_outline_color);

		/*elapsed += unscaled_delta_time / 1000.0F;
		if (elapsed > clip_length)
		{
			elapsed = 0.0F;
			int success = SDL_QueueAudio(AUDIO_device_id, thrust_wavBuffer, thrust_wavLength);
			SDL_PauseAudioDevice(AUDIO_device_id, 0);
		}*/
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

			window->render_point(wx, wy, {0, 0, 255, 255 });
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
		Asteroid* player = ((Asteroid*)entities + PLAYER_entity_id);

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
	thrust_renderer_cleanup();
}

void Asteroid::on_collision(Entity* other, int collision_x, int collision_y)
{
	if (other->id < GAME_ship_count) // other is ship
	{
		float crit_vel = 0.02F;
		alert_ship_warning(other, this);
		if (id == PLAYER_entity_id && velocity_x * velocity_x + velocity_y * velocity_y >= crit_vel * crit_vel)
		{
			ship_damage(other, 1);
		}
	}
}
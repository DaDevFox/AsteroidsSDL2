#include "asteroid.h"
#include "../main.h"
#include <math.h>
#include <stdlib.h>
#include <set>
#include <queue>
#include <iostream>

float minimum_mass = 1.0F;
float maximum_mass = 5.0F;
float mass_per_pixel = (maximum_mass - minimum_mass) / (4.0F * max_asteroid_radius * max_asteroid_radius);
int max_asteroid_radius = 128/2;

int asteroid_variance = 750;
float top_initial_speed = 0.02F;

const char* asteroid_texture_path = "./circle.png";

Asteroid *asteroids;
const int asteroids_count = 50;

int controlled_asteroid;

float speed = 1.0F;

int downup = 0;
int leftright = 0;

bool primary_held;

int mouse_diff_threshold_squared = 125;

const int max_asteroid_target_variance = 100;

const Uint32 asteroid_color_raw = 0xFFFFFFFF;
const Uint32 blank_space_color = 0x00000000;

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



Asteroid::Asteroid() 
{
	init(max_asteroid_radius * 2, max_asteroid_radius * 2);
}

void Asteroid::init(int w, int h) {
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

void Asteroid::rand_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count) 
{
	std::queue<int> open;
	std::set<int> visited;

	open.push(pixel_to_index(w / 2, h / 2, w));

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

		*(buffer + open.front()) = asteroid_color_raw;
		*pixel_count++;
		open.pop();

		if (curr.first < *leftmost_x) {
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

#define PI 3.14159265F

void Asteroid::circle_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count) 
{
	*pixel_count = 0;
	int reach_max_variance = 10;

	int octaves = 1;
	int min_reach_initial = 5;
	int max_reach_initial = 25 + (rand() % (2 * reach_max_variance + 1) - reach_max_variance);
	int variance = 1;

	float theta;
	// lines per circle
	float resolution = 360.0F;
	float step = 2.0F * PI / resolution;

	float bridge_resolution = (2.0F * PI) / 90.0F;
	float bridge_current = 0.0F;

	int center_x = w/2;
	int center_y = h/2;

	
	int reach = rand() % (max_reach_initial - min_reach_initial) + min_reach_initial;
	int reach_initial = reach;

	for (theta = 0; theta < 2.0F * octaves * PI; theta += step) 
	{
		if(bridge_current > bridge_resolution)
			reach += rand() % (2 * variance + 1) - variance;

		reach = SDL_max(SDL_min(reach, max_reach_initial), 0);

		int curr_reach = reach + (reach_initial - reach) * (theta / (2.0F * PI));
		for (int i = 0; i < curr_reach; i++)
		{
			int offset_x = cosf(theta) * i;
			int offset_y = sinf(theta) * i;

			*(buffer + pixel_to_index(center_x + offset_x, center_y + offset_y, w)) = asteroid_color_raw;
			pixel_count++;
		}

		bridge_current += theta;
	}
}

void Asteroid::generate()
{
	SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(temp_surface);

	Uint32* buffer = (Uint32*)temp_surface->pixels;

	int pixels;
	int leftmost_x;
	int leftmost_y;
	
	circle_expand_fill(buffer, &leftmost_x, &leftmost_y, &pixels);
	
	point_count = pixels;
	mass = minimum_mass + mass_per_pixel * pixels;

	create_outline(buffer);

	SDL_UnlockSurface(temp_surface);

	texture = window.create_texture_from_surface(temp_surface);

	SDL_FreeSurface(temp_surface);
}

void Asteroid::create_outline(Uint32* buffer) {
	std::vector<SDL_Point> outline;

	// wrap clockwise
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

	//	if (*(buffer + pixel_to_index(curr_x, curr_y, w)) != asteroid_color_raw)
	//		continue;

	//	if (curr_x + 1 < w &&
	//		curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y - 1, w)) == blank_space_color)
	//		curr_outline = true;

	//	if (curr_x + 1 < w &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == blank_space_color)
	//		curr_outline = true;

	//	if (curr_x + 1 < w &&
	//		curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y + 1, w)) == blank_space_color)
	//		curr_outline = true;

	//	if (curr_x - 1 > 0 &&
	//		curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y + 1, w)) == blank_space_color)
	//		curr_outline = true;

	//	if (curr_x - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == blank_space_color)
	//		curr_outline = true;

	//	if (curr_x - 1 > 0 &&
	//		curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y - 1, w)) == blank_space_color)
	//		curr_outline = true;;

	//	if (curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == blank_space_color)
	//		curr_outline = true;

	//	if (curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == blank_space_color)
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
	//		*(buffer + pixel_to_index(curr_x - 1, curr_y, w)) == asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x - 1, curr_y, w)) == visited.end())
	//	{
	//		open.push(pixel_to_index(curr_x - 1, curr_y, w));
	//		continue;
	//	}


	//	if (curr_y - 1 > 0 &&
	//		*(buffer + pixel_to_index(curr_x, curr_y - 1, w)) == asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x, curr_y - 1, w)) == visited.end())
	//	{
	//		open.push(pixel_to_index(curr_x, curr_y - 1, w));
	//		continue;
	//	}

	//	if (curr_x + 1 < w &&
	//		*(buffer + pixel_to_index(curr_x + 1, curr_y, w)) == asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x + 1, curr_y, w)) == visited.end())
	//	{ 
	//		open.push(pixel_to_index(curr_x + 1, curr_y, w));
	//		continue;
	//	}

	//	if (curr_y + 1 < h &&
	//		*(buffer + pixel_to_index(curr_x, curr_y + 1, w)) == asteroid_color_raw
	//		&& visited.find(pixel_to_index(curr_x, curr_y + 1, w)) == visited.end())
	//	{
	//		open.push(pixel_to_index(curr_x, curr_y + 1, w));
	//		continue;
	//	}
	//}

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

	// TODO: wrap outline clockwise first (for left normals; counterclockwise for right normals)
	// start floodfill at leftmost with upward bias???


	if (outline.size() < 4 * SETTING_MAX_POLYGON_VERTICES)
	{
		for (int i = 0; i < outline.size(); i++)
			this->outline[i] = outline.at(i);
		outline_point_count = outline.size();
	}
	else
		SDL_Log("ERROR: outline buffer overflow for asteroid.\n");
}

void Asteroid::cleanup() {
	SDL_DestroyTexture(texture);
}

bool Asteroid::in_bounds(int screen_x, int screen_y) const {
	return
		screen_x >= (this->screen_x - w / 2) && screen_x <= (this->screen_x + w / 2) &&
		screen_y >= (this->screen_y - h / 2) && screen_y <= (this->screen_y + h / 2);
}

//void Asteroid::render(RenderWindow* window)
//{
//	//char str[8];/*
//	//sprintf_s(str, "%i", hash_entity(this));
//	//SDL_Color color = 
//	//{
//	//	255, 255, 255, 255 
//	//};
//	//window->render_centered_world(x, y - (float)h/2 - 10.0F, str, encode_sans_medium, color);*/
//
//	/*window->render(0, 0, 0, 0, screen_x - w / 2, screen_y - h / 2, w, h, texture);*/
//}

void asteroids_init() 
{
	asteroids = new Asteroid[asteroids_count];

	int i = 0;
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->x = GAME_width / 2.0F + (float)(rand() % asteroid_variance);
		asteroid->y = GAME_height / 2.0F + (float)(rand() % asteroid_variance);

		asteroid->velocity_x = (float)((float)rand() / (float)RAND_MAX) * 2 * top_initial_speed - top_initial_speed * 0.5F;
		asteroid->velocity_y = (float)((float)rand() / (float)RAND_MAX) * 2 * top_initial_speed - top_initial_speed * 0.5F;
		asteroid->velocity_x /= asteroid->mass;
		asteroid->velocity_y /= asteroid->mass;
		asteroid->desired_velocity_x = asteroid->velocity_x;
		asteroid->desired_velocity_y = asteroid->velocity_y;
		asteroid->drag_enabled = true;
		
		asteroid->idx = i;

		std::cout << "asteroid initialized w/ main index " << asteroid->id << " and asteroid index " << asteroid->idx << std::endl;
		i++;
	}
}

void player_input_update(SDL_Event *running_event) 
{
	// Select controlled asteroid
	
	// Direct controlled asteroid
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		//if (asteroid->asteroid_id != controlled_asteroid)
		//	continue;

		//// ESC to deselect 
		//if (running_event->type == SDL_EventType::SDL_KEYUP && running_event->key.keysym.sym == SDL_KeyCode::SDLK_ESCAPE) 
		//{
		//	controlled_asteroid = -1;
		//}

		//// CLICK to set heading
		//if (running_event->type == SDL_MOUSEBUTTONUP && running_event->button.button == SETTING_primary_mouse_button)
		//{
		//	asteroid->heading_x = window.camera.screen_to_world_x(running_event->motion.x);
		//	asteroid->heading_y = window.camera.screen_to_world_y(running_event->motion.y);
		//	controlled_asteroid = -1;
		//}

		//// mouse based rotation always
		//if (running_event->type == SDL_MOUSEMOTION) 
		//	asteroid->angle = atan2((double)y_diff_to_mouse, (double)x_diff_to_mouse);
	}
}

inline bool operator==(const SDL_Color& self, const SDL_Color& b) {
	return self.r == b.r && self.g == b.g && self.b == b.b && self.a == b.a;
}

void asteroids_render_update(RenderWindow *window)
{
	for (int x = 0; x < GAME_width; x += tile_size) 
		window->render_line(x, 0, x, GAME_height, {200, 200, 200, 255});

	for (int y = 0; y < GAME_height; y += tile_size)
		window->render_line(0, y, GAME_width, y, { 200, 200, 200, 255 });
	

	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
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
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->update();
	}
}

void asteroids_cleanup() 
{
	for (Asteroid* asteroid = asteroids; asteroid < &asteroids[0] + asteroids_count; asteroid++)
	{
		asteroid->cleanup();
	}
	delete[] asteroids;
}

#include <vector>
#include <random>
#include "thrust_renderer.h"

std::vector<int*> all_thrust_columns;
std::vector<float> thrusts_elapsed;

int thrust_outline_thickness = 2;
//const int thrust_min_height = 2;
//const int thrust_max_height = 6;
float thrust_random_tick = 6.0F;
const int thrust_numColumns = 10;
std::random_device rand_device;

void render_thrust(RenderWindow* window, Entity* entity, const SDL_Color& color, int min_height, int max_height, float random_tick, bool central)
{
	if (entity->outline_point_count == 0)
		return;

	int thrust_min_height = 0;
	int thrust_max_height = 0;
	float thrust_random_tick = 0.0F;
	thrust_min_height = SDL_max(thrust_min_height, min_height);
	thrust_max_height = SDL_max(thrust_max_height, max_height);
	thrust_random_tick = SDL_max(thrust_random_tick, random_tick);

	std::uniform_int_distribution<int> dist(thrust_min_height, thrust_max_height);

	if (all_thrust_columns.size() == 0)
	{
		for (int i = 0; i < GAME_ship_count + GAME_asteroid_pool_size; i++)
		{
			all_thrust_columns.push_back(new int[thrust_numColumns]);
			for (int j = 0; j < thrust_numColumns; j++)
				all_thrust_columns[i][j] = 0;
			thrusts_elapsed.push_back(0.0F);
		}
	}

	int* thrust_columns = all_thrust_columns[entity->id];
	float thrust_elapsed = thrusts_elapsed[entity->id];
	thrust_elapsed += delta_time;
	if (thrust_elapsed > thrust_random_tick)
	{
		for (int i = 0; i < thrust_numColumns; i++)
		{
			thrust_columns[i] = dist(rand_device);
		}

		thrust_elapsed = 0.0F;
	}

	thrusts_elapsed[entity->id] = thrust_elapsed;

	if (central)
	{
		float avg_radius = (float)entity->outline_point_count / (2 * PI); // weird but fast way to get approximate radius
		int center_x = 0, center_y = 0;
		for (int i = 0; i < entity->outline_point_count; i++)
		{
			center_x += entity->outline[i].x;
			center_y += entity->outline[i].y;
		}

		center_x /= entity->outline_point_count;
		center_y /= entity->outline_point_count;

		for (int i = 0; i < entity->outline_point_count; i++)
		{
			float diff_normalized_x = -(entity->outline[i].x - center_x) / avg_radius;
			float diff_normalized_y = -(entity->outline[i].y - center_y) / avg_radius;


			SDL_Point point = entity->outline[i];
			int size = thrust_outline_thickness * thrust_columns[(int)((float)thrust_numColumns * ((float)i / (float)entity->outline_point_count))];
			int size_x = size * diff_normalized_x;
			int size_y = size * diff_normalized_y;

			if (size_x < 0)
				size_x = SDL_min(-1, size_x);
			else
				size_x = SDL_max(1, size_x);
			if (size_y < 0)
				size_y = SDL_min(-1, size_y);
			else
				size_y = SDL_max(1, size_y);

			window->render_rect(
				entity->x + (float)(point.x - (entity->w >> 1) - (size_x)),
				entity->y + (float)(point.y - (entity->h >> 1) - (size_y)),
				(float)size_x, (float)size_y, color);
		}

		return;
	}

	float vel_magnitude = sqrt((entity->desired_velocity_x * entity->desired_velocity_x) + (entity->desired_velocity_y * entity->desired_velocity_y));
	float vel_normalized_x = entity->desired_velocity_x / vel_magnitude;
	float vel_normalized_y = entity->desired_velocity_y / vel_magnitude;

	for (int i = 0; i < entity->outline_point_count; i++)
	{
		SDL_Point point = entity->outline[i];
		int size = thrust_outline_thickness * thrust_columns[(int)((float)thrust_numColumns * ((float)i / (float)entity->outline_point_count))];
		int size_x = size * vel_normalized_x;
		int size_y = size * vel_normalized_y;

		if (size_x < 0)
			size_x = SDL_min(-1, size_x);
		else
			size_x = SDL_max(1, size_x);
		if (size_y < 0)
			size_y = SDL_min(-1, size_y);
		else
			size_y = SDL_max(1, size_y);

		window->render_rect(
			entity->x + (float)(point.x - (entity->w >> 1) - (size_x)),
			entity->y + (float)(point.y - (entity->h >> 1) - (size_y)),
			(float)size_x, (float)size_y, color);
	}
}


void thrust_renderer_cleanup()
{
	for (int* thrust_columns : all_thrust_columns)
		delete[] thrust_columns;
}

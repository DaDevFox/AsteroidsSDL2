#pragma once
#include "../main.h"

/// <summary>
/// WIP
/// </summary>
class Entity 
{
public: 
	float x;
	float y;
	int w;
	int h;
	
	float velocity_x;
	float velocity_y;

	float desired_velocity_x;
	float desired_velocity_y;

	bool drag_enabled;

	int screen_x;
	int screen_y;
	
	float movement_windup_speed;
	float drag;
	float mass;

	SDL_Texture* texture;

	int collision_chunk;

	float max_internal_chord = 300.0F;
	int point_count;
	SDL_Point outline[SETTING_MAX_POLYGON_VERTICES];

	Entity() :
		x(0.0F),
		y(0.0F),
		w(512),
		h(512),

		velocity_x(0.0F),
		velocity_y(0.0F),
		desired_velocity_x(0.0F),
		desired_velocity_y(0.0F),
		screen_x(0),
		screen_y(0),
		collision_chunk(0),

		movement_windup_speed(0.005f),
		drag(0.0001F),
		mass(1.0F)
	{

	}

	void init();
	void update();
	void render(RenderWindow *window);
	void cleanup();

	bool in_bounds(int screen_x, int screen_y);
	bool in_bounds(Entity other);
private:
	void move();
	void update_collision_chunk();
	void check_collisions();

};

class Asteroid : public Entity {
public:
	float target_offset_x;
	float target_offset_y;

	double angle;
	int rel_pivot_x;
	int rel_pivot_y;

	void init(const char* texture_file_path, int w, int h);
	void generate();
	bool in_bounds(int screen_x, int screen_y);
	void render(RenderWindow* window);
	void cleanup();

	Asteroid(const char* texture_file_path, int w, int h);
};
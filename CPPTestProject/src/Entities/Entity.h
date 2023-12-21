#pragma once
#include "../window/RenderWindow.h"

/// <summary>
/// ABSTRACT DO NOT USE; see RectEntity instead
/// </summary>
class Entity {
public: 
	float x;
	float y;
	
	float velocity_x;
	float velocity_y;

	float desired_velocity_x;
	float desired_velocity_y;


	int screen_x;
	int screen_y;
	
	float movement_windup_speed;
	float drag;
	float mass;

	Entity() :
		x(0.0F),
		y(0.0F),
		velocity_x(0.0F),
		velocity_y(0.0F),
		desired_velocity_x(0.0F),
		desired_velocity_y(0.0F),
		screen_x(0),
		screen_y(0),

		movement_windup_speed(0.005f),
		drag(0.01F),
		mass(1.0F)
	{

	}

	void init();
	void update();
	void render(RenderWindow *window);
	void cleanup();

	bool in_bounds(int screen_x, int screen_y);
	bool in_bounds(Entity other);
};

class RectEntity : public Entity {
public:
	int w;
	int h;

	double angle;
	int rel_pivot_x;
	int rel_pivot_y;

	SDL_Texture* texture;

	void init(const char* texture_file_path, int w, int h);
	bool in_bounds(int screen_x, int screen_y);
	void render(RenderWindow* window);
	void cleanup();

	RectEntity(const char* texture_file_path, int w, int h);
};
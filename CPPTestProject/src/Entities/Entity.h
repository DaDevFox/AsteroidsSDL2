#pragma once
#include "../RenderWindow.h"

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

	float linear_wind_speed = 0.0005F;

	int screen_x;
	int screen_y;
	
	float drag = 0.01F;

	float mass = 1.0F;

	void init();
	void update();
	void render(RenderWindow *window);
	void cleanup();

	bool in_bounds(int screen_x, int screen_y);
};

class RectEntity : public Entity {
public:
	int w;
	int h;

	double angle;
	int rel_pivot_x;
	int rel_pivot_y;

	SDL_Texture* texture;

	void init();
	bool in_bounds(int screen_x, int screen_y);
	void render(RenderWindow* window);
	void cleanup();

	RectEntity(const char* texture_file_path, int w, int h);
};
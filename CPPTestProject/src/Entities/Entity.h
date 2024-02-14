#pragma once
#include "../main.h"
#include "EntityManager.h"
#include <vector>

extern const int tile_size;
extern const int chunk_height;
extern const int chunk_width;

/// <summary>
/// WIP
/// </summary>
class Entity 
{
public:
	static std::vector<Entity*> active;

	static int curr_id;
	int id;

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

	int center_x;
	int center_y;
	
	float movement_windup_speed;
	float drag;
	float mass;

	SDL_Texture* texture;

	int collision_chunk;

	int outline_point_count;
	int point_count;
	SDL_Point outline[4 * SETTING_MAX_POLYGON_VERTICES];

public:
	Entity();
	Entity(const Entity& copy);

	void init();
	virtual void update();
	virtual void render(RenderWindow *window);
	void cleanup();

	bool in_bounds(int screen_x, int screen_y);
	bool in_bounds(Entity other);
private:
	void move();
	void update_collision_chunk();
	void check_collisions();

	friend class EntityManager;
};

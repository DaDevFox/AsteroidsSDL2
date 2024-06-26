#pragma once
#include "../main.h"
#include <vector>
#include <map>
#include <set>

extern const int chunk_size;
extern const int GAME_chunkwise_height;
extern const int GAME_chunkwise_width;

extern std::map<int, std::set<int>*> collision_check_grid;
class Entity
{
public:

	static std::vector<Entity*> active;

	static int curr_id;


#pragma region core data

	int id;

	float x;
	float y;
	int w;
	int h;

	float velocity_x;
	float velocity_y;

	float desired_velocity_x;
	float desired_velocity_y;

	int screen_x;
	int screen_y;

	int center_x;
	int center_y;

#pragma endregion

#pragma region physics data



	float movement_windup_speed;
	float rotation_windup_speed;
	float drag;
	float mass;

	double desired_rotation;
	double rotation;

#pragma endregion

#pragma region graphics + systems data
	SDL_Texture* texture;

	int collision_chunk;

	int outline_point_count;
	int point_count;
	SDL_Point outline[4 * SETTING_ENTITY_DIMENSION]; // assuming used correctly; max amount of points in outline is max(perimeter of rect, circumference of inscribed circle in rect)?

#pragma endregion

public:
	Entity();
	Entity(const Entity& copy);

	void init();
	virtual void update();
	virtual void render(RenderWindow* window);
	void cleanup();

	bool in_bounds(float world_x, float world_y) const;
	bool in_bounds(Entity other);

	virtual void on_collision(Entity* other, int collision_x, int collision_y);
private:
	void move();
	void update_collision_chunk();
	void check_collisions();


	friend class EntityManager;
};

void entities_init();
void entities_cleanup();

extern int hash_entity(Entity* entity);

extern int hash(int x, int y);
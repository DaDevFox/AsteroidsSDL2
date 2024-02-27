#pragma once
#include <SDL.h>
#include "Entity.h"

class Asteroid : public Entity
{
public:
	//int idx;

	void init(int w, int h);
	void generate();
	void create_outline(Uint32* buffer);
	bool in_bounds(int screen_x, int screen_y) const;
	//void render(RenderWindow* window);
	void cleanup();

	Asteroid();
	/*~Asteroid();*/
private:
	void rand_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count);
	void circle_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count);

};

extern int ASTEROID_max_asteroid_radius;

extern const int GAME_asteroid_count;
extern Asteroid* asteroids;
extern Asteroid player;



void asteroids_init();
void player_input_update(SDL_Event* running_event);
void asteroids_render_update(RenderWindow* window);
void asteroids_update(float delta_time);
void asteroids_cleanup();


#pragma once
#include <SDL.h>
#include "Entity.h"

class Asteroid : public Entity
{
public:
	void init(int w, int h);
	void generate();
	void create_outline(Uint32* buffer);
	//bool in_bounds(int screen_x, int screen_y) const;
	//void render(RenderWindow* window);
	void cleanup();

	void on_collision(Entity* other, int collision_x, int collision_y) override;

	Asteroid* split(float collision_x, float collision_y, float collision_rel_velocity, bool specify_theta = false, float specified_theta = 0.0F);

	Asteroid();
	/*~Asteroid();*/
private:
	void rand_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count);
	void circle_expand_fill(Uint32* buffer, int* leftmost_x, int* leftmost_y, int* pixel_count);

	bool check_valid_split_line(int start_idx, int end_idx);
	void resolve_endpoint(float collision_x, float collision_y, SDL_Point* start, SDL_Point* end, bool specify_theta, float specified_theta);
	void separate_outlines(const SDL_Point& start, const SDL_Point& end, Asteroid* other, int* separated_point_count);
	void undo_separate_outlines(Asteroid* other, int separated_point_count, int outline_bridge_count);
	Asteroid* split_init();
	void split_bridge_outline(Asteroid* asteroid, const SDL_Point& start, const SDL_Point& end, std::vector<SDL_Point>* outline_additions);
	void remove_bridge_outline(Asteroid* asteroid, const SDL_Point& start, const SDL_Point& end, std::vector<SDL_Point>* outline_additions);
	int fill_interior_from_outline(Asteroid* asteroid, std::set<SDL_Point>& added);
	void fill_pixels(Asteroid* asteroid, std::set<SDL_Point>& points);
};

extern int ASTEROID_max_asteroid_radius;

extern int GAME_asteroid_count;
extern Asteroid* asteroids;
extern Asteroid player;



void asteroids_init();
void player_input_update(SDL_Event* running_event);
void asteroids_render_update(RenderWindow* window);
void asteroids_update(float delta_time);
void asteroids_cleanup();

Asteroid* append_asteroid_to_pool();

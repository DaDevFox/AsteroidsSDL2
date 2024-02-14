#pragma once
#include <SDL.h>

class Camera {
public:
	float x;
	float y;
	float desired_x;
	float desired_y;

	float zoom;

	/// <summary>
	/// Sets zoom and teleports to new zoomed location to avoid jarring movements
	/// </summary>
	/// <param name="new_zoom"></param>
	void set_zoom(float new_zoom);
	/// <summary>
	/// Moves instantaneously to a given x and y coordinate
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void teleport(float x, float y);

	void input_update(SDL_Event* running_event);
	void update(float delta_time);

	bool in_game(int screen_x, int screen_y);
	bool screen_to_world(int screen_x, int screen_y, float* world_x, float* world_y);
	int screen_to_world_x(int screen_x);
	int screen_to_world_y(int screen_y);

	/// <summary>
	/// Returns true if world_x and world_y are on the screen after being converted to screen coordinates
	/// </summary>
	/// <param name="world_x"></param>
	/// <param name="world_y"></param>
	/// <returns></returns>
	bool on_screen(float world_x, float world_y);
	/// <summary>
	/// Returns true if screen_x and screen_y are on the screen and places the screen coordinate equivalents of world_x and world_y in them
	/// </summary>
	/// <param name="world_x"></param>
	/// <param name="world_y"></param>
	/// <param name="screen_x"></param>
	/// <param name="screen_y"></param>
	/// <returns></returns>
	bool world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y);
	int world_to_screen_x(float world_x);
	int world_to_screen_y(float world_y);

	Camera()
		: x(0.0F), y(0.0F), desired_x(0.0F), desired_y(0.0F), zoom(1.0F) {}
};

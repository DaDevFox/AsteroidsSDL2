#include "camera.h"
#include "../main.h"

void Camera::teleport(float x, float y) {
	this->x = x;
	this->y = y;
	desired_x = x;
	desired_y = y;
}

bool Camera::in_game(int screen_x, int screen_y) {
	int world_x = screen_x + x;
	int world_y = screen_y + y;
	return 
		world_x > 0 && world_x < GAME_width &&
		world_y > 0 && world_y < GAME_height;
}

bool Camera::screen_to_world(int screen_x, int screen_y, float* world_x, float* world_y) 
{
	*world_x = screen_x + x;
	*world_y = screen_y + y;
	return
		*world_x > 0 && *world_x < GAME_width &&
		*world_y > 0 && *world_y < GAME_height;
}

int Camera::screen_to_world_x(int screen_x) {
	return (int)(screen_x + x);
}
int Camera::screen_to_world_y(int screen_y) {
	return (int)(screen_y + y);
}

bool Camera::on_screen(float world_x, float world_y) {
	int screen_x = world_x - x;
	int screen_y = world_y - y;
	return
		screen_x > 0 && screen_x < WINDOW_width &&
		screen_y > 0 && screen_y < WINDOW_height;
}

bool Camera::world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y) {
	*screen_x = (int)(world_x - x);
	*screen_y = (int)(world_y - y);

	return
		*screen_x > 0 && *screen_x < WINDOW_width &&
		*screen_y > 0 && *screen_y < WINDOW_height;
}

int Camera::world_to_screen_x(float world_x) {
	return (int)(world_x - x);
}
int Camera::world_to_screen_y(float world_y) {
	return (int)(world_y - y);
}

void Camera::input_update(SDL_Event *running_event) 
{
	if (running_event->type == SDL_KEYDOWN)
	{
		if (running_event->key.keysym.sym == SDLK_RIGHT)
		{
			desired_x += SETTING_camera_pan_speed;
		}
		if (running_event->key.keysym.sym == SDLK_LEFT)
		{
			desired_x -= SETTING_camera_pan_speed;
		}
		if (running_event->key.keysym.sym == SDLK_DOWN)
		{
			desired_y += SETTING_camera_pan_speed;
		}
		if (running_event->key.keysym.sym == SDLK_UP)
		{
			desired_y -= SETTING_camera_pan_speed;
		}
	}
}

void Camera::update(float delta_time) {
	if(fabs(desired_x - x) > 0.1f)
		x = x + (desired_x - x) * delta_time * (1.0F - SETTING_camera_pan_smoothness);
	if(fabs(desired_y - y) > 0.1f)
		y = y + (desired_y - y) * delta_time * (1.0F - SETTING_camera_pan_smoothness);
	
	if (x > GAME_width)
		x = GAME_width;
	if (x < 0)
		x = 0;
	if (y > GAME_height)
		y = GAME_height;
	if (y < 0)
		y = 0;
}

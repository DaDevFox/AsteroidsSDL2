#include "camera.h"
#include "../main.h"

const int edge_buffer = 512;

void Camera::teleport(float x, float y) {
	this->x = x;
	this->y = y;
	desired_x = x;
	desired_y = y;
}

void Camera::set_zoom(float new_zoom) {
	zoom = new_zoom;

	x = desired_x - screen_to_world_x(WINDOW_width) / 2.0F;
	y = desired_y - screen_to_world_y(WINDOW_height) / 2.0F;
}

bool Camera::in_game(int screen_x, int screen_y) {
	int world_x = screen_x * zoom + x;
	int world_y = screen_y * zoom + y;
	return 
		world_x > 0 && world_x < GAME_width &&
		world_y > 0 && world_y < GAME_height;
}

bool Camera::screen_to_world(int screen_x, int screen_y, float* world_x, float* world_y) 
{
	*world_x = screen_x * zoom + x;
	*world_y = screen_y * zoom + y;
	return
		*world_x > 0 && *world_x < GAME_width &&
		*world_y > 0 && *world_y < GAME_height;
}

int Camera::screen_to_world_x(int screen_x) {
	return (int)(screen_x * zoom + x);
}
int Camera::screen_to_world_y(int screen_y) {
	return (int)(screen_y * zoom + y);
}

bool Camera::on_screen(float world_x, float world_y) {
	int screen_x = (int)((world_x - x) / zoom);
	int screen_y = (int)((world_y - y) / zoom);
	return
		screen_x > 0 && screen_x < WINDOW_width &&
		screen_y > 0 && screen_y < WINDOW_height;
}

bool Camera::world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y) {
	*screen_x = (int)((world_x - x) / zoom);
	*screen_y = (int)((world_y - y) / zoom);

	return
		*screen_x > 0 && *screen_x < WINDOW_width &&
		*screen_y > 0 && *screen_y < WINDOW_height;
}

int Camera::world_to_screen_x(float world_x) {
	return (int)((world_x - x)/zoom);
}
int Camera::world_to_screen_y(float world_y) {
	return (int)((world_y - y)/zoom);
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

		if (running_event->key.keysym.sym == SDLK_f)
		{
			set_zoom(zoom + 0.1f);
		}
		if (running_event->key.keysym.sym == SDLK_v)
		{
			set_zoom(zoom - 0.1f);
		}
	}
}

void Camera::update(float delta_time) {
	float true_desired_x = desired_x - screen_to_world_x(WINDOW_width)/2.0F;
	float true_desired_y = desired_y - screen_to_world_y(WINDOW_height)/2.0F;

	if(fabs(true_desired_x - x) > 0.1f)
		x = x + (true_desired_x - x) * delta_time * (1.0F - SETTING_camera_pan_smoothness);
	if(fabs(true_desired_y - y) > 0.1f)
		y = y + (true_desired_y - y) * delta_time * (1.0F - SETTING_camera_pan_smoothness);
	
	if (true_desired_x + screen_to_world_x(WINDOW_width) > GAME_width + edge_buffer)
		desired_x = GAME_width - screen_to_world_x(WINDOW_width) + screen_to_world_x(WINDOW_width) / 2.0F + edge_buffer;
	if (true_desired_x < -edge_buffer)
		desired_x = screen_to_world_x(WINDOW_width) / 2.0F - edge_buffer;
	/*if (true_desired_y + screen_to_world_y(WINDOW_height) > GAME_height)
		desired_y = GAME_height - screen_to_world_y(WINDOW_height) + screen_to_world_y(WINDOW_height) / 2.0F;
	if (true_desired_y < 0)
		desired_y = screen_to_world_y(WINDOW_height) / 2.0F;*/
}

#include "camera.h"
#include "../main.h"
#include <stdio.h>
#include "../entities/Entity.h"

const int edge_buffer = 512;

void Camera::set_zoom(float new_zoom) {
	zoom = SDL_min(SDL_max(new_zoom, SETTING_camera_zoom_min), SETTING_camera_zoom_max);

	x = desired_x - screen_to_world_x(WINDOW_width) / 2.0F;
	y = desired_y - screen_to_world_y(WINDOW_height) / 2.0F;
}

void Camera::teleport(float x, float y) {
	this->x = x;
	this->y = y;
	desired_x = x;
	desired_y = y;
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
	return (int)((world_x - x) / zoom);
}
int Camera::world_to_screen_y(float world_y) {
	return (int)((world_y - y) / zoom);
}

void Camera::input_update(SDL_Event* running_event)
{
	if (running_event->type == SDL_KEYDOWN)
	{
		auto sym = running_event->key.keysym.sym;


		if (sym == KEY_right || sym == KEY_right_alt)
		{
			desired_x += SETTING_camera_pan_speed;
		}
		if (sym == KEY_left || sym == KEY_left_alt)
		{
			desired_x -= SETTING_camera_pan_speed;
		}
		if (sym == KEY_down || sym == KEY_down_alt)
		{
			desired_y += SETTING_camera_pan_speed;
		}
		if (sym == KEY_up || sym == KEY_up_alt)
		{
			desired_y -= SETTING_camera_pan_speed;
		}

		if (sym == KEY_zoom_in || sym == KEY_zoom_in_alt)
		{
			set_zoom(zoom - 0.1f);
		}
		if (sym == KEY_zoom_out || sym == KEY_zoom_out_alt)
		{
			set_zoom(zoom + 0.1f);
		}
	}
}

void Camera::update(float delta_time) {
	float true_desired_x = desired_x - screen_to_world_x(WINDOW_width) / 2.0F;
	float true_desired_y = desired_y - screen_to_world_y(WINDOW_height) / 2.0F;

	if (fabs(true_desired_x - x) > 0.1f)
		x = x + (true_desired_x - x) * delta_time * (1.0F - SETTING_camera_pan_smoothness);
	if (fabs(true_desired_y - y) > 0.1f)
		y = y + (true_desired_y - y) * delta_time * (1.0F - SETTING_camera_pan_smoothness);

	if (DEBUG_focused_asteroid != -1) {
		Entity* entity = ((Entity*)entities) + GAME_ship_count + DEBUG_focused_asteroid;
		teleport(entity->x - (float)WINDOW_width * zoom * 0.5F, entity->y - (float)WINDOW_height * zoom * 0.5F);
	}

	if (true_desired_x + screen_to_world_x(WINDOW_width) > GAME_width + edge_buffer)
		desired_x = GAME_width - screen_to_world_x(WINDOW_width) + screen_to_world_x(WINDOW_width) / 2.0F + edge_buffer;
	if (true_desired_x < -edge_buffer)
		desired_x = screen_to_world_x(WINDOW_width) / 2.0F - edge_buffer;
	/*if (true_desired_y + screen_to_world_y(WINDOW_height) > GAME_height)
		desired_y = GAME_height - screen_to_world_y(WINDOW_height) + screen_to_world_y(WINDOW_height) / 2.0F;
	if (true_desired_y < 0)
		desired_y = screen_to_world_y(WINDOW_height) / 2.0F;*/
}

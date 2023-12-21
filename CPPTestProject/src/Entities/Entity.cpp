#include "Entity.h"
#include "../main.h"
#include <string.h>;

void Entity::update() 
{
	x += velocity_x * delta_time;
	y += velocity_y * delta_time;

	velocity_x = velocity_x + (desired_velocity_x - velocity_x) * delta_time * linear_wind_speed;
	velocity_y = velocity_y + (desired_velocity_y - velocity_y) * delta_time * linear_wind_speed;
	
	screen_x = (int)x;
	screen_y = (int)y;
}

RectEntity::RectEntity(const char* texture_file_path, int w, int h) {
	texture = window.load_texture(texture_file_path);
	this->x = 0;
	this->y = 0;
	this->w = w;
	this->h = h;
}

void RectEntity::init() {
	//texture = window.load_texture(texture_file_path);
}

void RectEntity::cleanup() {
	SDL_DestroyTexture(texture);
}

bool RectEntity::in_bounds(int screen_x, int screen_y) {
	return
		screen_x >= (this->screen_x - w / 2) && screen_x <= (this->screen_x + w / 2) &&
		screen_y >= (this->screen_y - h / 2) && screen_y <= (this->screen_y + h / 2);
}

void RectEntity::render(RenderWindow *window) {

	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
	window->render_rotate(0, 0, 0, 0, screen_x - w/2, screen_y - h/2, w, h, angle, texture);
}
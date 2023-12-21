#include "Entity.h"
#include "../main.h"
#include <string.h>;
#include <unordered_map>
#include <vector>


//int hash_entity(Entity entity) {
//	
//}

void Entity::update() 
{
	x += velocity_x * delta_time;
	y += velocity_y * delta_time;

	velocity_x = velocity_x + (desired_velocity_x - velocity_x) * delta_time * movement_windup_speed;
	velocity_y = velocity_y + (desired_velocity_y - velocity_y) * delta_time * movement_windup_speed;
	
	screen_x = (int)x;
	screen_y = (int)y;
}

RectEntity::RectEntity(const char* texture_file_path, int w, int h) {
	init(texture_file_path, w, h);
}

void RectEntity::init(const char* texture_file_path, int w, int h) {
	x = 0.0F;
	y = 0.0F;
	velocity_x = 0.0F;
	velocity_y = 0.0F;
	desired_velocity_x = 0.0F;
	desired_velocity_y = 0.0F;
	
	angle = 0.0F;

	movement_windup_speed = 0.005f;
	mass = 1.0F;
	drag = 0.1F;

	screen_x = 0;
	screen_y = 0;

	texture = window.load_texture(texture_file_path);
	this->w = w;
	this->h = h;
}

void RectEntity::cleanup() {
	SDL_DestroyTexture(texture);
}

bool RectEntity::in_bounds(int screen_x, int screen_y) {
	return
		screen_x >= (this->screen_x - w / 2) && screen_x <= (this->screen_x + w / 2) &&
		screen_y >= (this->screen_y - h / 2) && screen_y <= (this->screen_y + h / 2);
}

void RectEntity::render(RenderWindow *window) 
{
	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
	window->render_rotate(0, 0, 0, 0, screen_x - w/2, screen_y - h/2, w, h, angle, texture);
}
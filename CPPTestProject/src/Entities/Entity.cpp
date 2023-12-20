#include "Entity.h"
#include "../main.h"
#include <string.h>;

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

bool RectEntity::in_bounds(int x, int y) {
	return
		x >= (this->x - w / 2) && x <= (this->x + w / 2) &&
		y >= (this->y - h / 2) && y <= (this->y + h / 2);
}

void RectEntity::render(RenderWindow *window) {

	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
	window->render_rotate(0, 0, 0, 0, x - w/2, y - h/2, w, h, angle, texture);
}
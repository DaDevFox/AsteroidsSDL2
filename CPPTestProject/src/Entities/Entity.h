#pragma once
#include "../RenderWindow.h"

/// <summary>
/// ABSTRACT DO NOT USE; see RectEntity instead
/// </summary>
class Entity {
public: 
	int x;
	int y;
	
	void init();
	bool in_bounds(int x, int y);
	void render(RenderWindow *window);
	void cleanup();
};

class RectEntity : Entity {
public:
	int x;
	int y;
	int w;
	int h;

	double angle;
	int rel_pivot_x;
	int rel_pivot_y;

	SDL_Texture* texture;

	void init();
	bool in_bounds(int x, int y);
	void render(RenderWindow* window);
	void cleanup();

	RectEntity(const char* texture_file_path, int w, int h);
};
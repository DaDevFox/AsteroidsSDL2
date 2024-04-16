#pragma once
#include <SDL.h>
#include "../../main.h"

class ParallaxRenderer
{
private:
	SDL_Texture** layers = nullptr;
public:
	int layer_count = 4;

	float layer_parallax_multiple_exponent = 0.5F;
	float parallax_multiple = 0.1F;

	int layergen_points_variance = 3;
	int layergen_points = 20;
	float layergen_base_opacity = 1.0F;
	int layergen_points_avg_size = 10;
	float layergen_points_layer_exponent = 1.75F; // multiplied together [layer] times and then multiplied by layergen_points
	float layergen_opacity_layer_exponent = 0.5F; // multiplied together [layer] times and then multiplied by layergen_base_opacity
	float layergen_size_layer_exponent = 0.75F; // multiplied together [layer] times and then multiplied by layergen_points_avg_size


	float layer_movement_modifiers[];

	void init(const char** layer_image_paths = nullptr);

	void render(RenderWindow* window);

	void clean_up();
};


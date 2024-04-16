#include "parallax_renderer.h"
#include <chrono>
#include <set>
#include "../../window/RenderWindow.h"

int w = 512;
int h = 512;


void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* const target_pixel = (Uint32*)((Uint8*)surface->pixels
		+ y * surface->pitch
		+ x);
	*target_pixel = pixel;
}


void ParallaxRenderer::init(const char** layer_image_paths)
{
	if (layer_image_paths != nullptr)
		return; // TODO: image reading

	layers = new SDL_Texture * [layer_count];
	SDL_Surface* temp_surface = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(temp_surface);

	for (int i = 0; i < layer_count; i++)
	{
		srand(time(NULL));

		if (temp_surface == NULL)
		{
			SDL_Log("Error creating layer surface: %s\n", SDL_GetError());
			return;
		}

		Uint32* buffer = (Uint32*)temp_surface->pixels;

		float points_multiple = 1.0F;
		for (int j = 0; j < i; j++)
			points_multiple *= layergen_points_layer_exponent;

		int amount = rand() % layergen_points_variance + (int)((float)layergen_points * points_multiple);

		for (int i = 0; i < amount; i++)
		{
			int x = rand() % GAME_width;
			int y = rand() % GAME_height;

			float size_multiple = 1.0F;
			for (int j = 0; j < i; j++)
				size_multiple *= layergen_size_layer_exponent;

			std::set<int> visited;

			int size = rand() % layergen_points_variance + (int)((float)layergen_points * size_multiple);
			for (int j = 0; j < size; j++)
			{
				if (visited.find((x * w / GAME_width) + (y * h / GAME_height) * w) != visited.end() || y >= GAME_height || x >= GAME_width || y < 0 || x < 0)
				{
					// TODO: decide this
					// j--; 
					continue;
				}

				visited.insert((x * w / GAME_width) + (y * h / GAME_height) * w);
				Uint32 value = 0xAAAAAAAA;
				*(buffer + ((x * w / GAME_width) + (y * h / GAME_height) * w)) = value;


				int selection = rand() % 4;
				if (selection == 0)
				{
					x++;
					continue;
				}
				else if (selection == 1)
				{
					y++;
					continue;
				}
				else if (selection == 2)
				{
					x--;
					continue;
				}

				else if (selection == 2)
				{
					y--;
					continue;
				}
			}
		}
		layers[i] = window.create_texture_from_surface(temp_surface);

	}

	SDL_UnlockSurface(temp_surface);
	SDL_FreeSurface(temp_surface);
}

void ParallaxRenderer::render(RenderWindow* window)
{
	for (int i = 0; i < layer_count; i++)
	{
		float layer_opacity = layergen_base_opacity;
		float parallax_layer_multiple = parallax_multiple;
		for (int j = 0; j < i; j++)
		{
			parallax_layer_multiple *= layer_parallax_multiple_exponent;
			layer_opacity *= layergen_opacity_layer_exponent;
		}

		window->set_cull(false);
		window->render_alphamod(0, 0, 0, 0,
			(window->camera.x / window->camera.zoom - (float)GAME_width * 0.5F) * parallax_layer_multiple,
			(window->camera.y / window->camera.zoom - (float)GAME_height * 0.5F) * parallax_layer_multiple,
			GAME_width, GAME_height, layers[i], (Uint8)((float)255.0F * layer_opacity));
		window->set_cull(true);
	}
}

void ParallaxRenderer::clean_up()
{
	for (int i = 0; i < layer_count; i++)
		SDL_DestroyTexture(layers[i]);
	delete[] layers;
}

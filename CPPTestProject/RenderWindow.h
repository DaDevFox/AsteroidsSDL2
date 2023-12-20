#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class RenderWindow 
{

public:
	RenderWindow(const char* title, int width, int height);

	void cleanup();
	void init();

	/// <summary>
	/// Changes the current RenderDrawColor and draws a pixel at the given x and y pixel coordinate
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="color"></param>
	void render_point(int x, int y, SDL_Color color);
	/// <summary>
	/// Changes the current RenderDrawColor and draws a pixel at the given x and y pixel coordinate
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="color"></param>
	void render_point(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	/// <summary>
	/// Changes the current RenderDrawColor and draws a line
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="color"></param>
	void render_line(int x1, int y1, int x2, int y2, SDL_Color color);
	/// <summary>
	/// Changes the current RenderDrawColor and draws a line
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="color"></param>
	void render_line(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);


	void render(int x, int y, SDL_Texture* texture);
	void render(const SDL_Rect* destination, SDL_Texture* texture);
	void render(const SDL_Rect* source, const SDL_Rect* destination, SDL_Texture* texture);
	void render(int x, int y, int w, int h, SDL_Texture* texture);
	void render(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture);
	void render_center(int x, int y, int w, int h, SDL_Texture* texture);
	void render_center(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture);

	void render(float x, float y, const char* text, TTF_Font* font, SDL_Color color);
	void render_center(float x, float y, const char* text, TTF_Font* font, SDL_Color color);

	void draw();

	SDL_Texture* load_texture(const char* filename);

protected:
	SDL_Window* window;
	SDL_Renderer* renderer;
};
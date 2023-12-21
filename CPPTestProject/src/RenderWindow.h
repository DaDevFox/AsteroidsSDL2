#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class RenderWindow 
{

public:
	RenderWindow(const char* title, int width, int height);

	void clear();
	void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
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


	void render(int screen_x, int screen_y, SDL_Texture* texture);
	void render(const SDL_Rect* destination, SDL_Texture* texture);
	void render(const SDL_Rect* source, const SDL_Rect* destination, SDL_Texture* texture);
	void render(int screen_x, int screen_y, int w, int h, SDL_Texture* texture);
	void render(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture);
	void render_centered(int screen_x, int screen_y, int w, int h, SDL_Texture* texture);
	void render_centered(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture);
	void render_rotate(int screen_x, int screen_y, int w, int h, double angle, SDL_Texture* texture);
	/// <summary>
	/// Renders the region of the texture <code>texture</code> specified by src_x, src_y, src_w, src_h, rotated by <code>angle</code> about the center of the texture into the region of the screen specified by dst_x, dst_y, dst_w, dst_h.
	/// </summary>
	/// <param name="src_x"></param>
	/// <param name="src_y"></param>
	/// <param name="src_w"></param>
	/// <param name="src_h"></param>
	/// <param name="dst_x"></param>
	/// <param name="dst_y"></param>
	/// <param name="dst_w"></param>
	/// <param name="dst_h"></param>
	/// <param name="angle"></param>
	/// <param name="texture"></param>
	void render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, double angle, SDL_Texture* texture);
	void render_rotate(int screen_x, int screen_y, int w, int h, double angle, SDL_Point *center, SDL_RendererFlip flip, SDL_Texture* texture);
	void render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Texture* texture);

	void render(float screen_x, float screen_y, const char* text, TTF_Font* font, SDL_Color color);
	void render_centered(float screen_x, float screen_y, const char* text, TTF_Font* font, SDL_Color color);

	void draw();

	SDL_Texture* load_texture(const char* filename);

protected:
	SDL_Window* window;
	SDL_Renderer* renderer;
};
#pragma once
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "camera.h"
#include <vector>
#include <tuple>

class RenderWindow;

class UI
{
public:
	void input_update(SDL_Event* running_event);
	void render_update(RenderWindow* window);
	void update(float delta_time);
};


class RenderWindow
{

public:
	Camera camera;
	UI ui;


	RenderWindow(const char* title, int width, int height);
	~RenderWindow();

	RenderWindow(const RenderWindow& other);
	RenderWindow& operator=(const RenderWindow& other);


	void get_info(int* width, int* height) const;

	void clear();
	void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void cleanup();
	void init();

	/// <summary>
	/// DO NOT USE HORRENDOUSLY SLOW
	/// </summary>
	/// <param name="world_x"></param>
	/// <param name="world_y"></param>
	/// <returns></returns>
	SDL_Color get_pixel_color(float world_x, float world_y);

	void render_rect_outline(float world_x, float world_y, float world_w, float world_h, const SDL_Color& color);
	void render_rect(float world_x, float world_y, float world_w, float world_h, SDL_Color color);
	void render_rect(int screen_x, int screen_y, int screen_w, int screen_h, SDL_Color color);
	void render_rect_alphamod(float world_x, float world_y, float world_w, float world_h, SDL_Color color, Uint8 alpha);


	/// <summary>
	/// Changes the current RenderDrawColor and draws a pixel at the given x and y pixel coordinate
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="color"></param>
	void render_point(float x, float y, SDL_Color color);
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
	void render_line(int x1, int y1, int x2, int y2, const SDL_Color& color);
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
	void render_alphamod(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture, const Uint8& alpha);
	void render_centered(int x, int y, int w, int h, SDL_Texture* texture);
	void render_centered(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture);
	void render_rotate(int x, int y, int w, int h, double angle, SDL_Texture* texture);
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
	void render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, int dst_pivot_x, int dst_pivot_y, double angle, SDL_Texture* texture);
	void render_rotate(int x, int y, int w, int h, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Texture* texture);
	void render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Texture* texture);

	void render(float x, float y, const char* text, TTF_Font* font, SDL_Color color);
	void render_centered_screen(float x, float y, const char* text, TTF_Font* font, SDL_Color color);
	void render_centered_world(float x, float y, const char* text, TTF_Font* font, SDL_Color color);

	void render_pixel_deferred(float time, float x, float y, SDL_Color color);

	void render_all_deferred();

	void draw();

	SDL_Texture* create_texture_from_surface(SDL_Surface* surface);
	SDL_Texture* load_texture(const char* filename);

protected:
	SDL_Window* window;
	SDL_Renderer* renderer;

private:
	std::vector<std::tuple<float, float, float, SDL_Color>> deferred_render_calls;

	bool on_screen(float world_x, float world_y);
	bool world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y);
	int world_to_screen_x(float world_x);
	int world_to_screen_y(float world_y);
};


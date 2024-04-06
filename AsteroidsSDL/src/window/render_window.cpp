#include "RenderWindow.h"
#include "../main.h"
#include <iostream>

RenderWindow::RenderWindow(const char* title, int width, int height)
{
	camera.x = 0.0F;
	camera.y = 0.0F;

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (window == nullptr)
	{
		SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}
	if (renderer == nullptr)
	{
		SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		SDL_Log("Error on IMG_Init; IMG_INIT_PNG flag not initialized: %s\n", SDL_GetError());
		exit(1);
	}
}

RenderWindow::RenderWindow(const RenderWindow& other)
{
	std::cout << "Copy Constructor for RenderWindow called! Expensive!";

	camera.x = 0.0F;
	camera.y = 0.0F;
	// FLAG: no renderer sharing; that's weird
	window = SDL_CreateWindow("Untitled Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (window == nullptr)
	{
		SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}

	if (renderer == nullptr)
	{
		SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		SDL_Log("Error on IMG_Init; IMG_INIT_PNG flag not initialized: %s\n", SDL_GetError());
		exit(1);
	}
}

RenderWindow& RenderWindow::operator=(const RenderWindow& other)
{
	camera.x = 0.0F;
	camera.y = 0.0F;
	// FLAG: no renderer sharing; that's weird
	window = SDL_CreateWindow("Untitled Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (window == nullptr)
	{
		SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}
	if (renderer == nullptr)
	{
		SDL_Log("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		SDL_Log("Error on IMG_Init; IMG_INIT_PNG flag not initialized: %s\n", SDL_GetError());
		exit(1);
	}
}

RenderWindow::~RenderWindow()
{
	cleanup();
}

void RenderWindow::init()
{

}


void RenderWindow::clear()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	if (SDL_RenderClear(renderer) < 0)
		SDL_Log("Error on clear RenderWindow: %s\n", SDL_GetError());
}

void RenderWindow::clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	if (SDL_RenderClear(renderer) < 0)
		SDL_Log("Error on clear RenderWindow: %s\n", SDL_GetError());
}

void RenderWindow::get_info(int* width, int* height) const
{
	SDL_GetWindowSize(window, width, height);
}

void RenderWindow::cleanup()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}


SDL_Color RenderWindow::get_pixel_color(float world_x, float world_y)
{
	SDL_Rect rect = { 0,0,1,1 };
	camera.world_to_screen(world_x, world_y, &rect.x, &rect.y);
	Uint32* buffer = nullptr;
	SDL_RenderReadPixels(renderer, &rect, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, buffer, sizeof(int));

	if (buffer == nullptr)
		return { 0, 0, 0, 0 };

	Uint8 r = *((Uint8*)buffer);
	Uint8 g = *((Uint8*)buffer + 1);
	Uint8 b = *((Uint8*)buffer + 2);
	Uint8 a = *((Uint8*)buffer + 3);

	return { r, g, b, a };

}

#pragma region Rendering

bool RenderWindow::on_screen(float world_x, float world_y)
{
	return camera.on_screen(world_x, world_y);
}

bool RenderWindow::world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y)
{
	return camera.world_to_screen(world_x, world_y, screen_x, screen_y);
}

int RenderWindow::world_to_screen_x(float world_x)
{
	return camera.world_to_screen_x(world_x);
}

int RenderWindow::world_to_screen_y(float world_y)
{
	return camera.world_to_screen_y(world_y);
}

#pragma region Basic Geometry

void RenderWindow::render_pixel_deferred(float time, float x, float y, SDL_Color color)
{
	deferred_render_calls.push_back(std::tuple<float, float, float, SDL_Color>(time, x, y, color));
}

void RenderWindow::render_rect_outline(float world_x, float world_y, float world_w, float world_h, const SDL_Color& color)
{
	render_line(world_x, world_y, world_x + world_w, world_y, color);
	render_line(world_x + world_w, world_y, world_x + world_w, world_y + world_h, color);
	render_line(world_x + world_w, world_y + world_h, world_x, world_y + world_h, color);
	render_line(world_x, world_y + world_h, world_x, world_y, color);
}

void RenderWindow::render_rect(int screen_x, int screen_y, int screen_w, int screen_h, SDL_Color color)
{
	SDL_Rect rect{ screen_x, screen_y, screen_w, screen_h };
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void RenderWindow::render_rect(float world_x, float world_y, float world_w, float world_h, SDL_Color color)
{
	SDL_Rect rect{
		0,
		0,
		0,
		0
	};

	if (world_to_screen(world_x, world_y, &rect.x, &rect.y) && world_to_screen(world_x + world_w, world_y + world_h, &rect.w, &rect.h))
	{
		// w and h temporarily store screen_x2 and screen_y2
		rect.w -= rect.x;
		rect.h -= rect.y;

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void RenderWindow::render_rect_alphamod(float world_x, float world_y, float world_w, float world_h, SDL_Color color, Uint8 alpha)
{
	SDL_Rect rect{
		0,
		0,
		0,
		0
	};

	if (world_to_screen(world_x, world_y, &rect.x, &rect.y) && world_to_screen(world_x + world_w, world_y + world_h, &rect.w, &rect.h))
	{
		// w and h temporarily store screen_x2 and screen_y2
		rect.w -= rect.x;
		rect.h -= rect.y;
		// BROKNE
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	}
}

void RenderWindow::render_rect_alphamod(int screen_x, int screen_y, int screen_w, int screen_h, SDL_Color color, Uint8 alpha)
{
	SDL_Rect rect{
		screen_x,
		screen_y,
		screen_w,
		screen_h
	};

	// BROKNE
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}


void RenderWindow::render_point(float x, float y, SDL_Color color)
{
	int screen_x;
	int screen_y;
	if (world_to_screen(x, y, &screen_x, &screen_y))
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawPoint(renderer, screen_x, screen_y);
	}
}

void RenderWindow::render_point(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int screen_x;
	int screen_y;
	if (world_to_screen(x, y, &screen_x, &screen_y))
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
		SDL_RenderDrawPoint(renderer, screen_x, screen_y);
	}
}

void RenderWindow::render_line(int x1, int y1, int x2, int y2, const SDL_Color& color)
{
	int screen_x1;
	int screen_y1;
	int screen_x2;
	int screen_y2;
	world_to_screen(x1, y1, &screen_x1, &screen_y1);
	world_to_screen(x2, y2, &screen_x2, &screen_y2);
	SDL_Rect rect = { 0, 0, WINDOW_width, WINDOW_height };

	if (SDL_IntersectRectAndLine(&rect, &screen_x1, &screen_y1, &screen_x2, &screen_y2))
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(renderer, screen_x1, screen_y1, screen_x2, screen_y2);
	}
}

void RenderWindow::render_line(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int screen_x1;
	int screen_y1;
	int screen_x2;
	int screen_y2;
	if (world_to_screen(x1, y1, &screen_x1, &screen_y1) || world_to_screen(x2, y2, &screen_x2, &screen_y2))
	{
		SDL_SetRenderDrawColor(renderer, r, g, b, a);
		SDL_RenderDrawLine(renderer, screen_x1, screen_y1, screen_x2, screen_y2);
	}
}


#pragma endregion

#pragma region Textures

void RenderWindow::render(int x, int y, SDL_Texture* texture)
{
	int screen_x;
	int screen_y;
	int w;
	int h;

	SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

	if (world_to_screen(x, y, &screen_x, &screen_y) || on_screen(x + w, y + h))
	{
		SDL_Rect destination{
			screen_x,
			screen_y,
			w,
			h
		};

		SDL_RenderCopy(renderer, texture, nullptr, &destination);
	}
}

void RenderWindow::render(int x, int y, int w, int h, SDL_Texture* texture)
{
	int screen_x;
	int screen_y;

	if (world_to_screen(x, y, &screen_x, &screen_y) || on_screen(x + w, y + h))
	{
		SDL_Rect destination{
			x,
			y,
			w,
			h
		};

		SDL_RenderCopy(renderer, texture, nullptr, &destination);
	}
}

void RenderWindow::render(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture)
{
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, nullptr, nullptr, &source.w, &source.h);

	int screen_dest_x_end;
	int screen_dest_y_end;

	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};
	bool flag1 = world_to_screen(dst_x, dst_y, &destination.x, &destination.y);
	bool flag2 = world_to_screen(dst_x + dst_w, dst_y + dst_h, &screen_dest_x_end, &screen_dest_y_end);

	if (flag1 || flag2 || on_screen(dst_x + dst_w, dst_y) || on_screen(dst_x, dst_y + dst_h))
	{
		destination.w = screen_dest_x_end - destination.x;
		destination.h = screen_dest_y_end - destination.y;

		SDL_RenderCopy(renderer, texture, &source, &destination);
	}
}

void RenderWindow::render_alphamod(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture, const Uint8& alpha)
{
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, nullptr, nullptr, &source.w, &source.h);

	int screen_dest_x_end;
	int screen_dest_y_end;

	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};
	bool flag1 = world_to_screen(dst_x, dst_y, &destination.x, &destination.y);
	bool flag2 = world_to_screen(dst_x + dst_w, dst_y + dst_h, &screen_dest_x_end, &screen_dest_y_end);

	if (flag1 || flag2 || on_screen(dst_x + dst_w, dst_y) || on_screen(dst_x, dst_y + dst_h))
	{
		destination.w = screen_dest_x_end - destination.x;
		destination.h = screen_dest_y_end - destination.y;

		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_RenderCopy(renderer, texture, &source, &destination);
	}
}

void RenderWindow::render(const SDL_Rect* destination, SDL_Texture* texture)
{
	int screen_dst_x;
	int screen_dst_y;

	if (world_to_screen(destination->x, destination->y, &screen_dst_x, &screen_dst_y) || on_screen(destination->x + destination->w, destination->y + destination->h))
	{
		SDL_Rect _destination{
			screen_dst_x,
			screen_dst_y,
			destination->w,
			destination->w
		};


		SDL_RenderCopy(renderer, texture, nullptr, &_destination);
	}
}

/// <summary>
/// FLAG: uses SCREEN not WORLD coordinates
/// </summary>
/// <param name="source"></param>
/// <param name="destination"></param>
/// <param name="texture"></param>
void RenderWindow::render(const SDL_Rect* source, const SDL_Rect* destination, SDL_Texture* texture)
{
	SDL_RenderCopy(renderer, texture, source, destination);
}

/// <summary>
/// FLAG: uses SCREEN not WORLD coordinates
/// </summary>
void RenderWindow::render_centered(int x, int y, int w, int h, SDL_Texture* texture)
{
	SDL_Rect destination{
		x - w / 2,
		y - h / 2,
		w,
		h
	};

	SDL_RenderCopy(renderer, texture, nullptr, &destination);
}

void RenderWindow::render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, double angle, SDL_Texture* texture)
{
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, nullptr, nullptr, &source.w, &source.h);

	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};

	int screen_dest_x_end = dst_x;
	int screen_dest_y_end = dst_y;

	bool flag1 = world_to_screen(dst_x, dst_y, &destination.x, &destination.y);
	bool flag2 = world_to_screen(dst_x + dst_w, dst_y + dst_h, &screen_dest_x_end, &screen_dest_y_end);
	if (flag1 || flag2)
	{
		destination.w = screen_dest_x_end - destination.x;
		destination.h = screen_dest_y_end - destination.y;

		SDL_Point center = {
			destination.w >> 1,
			destination.h >> 1
		};

		SDL_RenderCopyEx(renderer, texture, &source, &destination, 180.0 * (angle / PI), &center, SDL_FLIP_NONE);
	}
}


void RenderWindow::render_rotate_alphamod(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, double angle, SDL_Texture* texture, Uint8 alpha)
{
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, nullptr, nullptr, &source.w, &source.h);

	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};

	int screen_dest_x_end = dst_x;
	int screen_dest_y_end = dst_y;

	bool flag1 = world_to_screen(dst_x, dst_y, &destination.x, &destination.y);
	bool flag2 = world_to_screen(dst_x + dst_w, dst_y + dst_h, &screen_dest_x_end, &screen_dest_y_end);
	if (flag1 || flag2)
	{
		destination.w = screen_dest_x_end - destination.x;
		destination.h = screen_dest_y_end - destination.y;

		SDL_Point center = {
			destination.w >> 1,
			destination.h >> 1
		};

		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_RenderCopyEx(renderer, texture, &source, &destination, 180.0 * (angle / PI), &center, SDL_FLIP_NONE);
	}
}

void RenderWindow::render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, int dst_pivot_x, int dst_pivot_y, double angle, SDL_Texture* texture)
{
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, nullptr, nullptr, &source.w, &source.h);

	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};

	int screen_dest_x_end = dst_x;
	int screen_dest_y_end = dst_y;

	bool flag1 = world_to_screen(dst_x, dst_y, &destination.x, &destination.y);
	bool flag2 = world_to_screen(dst_x + dst_w, dst_y + dst_h, &screen_dest_x_end, &screen_dest_y_end);
	if (flag1 || flag2)
	{
		destination.w = screen_dest_x_end - destination.x;
		destination.h = screen_dest_y_end - destination.y;

		SDL_Point center;

		center.x = world_to_screen_x(dst_x + dst_pivot_x) - world_to_screen_x(dst_x);
		center.y = world_to_screen_y(dst_y + dst_pivot_y) - world_to_screen_y(dst_y);

		SDL_RenderCopyEx(renderer, texture, &source, &destination, 180.0 * (angle / PI), &center, SDL_FLIP_NONE);
	}
}

void RenderWindow::render_rotate(int screen_x, int screen_y, int w, int h, double angle, SDL_Texture* texture)
{
	SDL_Rect destination{
		screen_x,
		screen_y,
		w,
		h
	};


	if (SDL_RenderCopyEx(renderer, texture, nullptr, &destination, angle, nullptr, SDL_FLIP_NONE) < 0)
		SDL_Log("Error rendering texture with rotation: %s\n", SDL_GetError());
}



//void RenderWindow::render_rotate(int x, int y, int w, int h, double angle, SDL_Texture* texture) {
//	SDL_Rect destination{
//		x,
//		y,
//		w,
//		h
//	};
//
//	SDL_Point center = {
//		x + (w >> 1),
//		y + (h >> 1)
//	};
//
//	SDL_RenderCopyEx(renderer, texture, nullptr, &destination, angle, &center, SDL_FLIP_NONE);
//}


#pragma endregion

#pragma region Text

void RenderWindow::render(float x, float y, const char* text, TTF_Font* font, SDL_Color color)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;


	SDL_Rect dst;
	dst.w = src.w;
	dst.h = src.h;

	if (world_to_screen(x, y, &(dst.x), &(dst.y)) || on_screen(x + dst.w, y + dst.h))
	{
		SDL_RenderCopy(renderer, message, &src, &dst);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(message);
	}
}

void RenderWindow::render_centered_screen(float x, float y, const char* text, TTF_Font* font, SDL_Color color)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
	if (surfaceMessage == nullptr)
	{
		SDL_Log("Error rendering text; aborting: %s\n", SDL_GetError());
		return;
	}

	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	if (message == nullptr)
	{
		SDL_Log("Error rendering text; aborting: %s\n", SDL_GetError());
		return;
	}

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;
	dst.x = x - (src.w >> 1);
	dst.y = y - (src.h >> 1);
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);

}

void RenderWindow::render_centered_world(float x, float y, const char* text, TTF_Font* font, SDL_Color color)
{
	SDL_Rect dst;
	// NOTE: 1-byte Latin1 text (incl. ASCII) only
	TTF_SizeText(font, text, &(dst.w), &(dst.h));

	if (world_to_screen((int)x - (dst.w >> 1), (int)y - (dst.h >> 1), &(dst.x), &(dst.y)) || on_screen(x + dst.w, y + dst.h))
	{
		SDL_Rect src;
		src.x = 0;
		src.y = 0;
		src.w = dst.w;
		src.h = dst.h;

		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
		if (surfaceMessage == nullptr)
		{
			SDL_Log("Error rendering text; aborting: %s\n", SDL_GetError());
			return;
		}

		SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		if (message == nullptr)
		{
			SDL_Log("Error rendering text; aborting: %s\n", SDL_GetError());
			SDL_FreeSurface(surfaceMessage);
			return;
		}


		SDL_RenderCopy(renderer, message, &src, &dst);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(message);
	}
}

#pragma endregion

#pragma endregion

void RenderWindow::draw()
{
	SDL_RenderPresent(renderer);
}

void RenderWindow::render_all_deferred()
{
	if (deferred_render_calls.empty())
		return;

	auto it = deferred_render_calls.begin();
	while (it != deferred_render_calls.end())
	{
		std::tuple<float, float, float, SDL_Color> tuple = *it;
		std::get<0>(tuple) -= delta_time;
		if (std::get<1>(tuple) < 0.0F)
		{
			it = deferred_render_calls.erase(it);
			continue;
		}

		render_rect(std::get<1>(tuple), std::get<2>(tuple), 1.0F, 1.0F, std::get<3>(tuple));
		it++;
	}

}

SDL_Texture* RenderWindow::create_texture_from_surface(SDL_Surface* surface)
{
	SDL_Texture* result = SDL_CreateTextureFromSurface(renderer, surface);
	if (result == nullptr)
		SDL_LogError(SDL_LOG_PRIORITY_DEBUG, "Failed to create texture from surface: %s\n", SDL_GetError());

	return result;
}

SDL_Texture* RenderWindow::load_texture(const char* file_path)
{
	SDL_Texture* result = IMG_LoadTexture(renderer, file_path);
	if (result == nullptr)
		SDL_LogError(SDL_LOG_PRIORITY_DEBUG, "Failed to load texture %s: %s\n", file_path, SDL_GetError());


	return result;
}


#include "RenderWindow.h"

RenderWindow::RenderWindow(const char *title, int width, int height) {
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (window == NULL)
	{
		SDL_Log("Window could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}
	if (renderer == NULL)
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

void RenderWindow::init() {

}

void RenderWindow::clear() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	if (SDL_RenderClear(renderer) < 0)
		SDL_Log("Error on clear RenderWindow: %s\n", SDL_GetError());
}

void RenderWindow::clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	if (SDL_RenderClear(renderer) < 0)
		SDL_Log("Error on clear RenderWindow: %s\n", SDL_GetError());
}

void RenderWindow::cleanup() {
	SDL_DestroyWindow(window);
}

#pragma region Rendering

#pragma region Basic Geometry

void RenderWindow::render_point(int screen_x, int screen_y, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoint(renderer, screen_x, screen_y);
}

void RenderWindow::render_point(int screen_x, int screen_y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawPoint(renderer, screen_x, screen_y);
}

void RenderWindow::render_line(int x1, int y1, int x2, int y2, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void RenderWindow::render_line(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}


#pragma endregion

#pragma region Textures

void RenderWindow::render(int x, int y, SDL_Texture* texture) {
	SDL_Rect destination{
		x,
		y,
		0,
		0
	};

	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

void RenderWindow::render(int x, int y, int w, int h, SDL_Texture* texture) {
	SDL_Rect destination{
		x,
		y,
		w,
		h
	};

	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

void RenderWindow::render(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, SDL_Texture* texture) {
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, NULL, NULL, &source.w, &source.h);

	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};

	SDL_RenderCopy(renderer, texture, &source, &destination);
}

void RenderWindow::render(const SDL_Rect* destination, SDL_Texture* texture) {
	SDL_RenderCopy(renderer, texture, NULL, destination);
}

void RenderWindow::render(const SDL_Rect* source, const SDL_Rect* destination, SDL_Texture* texture) {
	SDL_RenderCopy(renderer, texture, source, destination);
}


void RenderWindow::render_centered(int x, int y, int w, int h, SDL_Texture* texture) {
	SDL_Rect destination{
		x - w/2,
		y - h/2,
		w,
		h
	};

	SDL_RenderCopy(renderer, texture, NULL, &destination);
}

void RenderWindow::render_rotate(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, double angle, SDL_Texture* texture) {
	SDL_Rect source{
		src_x,
		src_y,
		src_w,
		src_h
	};

	// adjusts source.w and source.h to actual values?
	SDL_QueryTexture(texture, NULL, NULL, &source.w, &source.h);
	
	SDL_Rect destination{
		dst_x,
		dst_y,
		dst_w,
		dst_h
	};

	SDL_Point center = {
		dst_w/2,
		dst_h/2
	};

	SDL_RenderCopyEx(renderer, texture, &source, &destination, angle, &center, SDL_FLIP_NONE);
}

void RenderWindow::render_rotate(int screen_x, int screen_y, int w, int h, double angle, SDL_Texture* texture) {
	SDL_Rect destination{
		screen_x,
		screen_y,
		w,
		h
	};


	if (SDL_RenderCopyEx(renderer, texture, NULL, &destination, angle, NULL, SDL_FLIP_NONE) < 0)
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
//		x + w / 2,
//		y + h / 2
//	};
//
//	SDL_RenderCopyEx(renderer, texture, NULL, &destination, angle, &center, SDL_FLIP_NONE);
//}


#pragma endregion

#pragma region Text

void RenderWindow::render(float screen_x, float screen_y, const char* text, TTF_Font* font, SDL_Color color) 
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;
	dst.x = screen_x;
	dst.y = screen_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}

void RenderWindow::render_centered(float screen_x, float screen_y, const char* text, TTF_Font* font, SDL_Color color)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
	if (surfaceMessage == NULL) {
		SDL_Log("Error rendering text; aborting: %s\n", SDL_GetError());
		return;
	}

	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	if (message == NULL) {
		SDL_Log("Error rendering text; aborting: %s\n", SDL_GetError());
		return;
	}

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;

	// FLAG: these hardcoded constants!!!
	
	dst.x = 640 / 2 - src.w / 2 + screen_x;
	dst.y = 480 / 2 - src.h / 2 + screen_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}

#pragma endregion

void RenderWindow::draw() {
	SDL_RenderPresent(renderer);
}

#pragma endregion

SDL_Texture* RenderWindow::load_texture(const char* file_path) {
	SDL_Texture* result = IMG_LoadTexture(renderer, file_path);
	if (result == NULL)
		SDL_LogError(SDL_LOG_PRIORITY_DEBUG, "Failed to load texture %s: %s\n", file_path, SDL_GetError());
	
	
	return result;
}


#include "../main.h"

void UI::input_update(SDL_Event* running_event) {
	if (running_event->type == SDL_KEYDOWN) {
#pragma region debug input
		SDL_Keycode sym = running_event->key.keysym.sym;

		if (sym == KEY_DEBUG_master)
			DEBUG_master = !DEBUG_master;

		if (sym == KEY_DEBUG_chunk_gridlines)
			DEBUG_chunk_gridlines = !DEBUG_chunk_gridlines;
		if (sym == KEY_DEBUG_display_chunk_numbers)
			DEBUG_display_chunk_numbers = !DEBUG_chunk_gridlines;
		if (sym == KEY_DEBUG_display_entity_outlines)
			DEBUG_display_entity_outlines = !DEBUG_display_entity_outlines;
		if (sym == KEY_DEBUG_highlight_capstone_asteroid)
			DEBUG_highlight_capstone_asteroid = !DEBUG_highlight_capstone_asteroid;
		if (sym == KEY_DEBUG_wireframe_mode)
			DEBUG_wireframe_mode = !DEBUG_wireframe_mode;

#pragma endregion

	}
}

void UI::render_update(RenderWindow* window) {
	int debug_bar_offset = 20;
	const char* debug_header_text = "debug";
	int debug_bar_height = 50;
	SDL_Color debug_bar_bg_color = { 150, 150, 150, 255 };
	SDL_Color debug_bar_bg_accent_color = { 100, 100, 100, 255 };
	SDL_Color debug_mode_indicator_color_on = { 0, 140, 0, 255 };
	SDL_Color debug_mode_indicator_color_off = { 30, 30, 30, 255 };
	int debug_mode_indicator_width = 150;


	if (DEBUG_master)
	{
		bool modes[] = { DEBUG_wireframe_mode, DEBUG_chunk_gridlines, DEBUG_display_entity_outlines };
		const char* text[] = { "wireframe", "grid", "outlines" };

		window->render_rect()

			for (int i = 0; i < 3; i++) {
				window->render_rect(
					i * debug_mode_indicator_width, debug_bar_offset,
					debug_mode_indicator_width, debug_bar_height,
					modes[i] ? debug_mode_indicator_color_on : debug_mode_indicator_color_off);

				window->render_centered_screen((float)debug_mode_indicator_width * 0.5F + (float)(i * debug_mode_indicator_width), (float)debug_bar_height * 0.5F, text[i], encode_sans_medium, { 255, 255, 255, 255 });
			}


		if (DEBUG_chunk_gridlines)
		{
			for (int x = 0; x < GAME_width; x += chunk_size)
				window->render_line(x, 0, x, GAME_height, { 200, 200, 200, 255 });

			for (int y = 0; y < GAME_height; y += chunk_size)
				window->render_line(0, y, GAME_width, y, { 200, 200, 200, 255 });
		}
	}
}


void UI::update(float delta_time) {

}

#include "../main.h"
#include "../entities/Entity.h"

void debug_update(SDL_Keycode sym);
void debug_update(SDL_Keycode sym)
{
	if (DEBUG_master)
	{
		if (sym == KEY_DEBUG_mode)
			DEBUG_mode = !DEBUG_mode;

		if (sym == KEY_DEBUG_focused_asteroid_cancel)
			CAMERA_focused_asteroid = -1;
		if (sym == KEY_DEBUG_focused_asteroid_increment)
			CAMERA_focused_asteroid = ++CAMERA_focused_asteroid >= GAME_asteroid_count ? GAME_asteroid_count - 1 : CAMERA_focused_asteroid;
		if (sym == KEY_DEBUG_focused_asteroid_decrement)
			CAMERA_focused_asteroid = --CAMERA_focused_asteroid < 0 ? -1 : CAMERA_focused_asteroid;

		if (sym == KEY_DEBUG_chunk_gridlines)
			DEBUG_chunk_gridlines = !DEBUG_chunk_gridlines;
		if (sym == KEY_DEBUG_ship_targets)
			DEBUG_ship_targets = !DEBUG_ship_targets;
		if (sym == KEY_DEBUG_display_chunk_numbers)
			DEBUG_display_chunk_numbers = !DEBUG_chunk_gridlines;
		if (sym == KEY_DEBUG_display_entity_outlines)
			DEBUG_display_entity_outlines = !DEBUG_display_entity_outlines;
		if (sym == KEY_DEBUG_ships_fire_at_will)
			DEBUG_ships_fire_at_will = !DEBUG_ships_fire_at_will;
		if (sym == KEY_DEBUG_wireframe_mode)
			DEBUG_wireframe_mode = !DEBUG_wireframe_mode;
	}
	else
		CAMERA_focused_asteroid = PLAYER_asteroid_id;
}

void render_player_ui_update(RenderWindow* window);
void render_player_ui_update(RenderWindow* window)
{
	int sidebar_width = 200;
	int sidebar_height = -1;
	int titletext_height = 20;

	int healthbar_height = 40;
	int healthbar_padding = 0;

	SDL_Color sidebar_bg_color = { 80, 80, 80, 255 };
	SDL_Color healthbar_bg_color = { 100, 100, 100, 255 };
	SDL_Color healthbar_color = { 0, 255, 0, 255 };

	Entity* player = (Entity*)entities + PLAYER_entity_id;
	float health_normalized = (float)(player->outline_point_count - GAME_min_outline_point_count) / (float)(PLAYER_initial_outline_point_count - GAME_min_outline_point_count);

	if (health_normalized <= 0.3F)
		healthbar_color = { 255, 255, 0, 255 };
	if (health_normalized <= 0.1F)
		healthbar_color = { 255, 0, 0, 255 };

	// sidebar
	window->render_rect(WINDOW_width - sidebar_width, 0, sidebar_width, sidebar_height == -1 ? WINDOW_height : sidebar_height, sidebar_bg_color);

	// healthbar title
	window->render_centered_screen(WINDOW_width - (sidebar_width >> 1), titletext_height >> 1, "Health", encode_sans_bold, healthbar_color);

	// healthbar bg + fill
	window->render_rect(WINDOW_width - sidebar_width + healthbar_padding, titletext_height, sidebar_width - 2 * healthbar_padding, healthbar_height, healthbar_bg_color);
	window->render_rect(WINDOW_width - sidebar_width + healthbar_padding, titletext_height, (int)(health_normalized * (float)(sidebar_width - 2 * healthbar_padding)), healthbar_height, healthbar_color);
}

void render_debug_update(RenderWindow* window);
void render_debug_update(RenderWindow* window)
{
	int curr_y = 0;
	int curr_x = 0;

	int debug_bar_offset = 30;
	const char* debug_header_text = "debug menu";
	int debug_bar_height = 50;
	SDL_Color debug_bar_bg_color = { 150, 150, 150, 255 };
	SDL_Color debug_bar_bg_accent_color = { 100, 100, 100, 255 };
	SDL_Color debug_mode_indicator_color_on = { 0, 140, 0, 255 };
	SDL_Color debug_mode_indicator_color_off = { 30, 30, 30, 255 };
	int debug_mode_indicator_width = 150;


	if (DEBUG_mode)
	{
		bool modes[] = { DEBUG_wireframe_mode, DEBUG_chunk_gridlines, DEBUG_display_entity_outlines, DEBUG_ships_fire_at_will, DEBUG_ship_targets };
		const char* text[] = { "wireframe", "grid", "outlines", "fire at will", "show ship targets" };

		window->render_rect(0, 0, WINDOW_width, debug_bar_offset, debug_bar_bg_accent_color);
		window->render_centered_screen((float)WINDOW_width * 0.5F, debug_bar_offset * 0.5F, debug_header_text, encode_sans_medium, { 255, 255, 255, 255 });

		// Debug Mode Indicators
		for (int i = 0; i < sizeof(modes) / sizeof(bool); i++)
		{
			window->render_rect(
				i * debug_mode_indicator_width, debug_bar_offset,
				debug_mode_indicator_width, debug_bar_height,
				modes[i] ? debug_mode_indicator_color_on : debug_mode_indicator_color_off);

			window->render_centered_screen(
				(float)debug_mode_indicator_width * 0.5F + (float)(i * debug_mode_indicator_width),
				(float)debug_bar_height * 0.5F + debug_bar_offset,
				text[i], encode_sans_medium, { 255, 255, 255, 255 });

			curr_x += debug_mode_indicator_width;
		}

		if (CAMERA_focused_asteroid != -1)
		{
			window->render_rect(
				curr_x * debug_mode_indicator_width, debug_bar_offset,
				debug_mode_indicator_width, debug_bar_height,
				debug_mode_indicator_color_on);

			char output[16];
			sprintf_s(output, "focused: %i", CAMERA_focused_asteroid);

			window->render_centered_screen(
				debug_mode_indicator_width * 0.5F + (float)(curr_x * debug_mode_indicator_width),
				(float)debug_bar_height * 0.5F + debug_bar_offset,
				output, encode_sans_medium, { 255, 255, 255, 255 });
		}

		if (DEBUG_chunk_gridlines)
		{
			for (int x = 0; x < GAME_width; x += chunk_size)
				window->render_line(x, 0, x, GAME_height, { 200, 200, 200, 255 });

			for (int y = 0; y < GAME_height; y += chunk_size)
				window->render_line(0, y, GAME_width, y, { 200, 200, 200, 255 });
		}

		curr_y += debug_bar_offset + debug_bar_height;
	}

	if (delta_time == 0.0F)
	{
		// FPS HUD
		char output[40] = "PAUSED";
		window->render_centered_screen(WINDOW_width / 2.0F, curr_y + 20, output, encode_sans_bold, { 255, 255, 255, 255 });
		curr_y += 20;
	}

	if (UI_FPSHUD_enabled)
	{
		// FPS HUD
		char output[40];

		sprintf_s(output, "%.1d fps; (%.1d, %.1d)", (int)((1000.0F / delta_time)), (int)window->camera.x, (int)window->camera.y);
		window->render_centered_screen(WINDOW_width / 2.0F, curr_y + 20, output, encode_sans_medium, { 255, 255, 255, 255 });
	}
}

void UI::input_update(SDL_Event* running_event)
{
	if (running_event->type == SDL_KEYDOWN)
	{
		SDL_Keycode sym = running_event->key.keysym.sym;

		debug_update(sym);

		if (sym == KEY_pause)
		{
			time_scaling = time_scaling > 0.0F ? 0.0F : 1.0F;
		}
	}
}

void UI::render_update(RenderWindow* window)
{
	render_debug_update(window);
	render_player_ui_update(window);
}


void UI::update(float delta_time)
{}

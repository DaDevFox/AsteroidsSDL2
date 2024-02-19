#pragma once
#include <SDL.h>

#define SETTING_MAX_POLYGON_VERTICES 512

extern const char* RESOURCE_ship_texture_path;

extern int SETTING_camera_pan_speed;
extern float SETTING_camera_pan_smoothness;

extern bool SETTING_keyboard_player_movement;
extern int SETTING_primary_mouse_button;
extern int SETTING_secondary_mouse_button;

extern float SETTING_camera_zoom_max;
extern float SETTING_camera_zoom_min;

extern bool DEBUG_chunk_gridlines;
extern bool DEBUG_entity_outlines;

// Game
extern const int GAME_asteroid_count;
extern const int GAME_ship_count;
extern const Uint32 GAME_asteroid_color_raw;
extern const Uint32 GAME_blank_space_color;

// Asteroid Generation
extern const float ASTEROID_minimum_mass;
extern const float ASTEROID_maximum_mass;
extern const int ASTEROID_maximum_radius;

extern const int ASTEROID_startpos_variance;
extern const float ASTEROID_startspeed_maximum;

extern const float PLAYER_controlspeed_maximum;
extern const SDL_Color PLAYER_thrusting_outline_color;
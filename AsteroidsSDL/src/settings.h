#pragma once
#include <SDL.h>

#define PI 3.14159265358979323F

#define SETTING_ENTITY_DIMENSION 128
#define SETTING_MAX_POINT_COUNT 512

extern const char* RESOURCE_ship_texture_path;
extern const char* RESOURCE_laser_beam_texture_path;
extern const char* RESOURCE_highlighter_beam_texture_path;

extern int SETTING_camera_pan_speed;
extern float SETTING_camera_pan_smoothness;

extern bool SETTING_keyboard_player_movement;
extern int SETTING_primary_mouse_button;
extern int SETTING_secondary_mouse_button;

extern float SETTING_camera_zoom_max;
extern float SETTING_camera_zoom_min;

extern bool UI_FPSHUD_enabled;

extern const SDL_KeyCode KEY_pause;

extern const bool DEBUG_master;

extern const SDL_KeyCode KEY_DEBUG_mode;
extern bool DEBUG_mode;

extern const SDL_KeyCode KEY_DEBUG_focused_asteroid_increment;
extern const SDL_KeyCode KEY_DEBUG_focused_asteroid_decrement;
extern const SDL_KeyCode KEY_DEBUG_focused_asteroid_cancel;
extern int DEBUG_focused_asteroid;

extern const SDL_KeyCode KEY_DEBUG_wireframe_mode;
extern bool DEBUG_wireframe_mode;

extern const SDL_KeyCode KEY_DEBUG_chunk_gridlines;
extern bool DEBUG_chunk_gridlines;
extern const SDL_KeyCode KEY_DEBUG_ships_fire_at_will;
extern bool DEBUG_ships_fire_at_will;
extern const SDL_KeyCode KEY_DEBUG_display_chunk_numbers;
extern bool DEBUG_display_chunk_numbers;
extern const SDL_KeyCode KEY_DEBUG_display_entity_outlines;
extern bool DEBUG_display_entity_outlines;

extern bool DEBUG_entity_rotations;

// Controls
extern const SDL_KeyCode KEY_up;
extern const SDL_KeyCode KEY_up_alt;
extern const SDL_KeyCode KEY_down;
extern const SDL_KeyCode KEY_down_alt;
extern const SDL_KeyCode KEY_left;
extern const SDL_KeyCode KEY_left_alt;
extern const SDL_KeyCode KEY_right;
extern const SDL_KeyCode KEY_right_alt;

extern const SDL_KeyCode KEY_zoom_in;
extern const SDL_KeyCode KEY_zoom_in_alt;
extern const SDL_KeyCode KEY_zoom_out;
extern const SDL_KeyCode KEY_zoom_out_alt;

// Game
extern int GAME_asteroid_count;
extern const int GAME_asteroid_pool_size;
extern const int GAME_ship_count;
extern const Uint32 GAME_asteroid_color_raw;
extern const Uint32 GAME_blank_space_color;

// Asteroid Generation
extern const float ASTEROID_minimum_mass;
extern const float ASTEROID_maximum_mass;
extern const int ASTEROID_maximum_radius;

extern const int ASTEROID_startpos_variance;
extern const float ASTEROID_startspeed_maximum;
extern const float ASTEROID_split_minimum_velocity;

extern const float PLAYER_controlspeed_maximum;
extern const SDL_Color PLAYER_thrusting_outline_color;

extern const float SHIP_speed_maximum;
extern const float SHIP_attack_cooldown_time;
extern const float SHIP_attack_targetting_time;
extern const float SHIP_attack_time;
extern const float SHIP_max_attack_range;
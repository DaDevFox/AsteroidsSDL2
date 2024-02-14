#pragma once

#define SETTING_MAX_POLYGON_VERTICES 512

extern int SETTING_camera_pan_speed;
extern float SETTING_camera_pan_smoothness;

extern bool SETTING_keyboard_player_movement;
extern int SETTING_primary_mouse_button;
extern int SETTING_secondary_mouse_button;

extern float SETTING_camera_zoom_max;
extern float SETTING_camera_zoom_min;

// Game
extern const int GAME_asteroid_count;
extern const Uint32 GAME_asteroid_color_raw;
extern const Uint32 GAME_blank_space_color;

// Asteroid Generation
extern float ASTEROID_minimum_mass;
extern float ASTEROID_maximum_mass;
extern int ASTEROID_maximum_radius;

extern int ASTEROID_startpos_variance;
extern float ASTEROID_startspeed_maximum;

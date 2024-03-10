#include "settings.h"
#include <SDL.h>


#pragma region Interactive

const char* RESOURCE_ship_texture_path = "./triangle.png";

int SETTING_camera_pan_speed = 10;
float SETTING_camera_pan_smoothness = 0.995F;

bool SETTING_keyboard_player_movement = false;

int SETTING_primary_mouse_button = SDL_BUTTON_LEFT;
int SETTING_secondary_mouse_button = SDL_BUTTON_RIGHT;

float SETTING_camera_zoom_max = 2.0F;
float SETTING_camera_zoom_min = 0.2F;

bool SETTING_thrust_animation;

// Movement Controls
const SDL_KeyCode KEY_up = SDLK_UP;
const SDL_KeyCode KEY_up_alt = SDLK_w;
const SDL_KeyCode KEY_down = SDLK_DOWN;
const SDL_KeyCode KEY_down_alt = SDLK_s;
const SDL_KeyCode KEY_left = SDLK_LEFT;
const SDL_KeyCode KEY_left_alt = SDLK_a;
const SDL_KeyCode KEY_right = SDLK_RIGHT;
const SDL_KeyCode KEY_right_alt = SDLK_d;

// Camera Controls
const SDL_KeyCode KEY_zoom_in = SDLK_v;
const SDL_KeyCode KEY_zoom_in_alt = SDLK_PAGEUP;
const SDL_KeyCode KEY_zoom_out = SDLK_f;
const SDL_KeyCode KEY_zoom_out_alt = SDLK_PAGEDOWN;

bool DEBUG_chunk_gridlines = false;
bool DEBUG_highlight_capstone_asteroid = true;
bool DEBUG_chunk_numbers = false;
bool DEBUG_entity_outlines = true;
bool DEBUG_entity_rotations = false;

#pragma endregion

int GAME_asteroid_count = 30;
const int GAME_asteroid_pool_size = 100;
const int GAME_ship_count = 2;


const Uint32 GAME_asteroid_color_raw = 0xFFFFFFFF;
const Uint32 GAME_blank_space_color = 0x00000000;

// Asteroid Generation
const float ASTEROID_minimum_mass = 1.0F;
const float ASTEROID_maximum_mass = 5.0F;
const int ASTEROID_maximum_radius = 128 / 2;

const int ASTEROID_startpos_variance = 750;
const float ASTEROID_startspeed_maximum = 0.02F;

const float ASTEROID_split_minimum_velocity = 2.0F;

const float PLAYER_controlspeed_maximum = 0.05F;
const SDL_Color PLAYER_thrusting_outline_color = { 0, 0, 200, 255 };

const float SHIP_speed_maximum = 0.5F;




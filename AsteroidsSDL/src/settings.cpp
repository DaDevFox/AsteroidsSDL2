#include "settings.h"
#include <SDL.h>

#pragma region Interactive

const char* RESOURCE_ship_texture_path = "./triangle.png";
const char* RESOURCE_pip_texture_path = "./pip.png";
const char* RESOURCE_laser_beam_texture_path = "./laser_beam.png";
const char* RESOURCE_highlighter_beam_texture_path = "./highlighter_beam.png";

float SETTING_zoom_clamping = 0.1F;
float SETTING_scroll_sensitivity = 0.01F;
int SETTING_camera_pan_speed = 10;
float SETTING_camera_pan_smoothness = 0.995F;
float SETTING_camera_zoom_smoothness = 0.995F;

bool SETTING_keyboard_player_movement = false;

int SETTING_primary_mouse_button = SDL_BUTTON_LEFT;
int SETTING_secondary_mouse_button = SDL_BUTTON_RIGHT;

float SETTING_camera_zoom_max = 2.0F;
float SETTING_camera_zoom_min = 0.2F;

bool SETTING_thrust_animation = true;
bool SETTING_all_targetting_indicators = false;

const int SETTING_default_entity_drag = 0.0001F;

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

const SDL_KeyCode KEY_pause = SDLK_p;

// UI
bool UI_FPSHUD_enabled = DEBUG_master;


const bool DEBUG_master = true;

// DEBUG
const SDL_KeyCode KEY_DEBUG_mode = SDLK_u;
bool DEBUG_mode = false;

const SDL_KeyCode KEY_DEBUG_focused_asteroid_increment = SDLK_b;
const SDL_KeyCode KEY_DEBUG_focused_asteroid_decrement = SDLK_c;
const SDL_KeyCode KEY_DEBUG_focused_asteroid_cancel = SDLK_z;
#ifdef RELEASE
int CAMERA_focused_asteroid = 0;
#else
int CAMERA_focused_asteroid = -1;
#endif

const SDL_KeyCode KEY_DEBUG_wireframe_mode = SDLK_q;
bool DEBUG_wireframe_mode = false;

const SDL_KeyCode KEY_DEBUG_chunk_gridlines = SDLK_g;
bool DEBUG_chunk_gridlines = false;

const SDL_KeyCode KEY_DEBUG_ship_targets = SDLK_t;
bool DEBUG_ship_targets = false;

const SDL_KeyCode KEY_DEBUG_ships_fire_at_will = SDLK_x;
bool DEBUG_ships_fire_at_will = false;

const SDL_KeyCode KEY_DEBUG_display_chunk_numbers = SDLK_g;
bool DEBUG_display_chunk_numbers = false;

const SDL_KeyCode KEY_DEBUG_display_entity_outlines = SDLK_o;
bool DEBUG_display_entity_outlines = true;

bool DEBUG_entity_rotations = false;

#pragma endregion

int GAME_asteroid_count = 30;
const int GAME_asteroid_pool_size = 200;
const int GAME_ship_count = 5;


const Uint32 GAME_asteroid_color_raw = 0xFFFFFFFF;
const Uint32 GAME_blank_space_color = 0x00000000;

// Asteroid Generation
const float ASTEROID_minimum_mass = 1.0F;
const float ASTEROID_maximum_mass = 5.0F;
const int ASTEROID_maximum_radius = SETTING_ENTITY_DIMENSION / 2;

const int ASTEROID_startpos_variance = 750;
const float ASTEROID_startspeed_maximum = 0.02F;

const float ASTEROID_split_minimum_velocity = 0.020F;

const float PLAYER_controlspeed_maximum = 0.1F;
const SDL_Color PLAYER_thrusting_outline_color = { 0, 0, 255, 255 };

// Ships
const int SHIP_initial_health = 3;

const float SHIP_health_damaged_showtime = 2.0F;
const float SHIP_health_damaged_fadetime = 0.5F;
const int SHIP_health_damaged_pulses = 2;
const float SHIP_health_damaged_pulsetime = 0.25F;

const float SHIP_speed_maximum = 0.2F;

const float SHIP_warning_time = 10.0F;
const float SHIP_min_warning_focus_time = 0.0F; // min amount of time to dedicate to a new warning target (attention can't be stolen for __ seconds)

const float SHIP_attack_cooldown_time = 3.0F;
const float SHIP_attack_targetting_time = 0.5F;
const float SHIP_attack_time = 0.5F;
const float SHIP_max_attack_range = 100.0F;




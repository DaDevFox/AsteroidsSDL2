#include "settings.h"
#include <SDL.h>

int SETTING_camera_pan_speed = 10;
float SETTING_camera_pan_smoothness = 0.995F;

bool SETTING_keyboard_player_movement = false;
int SETTING_primary_mouse_button = SDL_BUTTON_LEFT;
int SETTING_secondary_mouse_button = SDL_BUTTON_RIGHT;

float SETTING_camera_zoom_max = 2.0F;
float SETTING_camera_zoom_min = 0.2F;
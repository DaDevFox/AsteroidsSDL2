#pragma once
#include "window/RenderWindow.h"
#include "settings.h"

#define INITIAL_WIDTH 680
#define INITIAL_HEIGHT 400

extern int PLAYER_initial_outline_point_count;
extern const int GAME_min_outline_point_count;
extern const int PLAYER_asteroid_id;
extern const int PLAYER_entity_id;

extern bool GAME_game_over;
extern bool INPUT_enabled;

extern int GAME_width;
extern int GAME_height;
extern SDL_Rect GAME_window_rect;

extern const int chunk_size;
extern const int GAME_chunkwise_height;
extern const int GAME_chunkwise_width;

extern int WINDOW_width;
extern int WINDOW_height;

extern TTF_Font* encode_sans_medium;
extern TTF_Font* encode_sans_bold;

extern SDL_AudioSpec blip_warn_wavSpec;
extern Uint32 blip_warn_wavLength;
extern Uint8* blip_warn_wavBuffer;

extern SDL_AudioSpec blip_attack_wavSpec;
extern Uint32 blip_attack_wavLength;
extern Uint8* blip_attack_wavBuffer;

extern SDL_AudioSpec hit_damage_wavSpec;
extern Uint32 hit_damage_wavLength;
extern Uint8* hit_damage_wavBuffer;

extern SDL_AudioSpec laser_shoot_wavSpec;
extern Uint32 laser_shoot_wavLength;
extern Uint8* laser_shoot_wavBuffer;

extern SDL_AudioSpec thrust_wavSpec;
extern Uint32 thrust_wavLength;
extern Uint8* thrust_wavBuffer;

extern int AUDIO_device_id;



extern void* entities;
extern RenderWindow window;
extern float unscaled_delta_time;
extern float delta_time;
extern float time_scaling;

extern bool game_running;

float player_health();

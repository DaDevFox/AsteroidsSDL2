#pragma once
#include "Entity.h"

/// <summary>
/// Initialize ships
/// </summary>
void ships_init();

/// <summary>
/// Update ships (movement + collisions)
/// </summary>
void ships_update(float delta_time);

void ships_render_update(RenderWindow* window);

/// <summary>
/// Cleans up ship textures and instantiated data
/// </summary>
void ships_cleanup();

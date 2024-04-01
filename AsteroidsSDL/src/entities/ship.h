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

int get_health(Entity* ship);

void ship_damage(Entity* ship, int amount);

void alert_ship_warning(Entity* ship, Entity* alertee);

void alert_ship_attack(Entity* ship, Entity* alertee);

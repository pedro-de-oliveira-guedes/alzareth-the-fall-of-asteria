#pragma once

#include "../Projectile.h"

class GhostAttack : public Projectile {
public:
    GhostAttack(Game* game, Vector2 position, Vector2 velocity, float damage);
};

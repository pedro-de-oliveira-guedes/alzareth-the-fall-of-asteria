#pragma once

#include "../Projectile.h"

class Fireball : public Projectile {
public:
    Fireball(Game* game, Vector2 position, Vector2 velocity, float damage);

    void OnUpdate(float deltaTime) override;
};

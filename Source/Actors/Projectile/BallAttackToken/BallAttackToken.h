#pragma once

#include "../Projectile.h"

class BallAttackToken : public Projectile {
public:
    BallAttackToken(Game* game, Vector2 position, Vector2 velocity, float damage);

    void OnUpdate(float deltaTime) override;
};
#pragma once

#include "../Actor.h"
#include "../../Utils/Math.h"

class AABBColliderComponent;
class DrawAnimatedComponent;

class Projectile : public Actor {
public:
    Projectile(Game* game, Vector2 position, Vector2 velocity, float damage);

    void OnUpdate(float deltaTime) override;
    void OnCollision(float minOverlap, AABBColliderComponent* other) override;

    float GetDamage() const { return mDamage; }

protected:
    Vector2 mVelocity;
    float mDamage;
    float mLifetime;

    AABBColliderComponent* mColliderComponent;
    DrawAnimatedComponent* mDrawComponent;
};

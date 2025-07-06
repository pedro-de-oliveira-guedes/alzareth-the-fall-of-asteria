#include "Projectile.h"
#include "../../Game.h"

Projectile::Projectile(Game* game, Vector2 position, Vector2 velocity, float damage)
    : Actor(game), mVelocity(velocity), mDamage(damage), mLifetime(5.0f) {
    SetPosition(position);
}

void Projectile::OnUpdate(float deltaTime) {}

void Projectile::OnCollision(float minOverlap, AABBColliderComponent* other) {
}

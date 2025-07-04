#include "Projectile.h"
#include "../../Game.h"

Projectile::Projectile(Game* game, Vector2 position, Vector2 velocity, float damage)
    : Actor(game), mVelocity(velocity), mDamage(damage), mLifetime(5.0f) {
    SetPosition(position);
}

void Projectile::OnUpdate(float deltaTime) {
    Vector2 newPos = GetPosition() + mVelocity * deltaTime;
    SetPosition(newPos);

    if ((mGame->GetPlayer()->GetPosition() - GetPosition()).Length() < 20.0f) {
        auto player = mGame->GetPlayer();
        if (player) {
            player->TakeDamage(mDamage);
            mGame->RemoveActor(this);
        }
    }

    mLifetime -= deltaTime;
    if (mLifetime <= 0.0f) {
        mGame->RemoveActor(this);
    }
}

void Projectile::OnCollision(float minOverlap, AABBColliderComponent* other) {
}

#include "Fireball.h"
#include "../../../Game.h"
#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../Components/ColliderComponents/AABBColliderComponent.h"

Fireball::Fireball(Game* game, Vector2 position, Vector2 velocity, float damage)
    : Projectile(game, position, velocity, damage) {

    mColliderComponent = new AABBColliderComponent(
        this,
        0, 0, 10, 10, // Size of the fireball collider
        ColliderLayer::EnemyProjectile,
        false,
        true
    );

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Fireball/fireball.png",
        "../Assets/Sprites/Fireball/fireball.json",
        100
    );

    mDrawComponent->AddAnimation("default", {0});
    mDrawComponent->SetAnimation("default");
}

#include "BallAttackToken.h"
#include "../../../Game.h"
#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../Components/ColliderComponents/AABBColliderComponent.h"

BallAttackToken::BallAttackToken(Game* game, Vector2 position, Vector2 velocity, float damage)
    : Projectile(game, position, velocity, damage) {

    mColliderComponent = new AABBColliderComponent(
        this,
        0, 0, 10, 10, 
        ColliderLayer::PlayerProjectile, 
        false,
        true
    );

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/BallAtackToken/Ball.png", 
        "../Assets/Sprites/BallAtackToken/atack_ball.json", 
        100
    );

    mDrawComponent->AddAnimation("default", {0});
    mDrawComponent->SetAnimation("default");
}
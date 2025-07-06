#include "BallAttackToken.h"
#include "../../../Game.h"
#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../Components/ColliderComponents/AABBColliderComponent.h"

BallAttackToken::BallAttackToken(Game* game, Vector2 position, Vector2 velocity, float damage)
    : Projectile(game, position, velocity, damage) {

    mColliderComponent = new AABBColliderComponent(
        this,
        0, 0, 20, 20, 
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

void BallAttackToken::OnUpdate(float deltaTime) {
    Vector2 newPos = GetPosition() + mVelocity * deltaTime;
    SetPosition(newPos);

    mLifetime -= deltaTime;
    if (mLifetime <= 0.0f) {
        mGame->RemoveActor(this);
        return;
    }
}
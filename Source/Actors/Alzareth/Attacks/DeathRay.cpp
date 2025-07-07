#include "DeathRay.h"

#include "../../../Game.h"
#include "../../../Systems/SceneManager/SceneManagerSystem.h"

DeathRay::DeathRay(
    Game *game,
    const Vector2 start_position,
    const Vector2 end_position,
    const float timeUntilActive
) : Actor(game) {
    mEndPosition = end_position;
    mTimeUntilActive = timeUntilActive;
    std::vector<int> sprites;

    const float xOffset = std::abs(end_position.x - start_position.x);
    const float yOffset = std::abs(end_position.y - start_position.y);
    if (xOffset > yOffset) {
        mVelocity = Vector2(1.0f, 0.f) * mSpeed;
        mSize = Vector2(BASE_WIDTH, mGame->GetSceneManager()->GetLevelSize().second * SceneManagerSystem::TILE_SIZE);
        sprites = { 2, 3 };
    } else {
        mVelocity = Vector2(0.f, 1.f) * mSpeed;
        mSize = Vector2(mGame->GetSceneManager()->GetLevelSize().first * SceneManagerSystem::TILE_SIZE, BASE_WIDTH);
        sprites = { 0, 1 };
    }

    mRigidBodyComponent = new RigidBodyComponent(this);
    mRigidBodyComponent->SetEnabled(false);

    mColliderComponent = new AABBColliderComponent(
        this,
        0.f,
        0.f,
        mSize.x,
        mSize.y,
        ColliderLayer::EnemyProjectile,
        false,
        true
    );
    mColliderComponent->SetEnabled(false);

    mDrawAnimatedComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Alzareth/DeathRay/Alzareth_Death_Ray.png",
        "../Assets/Sprites/Alzareth/DeathRay/Alzareth_Death_Ray.json",
        mRotation
    );
    mDrawAnimatedComponent->AddAnimation("default", sprites);
    mDrawAnimatedComponent->SetAnimation("default");
    mDrawAnimatedComponent->SetAnimFPS(10.f);
    mDrawAnimatedComponent->SetEnabled(false);

    SetPosition(start_position);
}

DeathRay::~DeathRay() {}

void DeathRay::OnUpdate(const float deltaTime) {
    if (mTimeUntilActive > 0.0f) {
        mTimeUntilActive -= deltaTime;
        return;
    }

    if (mIsFinished) {
        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);
        mDrawAnimatedComponent->SetEnabled(false);
        return;
    }

    mRigidBodyComponent->SetEnabled(true);
    mColliderComponent->SetEnabled(true);
    mDrawAnimatedComponent->SetEnabled(true);

    mRigidBodyComponent->SetVelocity(mVelocity);
    mDamageCooldown -= deltaTime;
    mDamageCooldown = std::max(mDamageCooldown, 0.0f);

    if (
        std::abs(GetPosition().x - mEndPosition.x) <= 2*(deltaTime * mSpeed) &&
        std::abs(GetPosition().y - mEndPosition.y) <= 2*(deltaTime * mSpeed)
    ) {
        mIsFinished = true;
    }
}

void DeathRay::OnCollision(float minOverlap, AABBColliderComponent *other) {
    if (mIsFinished) return;

    if (other->GetLayer() == ColliderLayer::Player) {
        if (mDamageCooldown <= 0.0f) {
            auto player = mGame->GetPlayer();
            if (player) {
                player->TakeDamage(mDamage);
                mDamageCooldown = DAMAGE_COOLDOWN;
            }
        }
    }
}

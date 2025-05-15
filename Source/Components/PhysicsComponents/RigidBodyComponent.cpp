#include <SDL.h>
#include "../Actors/Actor.h"
#include "../Game.h"
#include "RigidBodyComponent.h"
#include "ColliderComponents/AABBColliderComponent.h"

RigidBodyComponent::RigidBodyComponent(
    Actor *owner,
    const float mass,
    const float friction,
    const bool applyGravity,
    const int updateOrder
) : Component(owner, updateOrder) {
    mMass = mass;
    mApplyGravity = applyGravity;
    mFrictionCoefficient = friction;
    mVelocity = Vector2::Zero;
    mAcceleration = Vector2::Zero;
}

void RigidBodyComponent::ApplyForce(const Vector2 &force) {
    mAcceleration += force * (1.f/mMass);
}

void RigidBodyComponent::Update(float deltaTime) {
    // Apply gravity acceleration
    if(mApplyGravity) {
        ApplyForce(Vector2::UnitY * GRAVITY);
    }

    // Apply friction
    if(Math::Abs(mVelocity.x) > 0.05f) {
        ApplyForce(Vector2::UnitX * -mFrictionCoefficient * mVelocity.x);
    }

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    mVelocity.x = Math::Clamp<float>(mVelocity.x, -MAX_SPEED_X, MAX_SPEED_X);
    mVelocity.y = Math::Clamp<float>(mVelocity.y, -MAX_SPEED_Y, MAX_SPEED_Y);

    if(Math::NearZero(mVelocity.x, 1.0f)) {
        mVelocity.x = 0.f;
    }

    const auto collider = mOwner->GetComponent<AABBColliderComponent>();

    mOwner->SetPosition(Vector2(mOwner->GetPosition().x + mVelocity.x * deltaTime, mOwner->GetPosition().y));
    if (collider->IsEnabled()) {
        collider->DetectHorizontalCollision(this);
    }

    mOwner->SetPosition(Vector2(mOwner->GetPosition().x, mOwner->GetPosition().y + mVelocity.y * deltaTime));
    if (collider->IsEnabled()) {
        collider->DetectVerticalCollision(this);
    }

    mAcceleration.Set(0.f, 0.f);
}

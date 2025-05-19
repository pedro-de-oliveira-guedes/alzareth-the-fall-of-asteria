#include "RigidBodyComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../ColliderComponents/AABBColliderComponent.h"

constexpr float MAX_SPEED_X = 2500.0f;
constexpr float MAX_SPEED_Y = 2500.0f;

RigidBodyComponent::RigidBodyComponent(
    Actor *owner,
    const float mass,
    const float friction,
    const int updateOrder
) : Component(owner, updateOrder) {
    mMass = mass;
    mFrictionCoefficient = friction;
    mVelocity = Vector2::Zero;
    mAcceleration = Vector2::Zero;
}

void RigidBodyComponent::ApplyForce(const Vector2 &force) {
    mAcceleration += force * (1.f/mMass);
}

void RigidBodyComponent::Update(float deltaTime) {
    // Apply friction
    if(Math::Abs(mVelocity.x) > 0.05f) {
        ApplyForce(Vector2::UnitX * -mFrictionCoefficient * mVelocity.x);
    }
    if (Math::Abs(mVelocity.y) > 0.05f) {
        ApplyForce(Vector2::UnitY * -mFrictionCoefficient * mVelocity.y);
    }

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    mVelocity.x = Math::Clamp<float>(mVelocity.x, -MAX_SPEED_X, MAX_SPEED_X);
    mVelocity.y = Math::Clamp<float>(mVelocity.y, -MAX_SPEED_Y, MAX_SPEED_Y);

    if (Math::NearZero(mVelocity.x, 1.0f)) {
        mVelocity.x = 0.f;
    }
    if (Math::NearZero(mVelocity.y, 1.0f)) {
        mVelocity.y = 0.f;
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

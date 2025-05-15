#include "CircleColliderComponent.h"
#include "../../Actors/Actor.h"

CircleColliderComponent::CircleColliderComponent(
    Actor *owner,
    const float radius,
    const int updateOrder
) : Component(owner, updateOrder) {
    mRadius = radius;
}

const Vector2& CircleColliderComponent::GetCenter() const {
    return mOwner->GetPosition();
}

float CircleColliderComponent::GetRadius() const {
    return mOwner->GetScale() * mRadius;
}

bool CircleColliderComponent::Intersect(const CircleColliderComponent &c) const {
    // Calculate amount squared
    Vector2 diff = GetCenter() - c.GetCenter();
    float distSq = diff.LengthSq();

    // Calculate sum of radii squared
    float radiiSq = GetRadius() + c.GetRadius();
    radiiSq *= radiiSq;

    return distSq <= radiiSq;
}
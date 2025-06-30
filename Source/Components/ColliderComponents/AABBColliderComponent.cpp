#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(
    Actor *owner,
    const int dx,
    const int dy,
    const int w,
    const int h,
    const ColliderLayer layer,
    const bool isStatic,
    const bool isTrigger,
    const int updateOrder
) : Component(owner, updateOrder) {
    mOffset = Vector2(static_cast<float>(dx), static_cast<float>(dy));
    mIsStatic = isStatic;
    mWidth = w;
    mHeight = h;
    mLayer = layer;
    mIsTrigger = isTrigger;
}

AABBColliderComponent::~AABBColliderComponent() {
}

Vector2 AABBColliderComponent::GetMin() const {
    return mOwner->GetPosition() + mOffset;
}

Vector2 AABBColliderComponent::GetMax() const {
    return GetMin() + Vector2(mWidth, mHeight);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent &b) const {
    if (GetMax().x <= b.GetMin().x) return false;
    if (GetMin().x >= b.GetMax().x) return false;
    if (GetMax().y <= b.GetMin().y) return false;
    if (GetMin().y >= b.GetMax().y) return false;

    return true;
}

float AABBColliderComponent::GetMinVerticalOverlap(const AABBColliderComponent *b) const {
    const float topIntersection = GetMax().y - b->GetMin().y;
    const float bottomIntersection = GetMin().y - b->GetMax().y;

    return std::abs(topIntersection) < std::abs(bottomIntersection) ? topIntersection : bottomIntersection;
}

float AABBColliderComponent::GetMinHorizontalOverlap(const AABBColliderComponent *b) const {
    const float leftIntersection = GetMax().x - b->GetMin().x;
    const float rightIntersection = GetMin().x - b->GetMax().x;

    return std::abs(leftIntersection) < std::abs(rightIntersection) ? leftIntersection : rightIntersection;
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody) const {
    if (mIsStatic) return false;

    const auto colliders = mOwner->GetGame()->GetNearbyColliders(mOwner->GetPosition());

    for (const auto &collider : colliders) {
        if (!collider->IsEnabled() || collider == this || !Intersect(*collider)) continue;

        if(mIsTrigger || collider->mIsTrigger){
            mOwner->OnCollision(0.0f, collider);
            return 0.0f;
        }

        if (GetMax().x > collider->GetMin().x && GetMin().x < collider->GetMax().x) {
            const float minHorizontalOverlap = GetMinHorizontalOverlap(collider);

            ResolveHorizontalCollisions(rigidBody, minHorizontalOverlap);
            mOwner->OnCollision(minHorizontalOverlap, collider);

            return minHorizontalOverlap;
        }
    }

    return 0.0f;
}

float AABBColliderComponent::DetectVerticalCollision(RigidBodyComponent *rigidBody) const {
    if (mIsStatic) return false;

    const auto colliders = mOwner->GetGame()->GetNearbyColliders(mOwner->GetPosition());

    for (const auto &collider : colliders) {
        if (!collider->IsEnabled() || collider == this || !Intersect(*collider)) continue;

        if(mIsTrigger || collider->mIsTrigger){
            mOwner->OnCollision(0.0f, collider);
            return 0.0f;
        }

        if (GetMax().y > collider->GetMin().y || GetMin().y < collider->GetMax().y) {
            const float minVerticalOverlap = GetMinVerticalOverlap(collider);

            ResolveVerticalCollisions(rigidBody, minVerticalOverlap);
            mOwner->OnCollision(minVerticalOverlap, collider);

            return minVerticalOverlap;
        }
    }

    return 0.0f;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap) const {
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(minXOverlap, 0));
    rigidBody->SetVelocity(Vector2(0, rigidBody->GetVelocity().y));
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap) const {
    mOwner->SetPosition(mOwner->GetPosition() - Vector2(0, minYOverlap));
    rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0));
}

#pragma once
#include "../Component.h"
#include "../../Utils/Math.h"
#include "../PhysicsComponents/RigidBodyComponent.h"

enum class ColliderLayer {
    Player,
    Enemy,
    Blocks,
    MeleeWeapon,
    Bullet
};

class AABBColliderComponent : public Component {
    public:

        AABBColliderComponent(
            class Actor *owner,
            int dx,
            int dy,
            int w,
            int h,
            ColliderLayer layer,
            bool isStatic = false,
            int updateOrder = 10
        );
        ~AABBColliderComponent() override;

        bool Intersect(const AABBColliderComponent &b) const;

        float DetectHorizontalCollision(RigidBodyComponent *rigidBody) const;
        float DetectVerticalCollision(RigidBodyComponent *rigidBody) const;

        Vector2 GetMin() const;
        Vector2 GetMax() const;
        ColliderLayer GetLayer() const { return mLayer; }

    private:
        float GetMinVerticalOverlap(const AABBColliderComponent *b) const;
        float GetMinHorizontalOverlap(const AABBColliderComponent *b) const;

        void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, float minXOverlap) const;
        void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, float minYOverlap) const;

        Vector2 mOffset;
        int mWidth;
        int mHeight;
        bool mIsStatic;

        ColliderLayer mLayer;
};
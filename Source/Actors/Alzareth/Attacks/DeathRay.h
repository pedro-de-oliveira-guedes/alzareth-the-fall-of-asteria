#pragma once

#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Actor.h"

class DeathRay : public Actor {
    public:
        DeathRay(Game *game, Vector2 start_position, Vector2 end_position, float timeUntilActive = 1.0f);
        ~DeathRay();

        bool IsFinished() const { return mIsFinished; }

        void OnUpdate(float deltaTime) override;
        void OnCollision(float minOverlap, AABBColliderComponent *other) override;

    private:
        DrawAnimatedComponent *mDrawAnimatedComponent;
        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;

        Vector2 mEndPosition;
        bool mIsFinished = false;
        float mTimeUntilActive;

        Vector2 mVelocity;
        float mSpeed = 100.0f;

        static constexpr float BASE_WIDTH = 100.0f;
        Vector2 mSize;
        float mRotation;

        float mDamage = 20.0f;
        static constexpr float DAMAGE_COOLDOWN = 0.5f;
        float mDamageCooldown = 0.0f;
};

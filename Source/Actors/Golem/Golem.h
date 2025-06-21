#pragma once
#include "../Actor.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

class Golem : public Actor {
    public:
        explicit Golem(
            Game *game,
            float walkSpeed = 500.0f
        );

        float GetMaxHealth() const { return mMaxHealth; }
        void SetMaxHealth(const float maxHealth) { mMaxHealth = maxHealth; }
        float GetCurrentHealth() const { return mCurrentHealth; }
        void SetCurrentHealth(const float currentHealth) { mCurrentHealth = currentHealth; }

        void OnUpdate(float deltaTime) override;

    private:
        // Input handling
        void HandleRotation();
        void ApplyBasicMovement(Vector2 force_vector);

        // Update handling

        void ManageAnimations() const;

        float mMaxHealth;
        float mCurrentHealth;

        float mWalkSpeed;

        bool mIsWalking;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
};

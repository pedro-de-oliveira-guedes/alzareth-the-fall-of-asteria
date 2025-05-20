#pragma once
#include "../Actor.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

class Player : public Actor {
    public:
        explicit Player(
            Game *game,
            float walkSpeed = 1000.0f,
            float runSpeed = 2200.0f,
            float dashSpeed = 3000.0f
        );

        float GetMaxHealth() const { return mMaxHealth; }
        void SetMaxHealth(const float maxHealth) { mMaxHealth = maxHealth; }
        float GetCurrentHealth() const { return mCurrentHealth; }
        void SetCurrentHealth(const float currentHealth) { mCurrentHealth = currentHealth; }

        float GetMaxEnergy() const { return mMaxEnergy; }
        void SetMaxEnergy(const float maxEnergy) { mMaxEnergy = maxEnergy; }
        float GetCurrentEnergy() const { return mCurrentEnergy; }
        void SetCurrentEnergy(const float currentEnergy) { mCurrentEnergy = currentEnergy; }

        void OnProcessInput(const Uint8 *keyState) override;
        void OnUpdate(float deltaTime) override;

    private:
        void ManageAnimations() const;

        float mMaxHealth;
        float mCurrentHealth;
        float mMaxEnergy;
        float mCurrentEnergy;

        float mWalkSpeed;
        float mRunSpeed;
        float mDashSpeed;
        float mDashTime;

        bool mIsWalking;
        bool mIsRunning;
        bool mIsDashing;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
};

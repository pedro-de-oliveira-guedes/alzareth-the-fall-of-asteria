#pragma once
#include <array>

#include "../Actor.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/UI/HUDComponent.h"
#include "../../Inventory/Inventory.h"
#include "../Items/Weapons/Melee/Sword.h"
#include "../../Systems/Audio/AudioSystem.h"

class Player : public Actor {
    public:
        explicit Player(
            Game *game,
            float walkSpeed = 1000.0f,
            float runSpeed = 2200.0f,
            float dashSpeed = 3000.0f
        );

        HUDComponent* GetHUDComponent() const { return mHUDComponent; }

        float GetMaxHealth() const { return mMaxHealth; }
        void SetMaxHealth(const float maxHealth) { mMaxHealth = maxHealth; }
        float GetCurrentHealth() const { return mCurrentHealth; }
        void SetCurrentHealth(const float currentHealth) { mCurrentHealth = Math::Min(currentHealth, mMaxHealth); }

        float GetMaxEnergy() const { return mMaxEnergy; }
        void SetMaxEnergy(const float maxEnergy) { mMaxEnergy = maxEnergy; }
        float GetCurrentEnergy() const { return mCurrentEnergy; }
        void SetCurrentEnergy(const float currentEnergy) { mCurrentEnergy = Math::Min(currentEnergy, mMaxEnergy); }

        bool IsInvulnerable() const { return mIsInvulnerable; }
        void SetInvulnerable(bool invulnerable) { mIsInvulnerable = invulnerable; }
        void SetInvulnerabilityTimer(float time) { mInvulnerabilityTime = time; }

        void OnProcessInput(const Uint8 *keyState) override;
        void OnUpdate(float deltaTime) override;

        float GetWalkSpeed() const { return mWalkSpeed; }

        const Inventory& GetInventory() const { return mInventory; }

        void OnCollision(float minOverlap, AABBColliderComponent *other) override;
        void Kill() override;

        void TakeDamage(float damage);

        void Attack(const Uint8 *keyState, Uint32 mouseButtonState);

        void UseItemAtIndex(int index);

        bool GetIsDashing() const { return mIsDashing; }

    private:
        // Input handling
        void HandleRotation();
        Vector2 HandleBasicMovementInput(const Uint8 *keyState);
        void ApplyBasicMovement(Vector2 force_vector);
        void HandleDash(const Uint8 *keyState, Vector2 force_vector);
        void HandleItemInput(const Uint8 *keyState);
        void HandleUseItem(const Uint8 *keyState);
        void HandleStatusEffects(float deltaTime);

        // Update handling
        void HandleMapBoundaries();
        void HandleEnergyAndCooldowns(float deltaTime);

        void ManageAnimations();

        void AttackToken();

        float mMaxHealth;
        float mCurrentHealth;
        float mMaxEnergy;
        float mCurrentEnergy;

        float mWalkSpeed;
        float mRunSpeed;
        float mDashSpeed;

        float mDashTime;
        float mDashCooldown;
        float mEnergyRechargeCooldown;

        bool mIsWalking;
        bool mIsRunning;
        bool mIsDashing;
        bool mEPressedLastFrame;
        SoundHandle mWalkSound;
        SoundHandle mSwordSound;
        SoundHandle mDamageSound;
        SoundHandle mItemPickupSound;

        std::array<bool, 5> mNumberKeysPressedLastFrame;

        Inventory mInventory;

        Uint32 mPreviousMouseButtonState;

        bool mIsInvulnerable;
        float mInvulnerabilityTime;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
        HUDComponent *mHUDComponent;
};

#pragma once
#include "../Actor.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/UI/HUDComponent.h"
#include "../../Inventory/Inventory.h"

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

        void AddItemToInventory(std::unique_ptr<Item> item);
        bool RemoveItemFromInventory(const std::string& itemName, int quantity = 1);
        const Inventory& GetInventory() const { return mInventory; }

    private:
        // Input handling
        void HandleRotation();
        Vector2 HandleBasicMovementInput(const Uint8 *keyState);
        void ApplyBasicMovement(Vector2 force_vector);
        void HandleDash(const Uint8 *keyState, Vector2 force_vector);

        // Update handling
        void HandleMapBoundaries();
        void HandleEnergyAndCooldowns(float deltaTime);

        void ManageAnimations() const;

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

        Inventory mInventory;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
        HUDComponent *mHUDComponent;
};

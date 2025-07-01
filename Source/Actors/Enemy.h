#pragma once
#include "./Actor.h"
#include "./../Components/DrawComponents/DrawAnimatedComponent.h"

class Enemy : public Actor {
    public:
        explicit Enemy(
            Game *game
        );

        float GetMaxHealth() const { return mMaxHealth; }
        void SetMaxHealth(const float maxHealth) { mMaxHealth = maxHealth; }
        float GetCurrentHealth() const { return mCurrentHealth; }
        void SetCurrentHealth(const float currentHealth) { mCurrentHealth = currentHealth; }

        virtual void OnUpdate(float deltaTime);
        void OnCollision(float minOverlap, AABBColliderComponent *other) override;
        void Kill() override;
        bool IsAlive() const { return !mIsDead; }

        float GetDamageAttack() const { return mDamageAttack; }

        void TakeDamage(float damage);

    protected:

        virtual void ManageAnimations() const;

        float mMaxHealth = 0.0f;
        float mCurrentHealth = 0.0f;

        float mWalkSpeed;
        float mDamageAttack;

        float mAttackCooldown;

        bool mIsAttacking = false;
        bool mIsWalking;
        bool mIsDead;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
};

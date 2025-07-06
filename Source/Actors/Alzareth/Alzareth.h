#pragma once

#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Actor.h"
#include "../Enemies/Enemy.h"
#include "Attacks/DeathRay.h"

class Alzareth : public Actor {
    public:
        enum class BossStage {
            SPAWNING,
            ENEMIES_SUMMONING,
            DEATH_RAYS_SUMMONING,
            ENEMIES_AND_DEATH_RAYS_SUMMONING,
        };

        explicit Alzareth(Game *game);
        ~Alzareth() override;

        void OnUpdate(float deltaTime) override;
        void TakeDamage(float damage);

    private:
        float mMaxHealth;
        float mCurrentHealth;

        BossStage mCurrentStage;
        void NextStage();
        void BuildStage();
        bool CurrentStageCleared() const;

        bool mFirstStageBuilt;
        void BuildFirstStage();
        static constexpr int ENEMIES_IN_FIRST_STAGE = 15;
        std::vector<Enemy*> mSummonedEnemies;

        bool mSecondStageBuilt;
        void BuildSecondStage();
        std::vector<DeathRay*> mDeathRays;


        bool mFinalStageBuilt;
        void BuildFinalStage();

        bool mIsIdle;

        static constexpr float FALL_ANIMATION_DURATION = 0.7f;
        float mFallAnimationTimer;
        bool mIsFalling;

        static constexpr float RISE_ANIMATION_DURATION = 0.7f;
        float mRiseAnimationTimer;
        bool mIsRising;

        static constexpr float VULNERABILITY_DURATION = 10.0f;
        float mVulnerabilityTimer;
        bool mIsVulnerable;

        static constexpr float CASTING_DURATION = 1.0f;
        float mCastingTimer;
        bool mIsCasting;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mAABBColliderComponent;

        static constexpr std::string IDLE_ANIMATION = "idle";
        static constexpr std::string FALLING_ANIMATION = "falling";
        static constexpr std::string RISING_ANIMATION = "rising";
        static constexpr std::string VULNERABLE_ANIMATION = "vulnerable";
        static constexpr std::string CASTING_ANIMATION = "casting";
        DrawAnimatedComponent *mDrawComponentAlzareth;
        void HandleAlzarethState(float deltaTime);

        static constexpr float BUILDING_SHIELD_DURATION = 0.7f;
        float mShieldBuildingTimer;
        bool mIsShieldBuilding;

        bool mIsShieldActive;

        static constexpr float DESTROYING_SHIELD_DURATION = 0.7f;
        float mShieldDestructionTimer;
        bool mIsShieldDestroying;

        static constexpr std::string SHIELD_BUILDING_ANIMATION = "shield_building";
        static constexpr std::string SHIELD_DESTRUCTION_ANIMATION = "shield_destruction";
        static constexpr std::string SHIELD_ACTIVE_ANIMATION = "shield_active";
        DrawAnimatedComponent *mDrawComponentShield;
        void HandleShieldState(float deltaTime);
};

#include "Alzareth.h"

#include "../../Game.h"
#include "../../Systems/SceneManager/SceneManagerSystem.h"
#include "../../Utils/Random.h"
#include "../Enemies/Golem/Golem.h"
#include "../Enemies/Skeleton/Skeleton.h"

Alzareth::Alzareth(Game *game) : Actor(game) {
    mMaxHealth = 1000.0f;
    mCurrentHealth = mMaxHealth;
    mCurrentStage = BossStage::SPAWNING;

    mFirstStageBuilt = false;
    mSecondStageBuilt = false;
    mFinalStageBuilt = false;

    mIsIdle = false;

    mIsFalling = false;
    mFallAnimationTimer = FALL_ANIMATION_DURATION;

    mIsRising = true;
    mRiseAnimationTimer = RISE_ANIMATION_DURATION;

    mIsVulnerable = false;
    mVulnerabilityTimer = VULNERABILITY_DURATION;

    mIsCasting = false;
    mCastingTimer = CASTING_DURATION;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mAABBColliderComponent = new AABBColliderComponent(
        this,
        0,
        0,
        Game::SPRITE_SIZE * 2,
        Game::SPRITE_SIZE * 2,
        ColliderLayer::Enemy
    );

    mDrawComponentAlzareth = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Alzareth/Wizard/alzareth.png",
        "../Assets/Sprites/Alzareth/Wizard/alzareth.json",
        100
    );
    mDrawComponentAlzareth->AddAnimation(IDLE_ANIMATION, { 0, 1 });
    mDrawComponentAlzareth->AddAnimation(CASTING_ANIMATION, { 2, 3, 4, 5 });
    mDrawComponentAlzareth->AddAnimation(FALLING_ANIMATION, { 6, 7, 8 });
    mDrawComponentAlzareth->AddAnimation(VULNERABLE_ANIMATION, { 9, 10 });
    mDrawComponentAlzareth->AddAnimation(RISING_ANIMATION, { 13, 12, 11 });
    mDrawComponentAlzareth->SetAnimFPS(10.f);

    mIsShieldBuilding = false;
    mShieldBuildingTimer = BUILDING_SHIELD_DURATION;

    mIsShieldActive = false;

    mIsShieldDestroying = false;
    mShieldDestructionTimer = DESTROYING_SHIELD_DURATION;

    mDrawComponentShield = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Alzareth/Shield/Alzareth_shield.png",
        "../Assets/Sprites/Alzareth/Shield/Alzareth_shield.json",
        101
    );
    mDrawComponentShield->AddAnimation(SHIELD_BUILDING_ANIMATION, { 0, 1, 2 });
    mDrawComponentShield->AddAnimation(SHIELD_DESTRUCTION_ANIMATION, { 3, 4, 5 });
    mDrawComponentShield->AddAnimation(SHIELD_ACTIVE_ANIMATION, { 6, 7, 8, 9 });
    mDrawComponentShield->SetAnimFPS(10.f);
    mDrawComponentShield->SetIsPaused(true);
}

Alzareth::~Alzareth() {
    delete mDrawComponentAlzareth;
    delete mDrawComponentShield;
    delete mRigidBodyComponent;
    delete mAABBColliderComponent;

    for (const auto enemy : mSummonedEnemies) {
        delete enemy;
    }
    mSummonedEnemies.clear();

    for (const auto deathRay : mDeathRays) {
        delete deathRay;
    }
    mDeathRays.clear();
}

void Alzareth::NextStage() {
    if (!CurrentStageCleared()) return;

    if (mCurrentStage == BossStage::SPAWNING)
        mCurrentStage = BossStage::ENEMIES_SUMMONING;
    else if (mCurrentStage == BossStage::ENEMIES_SUMMONING && mCurrentHealth <= 2/3 * mMaxHealth)
        mCurrentStage = BossStage::DEATH_RAYS_SUMMONING;
    else if (mCurrentStage == BossStage::DEATH_RAYS_SUMMONING && mCurrentHealth <= 1/3 * mMaxHealth)
        mCurrentStage = BossStage::ENEMIES_AND_DEATH_RAYS_SUMMONING;
}

void Alzareth::BuildFirstStage() {
    for (int i = 0; i < ENEMIES_IN_FIRST_STAGE; i++) {
        const float levelWidth = mGame->GetSceneManager()->GetLevelSize().first * SceneManagerSystem::TILE_SIZE;
        const float levelHeight = mGame->GetSceneManager()->GetLevelSize().second * SceneManagerSystem::TILE_SIZE;

        const int x = Random::GetIntRange(100, levelWidth - 100);
        const int y = Random::GetIntRange(50, levelHeight - 50);
        if (Random::GetIntRange(0, 100) < 60) {
            mSummonedEnemies.push_back(new Golem(mGame, Vector2(x, y)));
        }
        else {
            mSummonedEnemies.push_back(new Skeleton(mGame, Vector2(x, y)));
        }
    }
}

void Alzareth::BuildSecondStage() {
    const int sceneWidth = mGame->GetSceneManager()->GetLevelSize().first * SceneManagerSystem::TILE_SIZE;
    const int sceneHeight = mGame->GetSceneManager()->GetLevelSize().second * SceneManagerSystem::TILE_SIZE;

    // Left and right death rays approaching to the center
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(50.f, 0.f),
        Vector2(sceneWidth / 2.f - 75.f, 0)
    ));
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(sceneWidth - 50.f, 0.f),
        Vector2(sceneWidth / 2.f + 75.f, 0)
    ));

    // Top and bottom death rays approaching to the center
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(0.f, 50.f),
        Vector2(0.f, sceneHeight / 2.f - 75.f),
        5.0f
    ));
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(0.f, sceneHeight - 50.f),
        Vector2(0.f, sceneHeight / 2.f + 75.f),
        5.0f
    ));

    // Left death ray sweeping the screen
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(50.f, 0.f),
        Vector2(sceneWidth / 2.f - 75.f, 0.f),
        10.0f
    ));

    // Top death ray sweeping the screen
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(0.f, 50.f),
        Vector2(0.f, sceneHeight / 2.f - 75.f),
        15.0f
    ));
}

void Alzareth::BuildFinalStage() {
    BuildFirstStage();
    BuildSecondStage();
}

bool Alzareth::CurrentStageCleared() const {
    if (mCurrentStage == BossStage::ENEMIES_SUMMONING) {
        for (const auto& enemy : mSummonedEnemies) {
            if (enemy->GetCurrentHealth() > 0.0f) {
                return false;
            }
        }
        return true;
    }

    if (mCurrentStage == BossStage::DEATH_RAYS_SUMMONING) {
        for (const auto& deathRay : mDeathRays) {
            if (!deathRay->IsFinished()) {
                return false;
            }
        }
        return true;
    }

    if (mCurrentStage == BossStage::ENEMIES_AND_DEATH_RAYS_SUMMONING) {
        for (const auto& enemy : mSummonedEnemies) {
            if (enemy->GetCurrentHealth() > 0.0f) {
                return false;
            }
        }
        for (const auto& deathRay : mDeathRays) {
            if (!deathRay->IsFinished()) {
                return false;
            }
        }
        return true;
    }

    return false;
}

void Alzareth::BuildStage() {
    if (!mFirstStageBuilt && mCurrentStage == BossStage::ENEMIES_SUMMONING) {
        BuildFirstStage();
        mFirstStageBuilt = true;
    }
    else if (!mSecondStageBuilt && mCurrentStage == BossStage::DEATH_RAYS_SUMMONING) {
        BuildSecondStage();
        mSecondStageBuilt = true;
    }
    else if (!mFinalStageBuilt && mCurrentStage == BossStage::ENEMIES_AND_DEATH_RAYS_SUMMONING) {
        BuildFinalStage();
        mFinalStageBuilt = true;
    }
}


void Alzareth::HandleAlzarethState(const float deltaTime) {
    if (mIsRising) {
        mRiseAnimationTimer -= deltaTime;
        if (mRiseAnimationTimer <= 0.0f) {
            mIsRising = false;

            NextStage();

            mIsCasting = true;
            mCastingTimer = CASTING_DURATION;
        } else {
            mDrawComponentAlzareth->SetAnimation(RISING_ANIMATION);
        }
    }

    if (mIsCasting) {
        mCastingTimer -= deltaTime;
        if (mCastingTimer <= 0.0f) {
            mIsCasting = false;
            mIsIdle = true;

            BuildStage();

            mIsShieldBuilding = true;
            mShieldBuildingTimer = BUILDING_SHIELD_DURATION;
        } else {
            mDrawComponentAlzareth->SetAnimation(CASTING_ANIMATION);
        }
    }

    if (mIsIdle) {
        if (CurrentStageCleared()) {
            mIsIdle = false;
            mIsFalling = true;
            mFallAnimationTimer = FALL_ANIMATION_DURATION;

            mIsShieldDestroying = true;
            mShieldDestructionTimer = DESTROYING_SHIELD_DURATION;
        } else {
            mDrawComponentAlzareth->SetAnimation(IDLE_ANIMATION);
        }
    }

    if (mIsFalling) {
        mFallAnimationTimer -= deltaTime;
        if (mFallAnimationTimer <= 0.0f) {
            if (mCurrentStage == BossStage::ENEMIES_AND_DEATH_RAYS_SUMMONING) {
                mGame->GetSceneManager()->SetGameScene(SceneManagerSystem::GameScene::Win);
            }

            mIsFalling = false;
            mIsVulnerable = true;
            mVulnerabilityTimer = VULNERABILITY_DURATION;;
        } else {
            mDrawComponentAlzareth->SetAnimation(FALLING_ANIMATION);
        }
    }

    if (mIsVulnerable) {
        mVulnerabilityTimer -= deltaTime;
        if (mVulnerabilityTimer <= 0.0f) {
            mIsVulnerable = false;
            mIsRising = true;
            mRiseAnimationTimer = RISE_ANIMATION_DURATION;
        } else {
            mDrawComponentAlzareth->SetAnimation(VULNERABLE_ANIMATION);
        }
    }
}

void Alzareth::HandleShieldState(const float deltaTime) {
    if (mIsShieldBuilding) {
        mDrawComponentShield->SetIsPaused(false);
        mShieldBuildingTimer -= deltaTime;
        if (mShieldBuildingTimer <= 0.0f) {
            mIsShieldBuilding = false;
            mIsShieldActive = true;
        } else {
            mDrawComponentShield->SetAnimation(SHIELD_BUILDING_ANIMATION);
        }
    }

    if (mIsShieldActive) {
        mDrawComponentShield->SetIsPaused(false);
        mDrawComponentShield->SetAnimation(SHIELD_ACTIVE_ANIMATION);
    }

    if (mIsShieldDestroying) {
        mDrawComponentShield->SetIsPaused(false);
        mShieldDestructionTimer -= deltaTime;
        if (mShieldDestructionTimer <= 0.0f) {
            mIsShieldDestroying = false;
            mIsShieldActive = false;
            mDrawComponentShield->SetIsPaused(true);
        } else {
            mDrawComponentShield->SetAnimation(SHIELD_DESTRUCTION_ANIMATION);
        }
    }
}


void Alzareth::OnUpdate(const float deltaTime) {
    HandleAlzarethState(deltaTime);

    HandleShieldState(deltaTime);
}

void Alzareth::OnTakeDamage(const float damage) {
    if (!mIsVulnerable) return;

    mCurrentHealth -= damage;
    mCurrentHealth = Math::Max(mCurrentHealth, 0.0f);
    mGame->GetAudioSystem()->PlaySound("boss_damage.wav", false);

    if (mCurrentStage == BossStage::ENEMIES_SUMMONING && mCurrentHealth <= 2.f/3 * mMaxHealth) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 2.f/3 * mMaxHealth;
    }
    else if (mCurrentStage == BossStage::DEATH_RAYS_SUMMONING && mCurrentHealth <= 1.f/3 * mMaxHealth) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 1.f/3 * mMaxHealth;
    }
    else if (mCurrentStage == BossStage::ENEMIES_AND_DEATH_RAYS_SUMMONING && mCurrentHealth <= 0.0f) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 0.0f;
    }
}


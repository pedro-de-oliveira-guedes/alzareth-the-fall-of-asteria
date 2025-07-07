#include "Alzareth.h"

#include "../../Game.h"
#include "../../Systems/SceneManager/SceneManagerSystem.h"
#include "../../Utils/Random.h"
#include "../Enemies/Golem/Golem.h"
#include "../Enemies/Golem2/Golem2.h"
#include "../Enemies/Skeleton/Skeleton.h"
#include "../Enemies/Ghost/Ghost.h"

Alzareth::Alzareth(Game *game) : Actor(game) {
    const float levelWidth = static_cast<float>(mGame->GetSceneManager()->GetLevelSize().first * SceneManagerSystem::TILE_SIZE);
    const float levelHeight = static_cast<float>(mGame->GetSceneManager()->GetLevelSize().second * SceneManagerSystem::TILE_SIZE);
    SetPosition(Vector2(levelWidth / 2, 1.f/4 * levelHeight));

    mMaxHealth = 1000.0f;
    mCurrentHealth = mMaxHealth;
    mCurrentStage = BossStage::SPAWNING;

    mBasicEnemiesStageBuilt = false;
    mDeathRaysStageBuilt = false;
    mAllEnemiesAndDeathRaysStageBuilt = false;

    mIsIdle = false;

    mIsFalling = false;
    mFallAnimationTimer = FALL_ANIMATION_DURATION;

    mIsRising = false;
    mRiseAnimationTimer = RISE_ANIMATION_DURATION;

    mIsVulnerable = true;
    mVulnerabilityTimer = 1.f;

    mIsCasting = false;
    mCastingTimer = CASTING_DURATION;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mAABBColliderComponent = new AABBColliderComponent(
        this,
        0,
        0,
        Game::SPRITE_SIZE,
        Game::SPRITE_SIZE,
        ColliderLayer::Enemy
    );

    mDrawComponentAlzareth = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Alzareth/Wizard/Alzareth.png",
        "../Assets/Sprites/Alzareth/Wizard/Alzareth.json",
        100
    );
    mDrawComponentAlzareth->AddAnimation(IDLE_ANIMATION, { 0, 1 });
    mDrawComponentAlzareth->AddAnimation(CASTING_ANIMATION, { 2, 3, 4, 5 });
    mDrawComponentAlzareth->AddAnimation(FALLING_ANIMATION, { 6, 7, 8 });
    mDrawComponentAlzareth->AddAnimation(VULNERABLE_ANIMATION, { 9, 10 });
    mDrawComponentAlzareth->AddAnimation(RISING_ANIMATION, { 13, 12, 11 });
    mDrawComponentAlzareth->SetAnimFPS(2.f);
    mDrawComponentAlzareth->SetAnimation(VULNERABLE_ANIMATION);

    mIsShieldBuilding = false;
    mShieldBuildingTimer = BUILDING_SHIELD_DURATION;

    mIsShieldActive = false;

    mIsShieldDestroying = false;
    mShieldDestructionTimer = DESTROYING_SHIELD_DURATION;

    mDrawComponentShield = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Alzareth/Shield/Alzareth_Shield.png",
        "../Assets/Sprites/Alzareth/Shield/Alzareth_Shield.json",
        101,
        100.f
    );
    mDrawComponentShield->AddAnimation(SHIELD_BUILDING_ANIMATION, { 0, 1, 2 });
    mDrawComponentShield->AddAnimation(SHIELD_DESTRUCTION_ANIMATION, { 3, 4, 5 });
    mDrawComponentShield->AddAnimation(SHIELD_ACTIVE_ANIMATION, { 6, 7, 8, 9 });
    mDrawComponentShield->SetAnimFPS(3.f);
    mDrawComponentShield->SetEnabled(false);
}

Alzareth::~Alzareth() {
    mSummonedEnemies.clear();
    mDeathRays.clear();
}

void Alzareth::NextStage() {
    if (!CurrentStageCleared()) return;

    if (mCurrentStage == BossStage::SPAWNING) {
        mCurrentStage = BossStage::BASIC_ENEMIES_SUMMONING;
    }
    else if (mCurrentStage == BossStage::BASIC_ENEMIES_SUMMONING) {
        if (mCurrentHealth <= 4.f/5 * mMaxHealth) {
            mCurrentStage = BossStage::ADVANCED_ENEMIES_SUMMONING;
        } else {
            mBasicEnemiesStageBuilt = false;
        }
    }
    else if (mCurrentStage == BossStage::ADVANCED_ENEMIES_SUMMONING) {
        if (mCurrentHealth <= 3.f/5 * mMaxHealth) {
            mCurrentStage = BossStage::DEATH_RAYS_SUMMONING;
        } else {
            mDeathRaysStageBuilt = false;
        }
    }
    else if (mCurrentStage == BossStage::DEATH_RAYS_SUMMONING) {
        if (mCurrentHealth <= 2.f/5 * mMaxHealth) {
            mCurrentStage = BossStage::ALL_ENEMIES_SUMMONING;
        } else {
            mAdvancedEnemiesStageBuilt = false;
        }
    }
    else if (mCurrentStage == BossStage::ALL_ENEMIES_SUMMONING) {
        if (mCurrentHealth <= 1.f/5 * mMaxHealth) {
            mCurrentStage = BossStage::ALL_ENEMIES_AND_DEATH_RAYS_SUMMONING;
        } else {
            mAllEnemiesStageBuilt = false;
        }
    }
}

Vector2 Alzareth::GenerateRandomPosition() const {
    const float levelWidth = mGame->GetSceneManager()->GetLevelSize().first * SceneManagerSystem::TILE_SIZE;
    const float levelHeight = mGame->GetSceneManager()->GetLevelSize().second * SceneManagerSystem::TILE_SIZE;

    int x = Random::GetIntRange(100, levelWidth - 100);
    int y = Random::GetIntRange(50, levelHeight - 50);

    if (
        x > GetPosition().x && x < GetPosition().x + Game::SPRITE_SIZE &&
        y > GetPosition().y && y < GetPosition().y + Game::SPRITE_SIZE
    ) {
        const bool negativeX = Random::GetIntRange(0, 1) == 0;
        x += Random::GetIntRange(Game::SPRITE_SIZE, 2*Game::SPRITE_SIZE) * (negativeX ? -1 : 1);

        const bool negativeY = Random::GetIntRange(0, 1) == 0;
        y += Random::GetIntRange(Game::SPRITE_SIZE, 2*Game::SPRITE_SIZE) * (negativeY ? -1 : 1);
    }

    return Vector2(x, y);
}

void Alzareth::BuildBasicEnemiesStage() {
    mSummonedEnemies.clear();

    for (int i = 0; i < BASIC_ENEMIES_AMOUNT; i++) {
        const Vector2 position = GenerateRandomPosition();

        if (Random::GetIntRange(0, 100) < 60) {
            mSummonedEnemies.push_back(new Golem(mGame, position));
        }
        else {
            mSummonedEnemies.push_back(new Skeleton(mGame, position));
        }
    }
}

void Alzareth::BuildAdvancedEnemiesStage() {
    mSummonedEnemies.clear();

    for (int i = 0; i < ADVANCED_ENEMIES_AMOUNT; i++) {
        const Vector2 position = GenerateRandomPosition();

        if (Random::GetIntRange(0, 100) < 60) {
            mSummonedEnemies.push_back(new Golem2(mGame, position));
        }
        else {
            mSummonedEnemies.push_back(new Ghost(mGame, position));
        }
    }
}

void Alzareth::BuildDeathRaysStage() {
    mDeathRays.clear();

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
        3.0f
    ));
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(0.f, sceneHeight - 50.f),
        Vector2(0.f, sceneHeight / 2.f + 75.f),
        3.0f
    ));

    // Left death ray sweeping the screen
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(50.f, 0.f),
        Vector2(sceneWidth - 75.f, 0.f),
        6.0f
    ));

    // Top death ray sweeping the screen
    mDeathRays.push_back(new DeathRay(
        mGame,
        Vector2(0.f, 50.f),
        Vector2(0.f, sceneHeight - 75.f),
        9.0f
    ));
}

void Alzareth::BuildAllEnemiesStage() {
    mSummonedEnemies.clear();

    for (int i = 0; i < ALL_ENEMIES_AMOUNT; i++) {
        const Vector2 position = GenerateRandomPosition();

        if (Random::GetIntRange(0, 100) < 30) {
            mSummonedEnemies.push_back(new Golem(mGame, position));
        }
        else if (Random::GetIntRange(0, 100) < 60) {
            mSummonedEnemies.push_back(new Skeleton(mGame, position));
        }
        else if (Random::GetIntRange(0, 100) < 80) {
            mSummonedEnemies.push_back(new Golem2(mGame, position));
        }
        else {
            mSummonedEnemies.push_back(new Ghost(mGame, position));
        }
    }
}

void Alzareth::BuildAllEnemiesAndDeathRaysStage() {
    BuildAllEnemiesStage();
    BuildDeathRaysStage();
}

bool Alzareth::CurrentStageCleared() const {
    if (
        mCurrentStage == BossStage::BASIC_ENEMIES_SUMMONING ||
        mCurrentStage == BossStage::ADVANCED_ENEMIES_SUMMONING ||
        mCurrentStage == BossStage::ALL_ENEMIES_SUMMONING
    ) {
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

    if (mCurrentStage == BossStage::ALL_ENEMIES_AND_DEATH_RAYS_SUMMONING) {
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

    return true;
}

void Alzareth::BuildStage() {
    if (!mBasicEnemiesStageBuilt && mCurrentStage == BossStage::BASIC_ENEMIES_SUMMONING) {
        BuildBasicEnemiesStage();
        mBasicEnemiesStageBuilt = true;
    }
    else if (!mAdvancedEnemiesStageBuilt && mCurrentStage == BossStage::ADVANCED_ENEMIES_SUMMONING) {
        BuildAdvancedEnemiesStage();
        mAdvancedEnemiesStageBuilt = true;
    }
    else if (!mDeathRaysStageBuilt && mCurrentStage == BossStage::DEATH_RAYS_SUMMONING) {
        BuildDeathRaysStage();
        mDeathRaysStageBuilt = true;
    }
    else if (!mAllEnemiesStageBuilt && mCurrentStage == BossStage::ALL_ENEMIES_SUMMONING) {
        BuildAllEnemiesStage();
        mAllEnemiesStageBuilt = true;
    }
    else if (!mAllEnemiesAndDeathRaysStageBuilt && mCurrentStage == BossStage::ALL_ENEMIES_AND_DEATH_RAYS_SUMMONING) {
        BuildAllEnemiesAndDeathRaysStage();
        mAllEnemiesAndDeathRaysStageBuilt = true;
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
            mAABBColliderComponent->SetEnabled(true);
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
            mAABBColliderComponent->SetEnabled(false);
        }
    }
}

void Alzareth::HandleShieldState(const float deltaTime) {
    if (mIsShieldBuilding) {
        mDrawComponentShield->SetEnabled(true);
        mShieldBuildingTimer -= deltaTime;
        if (mShieldBuildingTimer <= 0.0f) {
            mIsShieldBuilding = false;
            mIsShieldActive = true;
        } else {
            mDrawComponentShield->SetAnimation(SHIELD_BUILDING_ANIMATION);
        }
    }

    if (mIsShieldActive) {
        mDrawComponentShield->SetEnabled(true);
        mDrawComponentShield->SetAnimation(SHIELD_ACTIVE_ANIMATION);
    }

    if (mIsShieldDestroying) {
        mDrawComponentShield->SetEnabled(true);
        mShieldDestructionTimer -= deltaTime;
        if (mShieldDestructionTimer <= 0.0f) {
            mIsShieldDestroying = false;
            mIsShieldActive = false;
            mDrawComponentShield->SetEnabled(false);
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
    mGame->GetAudioSystem()->PlaySound("enemy_damage.mp3", false);

    if (mCurrentStage == BossStage::BASIC_ENEMIES_SUMMONING && mCurrentHealth <= 4.f/5 * mMaxHealth) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 4.f/5 * mMaxHealth;
    }
    else if (mCurrentStage == BossStage::ADVANCED_ENEMIES_SUMMONING && mCurrentHealth <= 3.f/5 * mMaxHealth) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 3.f/5 * mMaxHealth;
    }
    else if (mCurrentStage == BossStage::DEATH_RAYS_SUMMONING && mCurrentHealth <= 2.f/5 * mMaxHealth) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 2.f/5 * mMaxHealth;
    }
    else if (mCurrentStage == BossStage::ALL_ENEMIES_SUMMONING && mCurrentHealth <= 1.f/5 * mMaxHealth) {
        mVulnerabilityTimer = -1.0f;
        mCurrentHealth = 1.f/5 * mMaxHealth;
    }
    else if (mCurrentStage == BossStage::ALL_ENEMIES_AND_DEATH_RAYS_SUMMONING && mCurrentHealth <= 0.0f) {
        mGame->GetSceneManager()->SetGameScene(SceneManagerSystem::GameScene::Win);
    }
}

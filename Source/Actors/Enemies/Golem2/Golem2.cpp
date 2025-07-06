#include "Golem2.h"
#include "../../../Game.h"
#include "../../../Components/PhysicsComponents/RigidBodyComponent.h"
#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../Items/Collectible/CollectibleItem.h"
#include "../../Projectile/Projectile.h"
#include "../../Items/Weapons/Ranged/MagicToken.h"

#include <random>

const std::string IDLE_ANIMATION = "idle";
const std::string WALKING_ANIMATION = "walking";
const std::string ATTACK_ANIMATION = "attack";


Golem2::Golem2(Game *game, Vector2 position) : Enemy(game) {

    SetPosition(position);

    mMaxHealth = 130.0f;
    mCurrentHealth = mMaxHealth;
    mIsDead = false;

    mWalkSpeed =  70.0f;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    int golem2Size = 99;
    int colliderSize = 80;
    
    int offsetX = (golem2Size - colliderSize) / 2;
    int offsetY = (golem2Size - colliderSize) / 2;
    
    mColliderComponent = new AABBColliderComponent(
        this, 
        offsetX,           
        offsetY,           
        colliderSize,      
        colliderSize,      
        ColliderLayer::Enemy,
        false,
        true
    );

    mDamageAttack = 5.0f;

    mAttackCooldown = 1.5f;

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Golem_2/golem2.png",
        "../Assets/Sprites/Golem_2/golem2.json",
        100
    );

    mDrawComponent->AddAnimation(IDLE_ANIMATION, {1});
    mDrawComponent->AddAnimation(ATTACK_ANIMATION, {3, 4, 5});
    mDrawComponent->AddAnimation(WALKING_ANIMATION, {1, 3, 0, 9});
    mDrawComponent->SetAnimFPS(10.f);

    mDrawComponent->SetAnimation(IDLE_ANIMATION);
}

void Golem2::Attack() {
    if (mAttackCooldown >= 0.0f) {
        return;
    }

    auto *player = mGame->GetPlayer();

    if (!player || player->GetIsDashing()) {
        return;
    }

    mIsWalking = false;
    mIsAttacking = true;

    if (mGame->GetAudioSystem()->GetSoundState(mAttackSound) != SoundState::Playing)
        mAttackSound = mGame->GetAudioSystem()->PlaySound("monster_attack.wav", false);

    player->TakeDamage(mDamageAttack);
    ManageAnimations();

    mAttackCooldown = 0.8f; // reset
}

void Golem2::OnUpdate(float deltaTime) {

    //DebugColliderPosition();

    if (mCurrentHealth <= 0.0f) {
        mState = ActorState::Destroy;
        Kill();
        return;
    }


    if (mAttackCooldown > 0.0f) {
        mAttackCooldown -= deltaTime;
    }
    
    Vector2 EnemyPos  = GetPosition();

    // add small random offset to the enemy position
    EnemyPos.x += (std::rand() % 10 - 5) * 0.1f; // random offset between -0.5 and 0.5
    EnemyPos.y += (std::rand() % 10 - 5) * 0.1f; // random offset between -0.5 and 0.5

    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 toPlayer  = playerPos - EnemyPos;

    // rotate based on player position
    if (toPlayer.x < 0.0f) {
        SetRotation(Math::Pi);
    } else {
        SetRotation(0.0f);
    }

    float distance = toPlayer.Length();

    if (distance > 0.0f) toPlayer *= 1/distance; // normalize

    if (distance < 30.0f) {
        Attack();
    } else {
        mIsWalking = true;
        mIsAttacking = false;

        Vector2 step = toPlayer * (mWalkSpeed) * (deltaTime);
        SetPosition(EnemyPos + step);
    }


    ManageAnimations();
}

void Golem2::Kill() {
    if (mState == ActorState::Destroy) {
        return; // ensure Kill is executed only once
    }
    mGame->RemoveActor(this);

    mDrawComponent->SetIsVisible(false);
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetEnabled(false);
    mIsDead = true;

    // get random int
    int randomInt = std::rand() % 100;

    if (randomInt > 20 && randomInt < 50) {
        new CollectibleItem(mGame, "Energy_Potion", Item::ItemType::Consumable,
            "../Assets/Sprites/Items/Energy/energy_potion.png",
            "../Assets/Sprites/Items/Energy/energy_potion_inventory.png",
            "../Assets/Sprites/Items/Energy/energy_potion.json",
            1, GetPosition());
    }
    else if (randomInt < 65) {
        new CollectibleItem(mGame, "Health_Potion", Item::ItemType::Consumable,
            "../Assets/Sprites/Items/Health/health_potion.png",
            "../Assets/Sprites/Items/Health/health_potion_inventory.png",
            "../Assets/Sprites/Items/Health/health_potion.json",
            1, GetPosition());
    }
    else if (randomInt < 70) {
        new CollectibleItem(mGame, "Invulnerability_Potion", Item::ItemType::Consumable,
            "../Assets/Sprites/Items/Invulnerability/invulnerability_potion.png",
            "../Assets/Sprites/Items/Invulnerability/invulnerability_potion_inventory.png",
            "../Assets/Sprites/Items/Invulnerability/invulnerability_potion.json",
            1, GetPosition());
    }
    else if (randomInt < 85){
        new MagicToken(
            mGame,
            "Magic_Token",
            "../Assets/Sprites/Weapons/Token/magic_token.png",
            "../Assets/Sprites/Weapons/Token/token_inventory.png",
            "../Assets/Sprites/Weapons/Token/magic_token.json",
            GetPosition(),
            1);
    }
}

void Golem2::ManageAnimations() const {
    if (mIsAttacking) {
        mDrawComponent->SetAnimation(ATTACK_ANIMATION);
    } else if (!mIsWalking) {
        mDrawComponent->SetAnimation(IDLE_ANIMATION);
    } else if (mIsWalking) {
        mDrawComponent->SetAnimation(WALKING_ANIMATION);
    }
}

void Golem2::OnCollision(float minOverlap, AABBColliderComponent *other) {

    if (other->GetLayer() == ColliderLayer::PlayerProjectile) {
        auto projectile = dynamic_cast<Projectile*>(other->GetOwner());
        if (projectile) {
            TakeDamage(projectile->GetDamage());
            projectile->SetState(ActorState::Destroy);
            mGame->RemoveActor(projectile);
            return; 
        }
    }

    if (other->GetLayer() == ColliderLayer::Player) {
        // get the player damage
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (!player) {
            return;
        }

        Attack();
    }

}
#include "Ghost.h"
#include "../../../Game.h"
#include "../../../Components/PhysicsComponents/RigidBodyComponent.h"
#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../Items/Collectible/CollectibleItem.h"
#include "../../Projectile/GhostAttack/GhostAttack.h"

#include <random>

const std::string IDLE_ANIMATION = "idle";
const std::string WALKING_ANIMATION = "walking";
const std::string ATTACK_ANIMATION = "attack";


Ghost::Ghost(Game *game, Vector2 position) : Enemy(game) {

    SetPosition(position);

    mMaxHealth = 100.0f;
    mCurrentHealth = mMaxHealth;
    mIsDead = false;

    mWalkSpeed =  60.0f;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    int GhostSize = 99;
    int colliderSize = 80;
    
    int offsetX = (GhostSize - colliderSize) / 2;
    int offsetY = (GhostSize - colliderSize) / 2;
    
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

    mDamageAttack = 15.0f;

    mAttackCooldown = 2.0f;

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Ghost/ghost.png",
        "../Assets/Sprites/Ghost/ghost.json",
        100
    );

    mDrawComponent->AddAnimation(IDLE_ANIMATION, {0});
    mDrawComponent->AddAnimation(ATTACK_ANIMATION, {4,6,9, 2});
    mDrawComponent->AddAnimation(WALKING_ANIMATION, {10,1,7,8});
    mDrawComponent->SetAnimFPS(10.f);

    mDrawComponent->SetAnimation(IDLE_ANIMATION);
}

void Ghost::Attack() {
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

    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 direction = playerPos - GetPosition();
    direction.Normalize();

    new GhostAttack(mGame, GetPosition(), direction * 200.0f, mDamageAttack); 
    ManageAnimations();

    mAttackCooldown = 2.0f; // reset
}

void Ghost::OnUpdate(float deltaTime) {

    //DebugColliderPosition();


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

    if (distance < 500.0f) {
        Attack();
    } else {
        mIsWalking = true;
        mIsAttacking = false;

        Vector2 step = toPlayer * (mWalkSpeed) * (deltaTime);
        SetPosition(EnemyPos + step);
    }


    ManageAnimations();
}

void Ghost::Kill() {
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

    if (randomInt < 20) {
        new CollectibleItem(mGame, "Energy_Potion", Item::ItemType::Consumable,
            "../Assets/Sprites/Items/Energy/energy_potion.png",
            "../Assets/Sprites/Items/Energy/energy_potion_inventory.png",
            "../Assets/Sprites/Items/Energy/energy_potion.json",
            1, GetPosition());
    } else if (randomInt < 50) {
        new CollectibleItem(mGame, "Health_Potion", Item::ItemType::Consumable,
            "../Assets/Sprites/Items/Health/health_potion.png",
            "../Assets/Sprites/Items/Health/health_potion_inventory.png",
            "../Assets/Sprites/Items/Health/health_potion.json",
            1, GetPosition());
    }
    // TODO: Add more items to drop
}

void Ghost::ManageAnimations() const {
    if (mIsAttacking) {
        mDrawComponent->SetAnimation(ATTACK_ANIMATION);
    } else if (!mIsWalking) {
        mDrawComponent->SetAnimation(IDLE_ANIMATION);
    } else if (mIsWalking) {
        mDrawComponent->SetAnimation(WALKING_ANIMATION);
    }
}

void Ghost::OnCollision(float minOverlap, AABBColliderComponent *other) {

    if (other->GetLayer() == ColliderLayer::Player) {
        // get the player damage
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (!player) {
            return;
        }

        Attack();
    }

}
#include "Golem.h"
#include "../../Game.h"
#include "../../Components/PhysicsComponents/RigidBodyComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"

#include <random>

const std::string IDLE_ANIMATION = "idle";
const std::string WALKING_ANIMATION = "walking";
const std::string ATTACK_ANIMATION = "attack";


Golem::Golem(Game *game) : Enemy(game) {

    mWalkSpeed =  1.0f;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::SPRITE_SIZE/3, Game::SPRITE_SIZE/3, ColliderLayer::Enemy);

    mDamageAttack = 5.0f;

    mAttackCooldown = 1.0f;

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Golem/golem.png",
        "../Assets/Sprites/Golem/golem.json",
        100
    );

    mDrawComponent->AddAnimation(IDLE_ANIMATION, {0});
    mDrawComponent->AddAnimation(ATTACK_ANIMATION, {2, 3});
    mDrawComponent->AddAnimation(WALKING_ANIMATION, {5, 1, 8});
    mDrawComponent->SetAnimFPS(10.f);

    mDrawComponent->SetAnimation(IDLE_ANIMATION);

    SDL_Log("Golem created");

}

void Golem::Attack() {

    if (mAttackCooldown >= 0.0f) {
        SDL_Log("Golem is on cooldown, cannot attack yet");
        return; // can't attack yet
    }

    mIsWalking = false;
    mIsAttacking = true;
    SDL_Log("Golem attacking player");

    auto player = static_cast<Player*>(mGame->GetPlayer());

    player->TakeDamage(mDamageAttack);
    ManageAnimations();

    mAttackCooldown = 1.0f; // reset

}

void Golem::OnUpdate(float deltaTime) {

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

    float distance    = toPlayer.Length();

    if (distance > 0.0f) toPlayer *= 1/distance; // normalize

    

    if (distance < 100.0f) {
            Attack();
    } else {
        mIsWalking = true;
        mIsAttacking = false;

        Vector2 step = toPlayer * (mWalkSpeed) * (deltaTime);
        SetPosition(EnemyPos + step);
    }


    ManageAnimations();
}

void Golem::Kill() {
    mState = ActorState::Destroy;
    mGame->RemoveActor(this);

    mDrawComponent->SetIsVisible(false);
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetEnabled(false);
    SDL_Log("Golem killed");


}

void Golem::ManageAnimations() const {
    if (mIsAttacking) {
        mDrawComponent->SetAnimation(ATTACK_ANIMATION);
    } else if (!mIsWalking) {
        mDrawComponent->SetAnimation(IDLE_ANIMATION);
    } else if (mIsWalking) {
        mDrawComponent->SetAnimation(WALKING_ANIMATION);
    }
}

void Golem::OnCollision(float minOverlap, AABBColliderComponent *other) {

    if (other->GetLayer() == ColliderLayer::Player) {
        // get the player damage
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (!player) {
            return;
        }

        Attack();
        

    }

}
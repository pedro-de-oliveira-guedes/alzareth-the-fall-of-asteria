#include "Golem.h"
#include "../../Game.h"
#include "../../Components/PhysicsComponents/RigidBodyComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"

const std::string IDLE_ANIMATION = "idle";
const std::string WALKING_ANIMATION = "walking";
const std::string ATTACK_ANIMATION = "attack";

Golem::Golem(Game *game, const float walkSpeed) : Actor(game) {
    mMaxHealth = 100.0f;
    mCurrentHealth = mMaxHealth;

    mWalkSpeed = walkSpeed;

    mIsWalking = false;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::SPRITE_SIZE, Game::SPRITE_SIZE, ColliderLayer::Enemy);

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/Golem/golem.png",
        "../Assets/Sprites/Golem/golem.json",
        100
    );

    mDrawComponent->AddAnimation(IDLE_ANIMATION, {0});
    mDrawComponent->AddAnimation(ATTACK_ANIMATION, {2, 3});
    mDrawComponent->AddAnimation(WALKING_ANIMATION, {5, 1, 8});
    mDrawComponent->SetAnimation(IDLE_ANIMATION);
    mDrawComponent->SetAnimFPS(10.f);

    SDL_Log("Golem created");

}

void Golem::HandleRotation() {


}

void Golem::ApplyBasicMovement(Vector2 force_vector) {
}


void Golem::OnUpdate(const float deltaTime) {

    mCurrentHealth = std::max(mCurrentHealth, 0.f);

    ManageAnimations();
}

void Golem::ManageAnimations() const {
    if (mIsWalking) {
        mDrawComponent->SetAnimation(WALKING_ANIMATION);
    } else {
        mDrawComponent->SetAnimation(IDLE_ANIMATION);
    }
}


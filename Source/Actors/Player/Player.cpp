#include "Player.h"
#include "../../Game.h"
#include "../../Components/PhysicsComponents/RigidBodyComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"

Player::Player(Game *game, const float walkSpeed, const float runSpeed, const float dashSpeed) : Actor(game) {
    mMaxHealth = 100.0f;
    mCurrentHealth = mMaxHealth;

    mMaxEnergy = 100.0f;
    mCurrentEnergy = mMaxEnergy;

    mWalkSpeed = walkSpeed;
    mRunSpeed = runSpeed;
    mDashSpeed = dashSpeed;

    mIsWalking = false;
    mIsRunning = false;
    mIsDashing = false;

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0,Game::SPRITE_SIZE, Game::SPRITE_SIZE, ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/00-Player/main_character.png",
        "../Assets/Sprites/00-Player/main_character.json",
        100
    );
    mDrawComponent->AddAnimation("dash", {0, 1, 2, 3});
    mDrawComponent->AddAnimation("idle", {4});
    mDrawComponent->AddAnimation("walking", {8, 9, 10, 11});
    mDrawComponent->AddAnimation("running", {5, 6, 7});
    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.f);
}

void Player::OnProcessInput(const Uint8 *keyState) {
    if (mIsDashing && mDashTime > 0.f) return;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    mouseX += mGame->GetCameraPos().x;
    mouseY += mGame->GetCameraPos().y;

    if (mouseX < GetPosition().x) SetRotation(Math::Pi);
    else if (mouseX > GetPosition().x) SetRotation(0);

    if (keyState[SDL_SCANCODE_LCTRL]) {
        mIsRunning = true;
    } else {
        mIsRunning = false;
    }

    auto force_vector = Vector2::Zero;
    if (keyState[SDL_SCANCODE_W]) {
        force_vector += Vector2(0.f, -1.f);
    }
    if (keyState[SDL_SCANCODE_A]) {
        force_vector += Vector2(-1.f, 0.f);
    }
    if (keyState[SDL_SCANCODE_S]) {
        force_vector += Vector2(0.f, 1.f);
    }
    if (keyState[SDL_SCANCODE_D]) {
        force_vector += Vector2(1.f, 0.f);
    }

    if (std::abs(force_vector.Length()) > 0.f) {
        if (mIsRunning) {
            force_vector *= mRunSpeed;
            mIsWalking = false;
        } else {
            force_vector *= mWalkSpeed;
            mIsWalking = true;
        }
        mRigidBodyComponent->ApplyForce(force_vector);
    } else {
        mIsRunning = false;
        mIsWalking = false;
    }

    if (keyState[SDL_SCANCODE_SPACE] && std::abs(force_vector.Length()) > 0.f) {
        mIsDashing = true;
        mDashTime = mDrawComponent->GetAnimTime("dash");

        mIsRunning = false;
        mIsWalking = false;

        mRigidBodyComponent->SetVelocity(force_vector * mDashSpeed);

        if (force_vector.x < 0) SetRotation(Math::Pi);
        else SetRotation(0);
    } else {
        mIsDashing = false;
    }
}

void Player::OnUpdate(const float deltaTime) {
    if (GetPosition().x < GetGame()->GetCameraPos().x) {
        SetPosition(Vector2(0.f, GetPosition().y));
    }
    else if (GetPosition().x + Game::SPRITE_SIZE > GetGame()->GetCameraPos().x + GetGame()->GetWindowWidth()) {
        SetPosition(Vector2(Game::LEVEL_WIDTH * Game::TILE_SIZE - Game::SPRITE_SIZE, GetPosition().y));
    }

    if (GetPosition().y < GetGame()->GetCameraPos().y) {
        SetPosition(Vector2(GetPosition().x, 0.f));
    }
    else if (GetPosition().y + Game::SPRITE_SIZE > GetGame()->GetCameraPos().y + GetGame()->GetWindowHeight()) {
        SetPosition(Vector2(GetPosition().x, Game::LEVEL_HEIGHT * Game::TILE_SIZE - Game::SPRITE_SIZE));
    }

    if (mIsDashing) mDashTime -= deltaTime;

    ManageAnimations();
}

void Player::ManageAnimations() const {
    if (mIsWalking) {
        mDrawComponent->SetAnimation("walking");
    } else if (mIsRunning) {
        mDrawComponent->SetAnimation("running");
    } else if (mIsDashing) {
        mDrawComponent->SetAnimation("dash");
    } else {
        mDrawComponent->SetAnimation("idle");
    }
}

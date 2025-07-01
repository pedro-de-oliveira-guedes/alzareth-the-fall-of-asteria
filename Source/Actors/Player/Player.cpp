#include "Player.h"
#include "../Enemy.h"
#include "../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../Components/PhysicsComponents/RigidBodyComponent.h"
#include "../../Game.h"
#include "../Items/Collectible/CollectibleItem.h"

const std::string DASH_ANIMATION = "dash";
const std::string IDLE_ANIMATION = "idle";
const std::string WALKING_ANIMATION = "walking";
const std::string RUNNING_ANIMATION = "running";

constexpr float DASH_ENERGY_COST = 10.f;
constexpr float RUNNING_ENERGY_COST = 5.f;
constexpr float ENERGY_RECHARGE_RATE = 3.f;

constexpr float DASH_COOLDOWN = 1.f;
constexpr float ENERGY_RECHARGE_COOLDOWN = 1.f;

Player::Player(Game* game, const float walkSpeed, const float runSpeed, const float dashSpeed) : Actor(game) {
    mMaxHealth = 100.0f;
    mCurrentHealth = 100.0f;

    mMaxEnergy = 100.0f;
    mCurrentEnergy = mMaxEnergy;

    mWalkSpeed = walkSpeed;
    mRunSpeed = runSpeed;
    mDashSpeed = dashSpeed;

    mDashCooldown = -1.f;
    mEnergyRechargeCooldown = -1.f;

    mIsWalking = false;
    mIsRunning = false;
    mIsDashing = false;
    mEPressedLastFrame = false;

    mIsInvulnerable = false;      
    mInvulnerabilityTime = 0.0f; 

    mNumberKeysPressedLastFrame.fill(false);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::SPRITE_SIZE, Game::SPRITE_SIZE, ColliderLayer::Player);

    mDrawComponent = new DrawAnimatedComponent(
        this,
        "../Assets/Sprites/00-Player/main_character.png",
        "../Assets/Sprites/00-Player/main_character.json",
        100
    );
    mDrawComponent->AddAnimation(DASH_ANIMATION, { 0, 1, 2, 3 });
    mDrawComponent->AddAnimation(IDLE_ANIMATION, { 4 });
    mDrawComponent->AddAnimation(WALKING_ANIMATION, { 8, 9, 10, 11 });
    mDrawComponent->AddAnimation(RUNNING_ANIMATION, { 5, 6, 7 });
    mDrawComponent->SetAnimation(IDLE_ANIMATION);
    mDrawComponent->SetAnimFPS(10.f);

    mHUDComponent = new HUDComponent(this, mMaxHealth, mMaxEnergy);
}

void Player::HandleRotation() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    mouseX += static_cast<int>(mGame->GetCameraPos().x);

    if (static_cast<float>(mouseX) < GetPosition().x) SetRotation(Math::Pi);
    else if (static_cast<float>(mouseX) > GetPosition().x) SetRotation(0);
}

Vector2 Player::HandleBasicMovementInput(const Uint8* keyState) {
    if (keyState[SDL_SCANCODE_LCTRL]) {
        mIsRunning = true;
    }
    else {
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

    return force_vector;
}

void Player::ApplyBasicMovement(Vector2 force_vector) {
    if (std::abs(force_vector.Length()) > 0.f) {
        if (mIsRunning && mCurrentEnergy > 0.05f * RUNNING_ENERGY_COST) {
            force_vector *= mRunSpeed;
            mIsWalking = false;
            mEnergyRechargeCooldown = ENERGY_RECHARGE_COOLDOWN;
        }
        else {
            force_vector *= mWalkSpeed;
            mIsWalking = true;
        }
        mRigidBodyComponent->ApplyForce(force_vector);
    }
    else {
        mIsRunning = false;
        mIsWalking = false;
    }
}

void Player::HandleDash(const Uint8* keyState, const Vector2 force_vector) {
    if (mDashCooldown > 0.f) {
        mIsDashing = false;
        return;
    }

    if (keyState[SDL_SCANCODE_SPACE] && std::abs(force_vector.Length()) > 0.f && mCurrentEnergy >= DASH_ENERGY_COST) {
        mIsDashing = true;
        mDashTime = mDrawComponent->GetAnimTime(DASH_ANIMATION);

        mIsRunning = false;
        mIsWalking = false;

        mRigidBodyComponent->SetVelocity(force_vector * mDashSpeed);
        mCurrentEnergy -= DASH_ENERGY_COST;

        if (force_vector.x < 0) SetRotation(Math::Pi);
        else SetRotation(0);
    }
    else {
        mIsDashing = false;
    }
}

void Player::HandleItemInput(const Uint8* keyState) {
    bool currentEPressed = keyState[SDL_SCANCODE_C];

    if (currentEPressed && !mEPressedLastFrame) {
        const auto& colliders = mGame->GetNearbyColliders(mPosition);
        for (const AABBColliderComponent* otherCollider : colliders) {
            if (
                otherCollider->GetLayer() == ColliderLayer::Collectible &&
                mColliderComponent->Intersect(*otherCollider) &&
                otherCollider->IsEnabled()
            ) {
                auto* item = dynamic_cast<CollectibleItem*>(otherCollider->GetOwner());
                if (item) {
                    mInventory.AddItem(item);
                    item->SetState(ActorState::Destroy);

                    SDL_Log("Collected item: %s", item->GetName().c_str());
                    break;
                }
            }
        }
    }

    mEPressedLastFrame = currentEPressed;
}

void Player::HandleUseItem(const Uint8* keyState) {
    for (int i = 0; i < 5; ++i) {
        SDL_Scancode key_scancode = static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i);
        bool currentKeyPressed = keyState[key_scancode];

        if (currentKeyPressed && !mNumberKeysPressedLastFrame[i]) {
            UseItemAtIndex(i);
        }
        mNumberKeysPressedLastFrame[i] = currentKeyPressed;
    }
}

void Player::UseItemAtIndex(int index) {
    size_t actualIndex = static_cast<size_t>(index);
    Item* itemToUse = mInventory.GetItemAtIndex(actualIndex);

    if (itemToUse && itemToUse->GetType() == Item::ItemType::Consumable) {
        itemToUse->Use(this);
        mInventory.RemoveItemAtIndex(actualIndex);
    } else {
        // TODO: Play a sound here
    }
}

void Player::HandleStatusEffects(float deltaTime) { 
    if (mIsInvulnerable) {
        mInvulnerabilityTime -= deltaTime;
        if (mInvulnerabilityTime <= 0.0f) {
            mIsInvulnerable = false;
            mInvulnerabilityTime = 0.0f; 
            SDL_Log("Invulnerabilidade desativada!");
        }
    }
}

void Player::OnProcessInput(const Uint8* keyState) {
    if (mIsDashing && mDashTime > 0.f) return;

    HandleRotation();
    HandleItemInput(keyState);
    HandleUseItem(keyState); 
    Attack(keyState);

    const auto force_vector = HandleBasicMovementInput(keyState);
    ApplyBasicMovement(force_vector);

    HandleDash(keyState, force_vector);
}

void Player::Attack(const Uint8 *keyState) {
    // TODO: GASTAR ENERGIA RELACIONADA A CADA ARMA

    if (mIsDashing || mIsRunning || mIsWalking) {
        return;
    }

    int mouseState = SDL_GetMouseState(nullptr, nullptr);
    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        // check if the inventory contains a weapon
        int weaponIdx = mInventory.ReturnWeaponIndex();
    
        if (weaponIdx < 0) {
            SDL_Log("No weapon in inventory to attack with");
            return; // No weapon to attack with
        }

        // Get the equipped weapon
        Item* weaponItem = mInventory.GetItemAtIndex(weaponIdx);
        if (weaponItem && weaponItem->GetType() == Item::ItemType::Weapon) {
            Sword* sword = dynamic_cast<Sword*>(weaponItem);
            if (sword) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                Vector2 mousePos(mouseX + mGame->GetCameraPos().x, 
                               mouseY + mGame->GetCameraPos().y);
                
                
                sword->SetPlayerPos(GetPosition());
                sword->SetMousePos(mousePos);
                sword->DrawForAttack();

                Vector2 attackPosition = GetPosition();
                float rangeX = sword->GetRangeX();
                float rangeY = sword->GetRangeY();

                const auto& colliders = mGame->GetNearbyColliders(mPosition);
                for (const AABBColliderComponent* otherCollider : colliders) {
                    if (otherCollider->GetLayer() == ColliderLayer::Enemy) {
                        Vector2 otherPos = otherCollider->GetOwner()->GetPosition();
                        if (
                            std::abs(otherPos.x - attackPosition.x) <= rangeX &&
                            std::abs(otherPos.y - attackPosition.y) <= rangeY
                        ) {
                            auto *enemy = dynamic_cast<Enemy*>(otherCollider->GetOwner());
                            if (enemy) {
                                enemy->TakeDamage(sword->GetDamage());
                                sword->SetHasHitThisAttack(true);
                            }
                        }
                    }
                }
            }
        }
    }
}

void Player::HandleMapBoundaries() {
    if (GetPosition().x < GetGame()->GetCameraPos().x) {
        SetPosition(Vector2(0.f, GetPosition().y));
    }
    else if (GetPosition().x + Game::SPRITE_SIZE > GetGame()->GetCameraPos().x + GetGame()->GetWindowWidth()) {
        SetPosition(Vector2(Game::LEVEL_WIDTH * Game::TILE_SIZE - Game::SPRITE_SIZE, GetPosition().y));
    }

    if (GetPosition().y < 0) {
        SetPosition(Vector2(GetPosition().x, 0.f));
    }
    else if (GetPosition().y + Game::SPRITE_SIZE > Game::LEVEL_HEIGHT * Game::TILE_SIZE) {
        SetPosition(Vector2(GetPosition().x, Game::LEVEL_HEIGHT * Game::TILE_SIZE - Game::SPRITE_SIZE));
    }
}

void Player::HandleEnergyAndCooldowns(const float deltaTime) {
    if (mIsRunning) {
        mCurrentEnergy -= RUNNING_ENERGY_COST * deltaTime;
    }
    if (mIsDashing) {
        mDashTime -= deltaTime;
        mDashCooldown = DASH_COOLDOWN;
    }
    if (mDashCooldown >= 0.f) {
        mDashCooldown -= deltaTime;
        mEnergyRechargeCooldown = ENERGY_RECHARGE_COOLDOWN;
    }
    if (mEnergyRechargeCooldown > 0.f) {
        mEnergyRechargeCooldown -= deltaTime;
    }
    if (!mIsDashing && !mIsRunning && mCurrentEnergy < mMaxEnergy && mEnergyRechargeCooldown <= 0.f) {
        mCurrentEnergy += ENERGY_RECHARGE_RATE * deltaTime;
        mCurrentEnergy = std::min(mCurrentEnergy, mMaxEnergy);
    }
}

void Player::OnUpdate(const float deltaTime) {
    HandleMapBoundaries();

    HandleEnergyAndCooldowns(deltaTime);

    mCurrentEnergy = std::max(mCurrentEnergy, 0.f);
    mCurrentHealth = std::max(mCurrentHealth, 0.f);
    mHUDComponent->UpdateStats(mMaxHealth, mCurrentHealth, mMaxEnergy, mCurrentEnergy);

    ManageAnimations();
}

void Player::ManageAnimations() const {
    if (mIsWalking) {
        mDrawComponent->SetAnimation(WALKING_ANIMATION);
    }
    else if (mIsRunning) {
        mDrawComponent->SetAnimation(RUNNING_ANIMATION);
    }
    else if (mIsDashing) {
        mDrawComponent->SetAnimation(DASH_ANIMATION);
    }
    else {
        mDrawComponent->SetAnimation(IDLE_ANIMATION);
    }
}

void Player::AddItemToInventory(Item* item) {
    mInventory.AddItem(std::move(item));
}

bool Player::RemoveItemFromInventory(const std::string& itemName) {
    return mInventory.RemoveItem(itemName);
}

void Player::TakeDamage(float damage) {
    mCurrentHealth -= damage;
    mCurrentHealth = std::max(mCurrentHealth, 0.f);
    mHUDComponent->UpdateStats(mMaxHealth, mCurrentHealth, mMaxEnergy, mCurrentEnergy);

    if (mCurrentHealth <= 0.f) {
        Kill();
    }
}

void Player::OnCollision(float minOverlap, AABBColliderComponent *other) {
    if (other->GetLayer() == ColliderLayer::MeleeWeapon) {
        auto weapon = dynamic_cast<Sword*>(other->GetOwner());
        if (!weapon) {
            return;
        }
        weapon->Collect();
        mInventory.AddItem(weapon);
    }
}

void Player::Kill() {
    mState = ActorState::Destroy;
    mGame->Quit();
}
#include "Sword.h"
#include "../../../Player/Player.h"

const std::string ATTACK_ANIMATION = "attack";

Sword::Sword(Game* game, const std::string& name,
               const std::string& texturePath, const std::string& textureInventoryPath, const std::string& spriteSheetData, const Vector2& position, int quantity)
    : Item(game, name, ItemType::Weapon, texturePath, textureInventoryPath, spriteSheetData, quantity) {

    mPosition = position;
    
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 1, 1, ColliderLayer::MeleeWeapon, false, true);

    mDrawComponent = new DrawAnimatedComponent(
        this,
        texturePath,
        spriteSheetData, 
        100 // drawOrder
    );

    mDrawComponent->AddAnimation(ATTACK_ANIMATION, {0});
    mDrawComponent->SetAnimation(ATTACK_ANIMATION);
    mDrawComponent->SetAnimFPS(10.0f);

    SDL_Log("Sword texture path: %s", texturePath.c_str());
    SDL_Log("Sword sprite sheet data: %s", spriteSheetData.c_str());
    

}

Sword::~Sword() {}

void Sword::Use(Player* player) {}

void Sword::OnCollision(float minOverlap, AABBColliderComponent *other) {

  SDL_Log("Sword collided with another actor");

  // check if player collided with a Sword
  if (other->GetLayer() == ColliderLayer::Player) {
    SDL_Log("Sword collided with player, using Sword");
    // Use the Sword on the player
  }
}

void Sword::OnUpdate(float deltaTime) {

    if (mIsAttacking) {
        mTimeAttack -= deltaTime;
        DrawForAttack();
    }

    // Update the draw component
    mDrawComponent->Update(deltaTime);
}

void Sword::DrawForAttack() {

    mIsAttacking = true;
    // Draw the sword in attack position
    mDrawComponent->SetIsVisible(true);
    mDrawComponent->SetAnimation(ATTACK_ANIMATION);

    if (mTimeAttack < 0.0f) {
        mDrawComponent->SetIsVisible(false);
        mTimeAttack = 1.0f; // Reset the attack time
        mIsAttacking = false;
    }
    
}

void Sword::Collect() {
    SDL_Log("Sword collected");
    
    // Disable the draw component
    mDrawComponent->SetIsVisible(false);
    
    // Disable the collider component
    mColliderComponent->SetEnabled(false);
}

void Sword::SetRotation(float angle) {
    mRotation = angle;
    if (mDrawComponent) {
        mDrawComponent->SetRotation(angle);
    }
}
#include "CollectibleItem.h"

const std::string FLOATING_ANIMATION = "floating";

CollectibleItem::CollectibleItem(
    Game* game,
    const std::string& name,
    const ItemType type,
    const std::string& textureSpritePath,
    const std::string& textureInventoryPath,
    const std::string& spriteSheetData,
    const int quantity,
    const Vector2& position
) : Item(game, name, type, textureSpritePath, textureInventoryPath, spriteSheetData, quantity) {
    SetPosition(position);

    mDrawComponent = new DrawAnimatedComponent(
        this,
        textureSpritePath,
        spriteSheetData,
        100 // drawOrder
    );

    mDrawComponent->AddAnimation(FLOATING_ANIMATION, { 0, 1, 2, 3, 4, 5, 6 });
    mDrawComponent->SetAnimation(FLOATING_ANIMATION);
    mDrawComponent->SetAnimFPS(10.0f);
    mDrawComponent->SetIsVisible(true);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE / 4, Game::TILE_SIZE / 4, ColliderLayer::Collectible, false, true);
    mColliderComponent->SetEnabled(true);
}

CollectibleItem::~CollectibleItem() { }

void CollectibleItem::Use(Player* player) {
    if (mType == ItemType::Consumable) {
        if (mName == "Energy_Potion") {
            player->SetCurrentEnergy(player->GetCurrentEnergy() + 30.0f);
        }
        else if (mName == "Health_Potion") {
            player->SetCurrentHealth(player->GetCurrentHealth() + 30.0f);
        }
        else if (mName == "Invulnerability_Potion") {
            player->SetInvulnerable(true);        
            player->SetInvulnerabilityTimer(10.0f);
        }
    }
}

void CollectibleItem::Collect() {
    mDrawComponent->SetIsVisible(false);
    mColliderComponent->SetEnabled(false);
}

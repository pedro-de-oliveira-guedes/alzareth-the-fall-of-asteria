#include "CollectibleItem.h"

const std::string FLOATING_ANIMATION = "floating";

CollectibleItem::CollectibleItem(Game* game, const std::string& name, ItemType type,
                                 const std::string& textureSpritePath, const std::string& textureInventoryPath, const std::string& spriteSheetData, int quantity, const Vector2& position)
    : Item(game, name, type, textureSpritePath, textureInventoryPath, spriteSheetData, quantity) { 
    mPosition = position;
    mName = name;

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

    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE/4, Game::TILE_SIZE/4, ColliderLayer::Collectible, false, true);
    mColliderComponent->SetEnabled(true);
}

CollectibleItem::~CollectibleItem()
{
}

void CollectibleItem::Use(Player* player) {
    if (mName == "Energy Potion" && mType == ItemType::Consumable) {
        player->SetCurrentEnergy(player->GetCurrentEnergy() + 20.0f); 
        player->SetCurrentEnergy(Math::Min(player->GetCurrentEnergy(), player->GetMaxEnergy())); 
        SDL_Log("Jogador usou %s. Energia atual: %f", mName.c_str(), player->GetCurrentEnergy());
    } else {
        SDL_Log("Item %s não possui um uso definido ou não é consumível.", mName.c_str());
    }
}
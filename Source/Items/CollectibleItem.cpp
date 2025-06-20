#include "CollectibleItem.h"

CollectibleItem::CollectibleItem(Game* game, const std::string& name, ItemType type, const std::string& texturePath, int quantity, const Vector2& position)
    : Item(game, name, type, texturePath, quantity) {
    mPosition = position;

    mDrawComponent = new DrawSpriteComponent(this, texturePath);
    mDrawComponent->SetIsVisible(true);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Collectible);
    mColliderComponent->SetEnabled(true);
}

CollectibleItem::~CollectibleItem()
{
    
}

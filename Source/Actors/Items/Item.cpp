#include "./Item.h"

Item::Item(
    Game* game,
    const std::string& name, const ItemType type,
    const std::string& texturePath,
    const std::string& textureInventoryPath,
    const std::string& spriteSheetData,
    const int quantity
) : Actor(game) {
    mName = name;
    mType = type;
    mTexturePath = texturePath;
    mInventoryTexturePath = textureInventoryPath;
    mQuantity = quantity;
    mSpriteSheetData = spriteSheetData;
}


Item::~Item() {}

void Item::Use(Player* player) {
    (void)player;
}

void Item::OnCollision(float minOverlap, AABBColliderComponent *other) {}
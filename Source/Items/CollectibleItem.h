#pragma once
#include "Item.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Game.h"

class CollectibleItem : public Item {
public:
  CollectibleItem(Game* game, const std::string& name, ItemType type,
                  const std::string& textureSpritePath, const std::string& textureInventoryPath, const std::string& spriteSheetData, int quantity, const Vector2& position);
  ~CollectibleItem() override;

  void Use(class Player* player) override;

private:
  DrawAnimatedComponent* mDrawComponent;
  AABBColliderComponent* mColliderComponent;
};
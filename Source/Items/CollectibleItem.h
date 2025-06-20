#pragma once
#include "Item.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Game.h"

class CollectibleItem : public Item {
public:
  CollectibleItem(Game* game, const std::string& name, ItemType type, const std::string& texturePath, int quantity, const Vector2& position);
  ~CollectibleItem() override;

private:
  DrawSpriteComponent* mDrawComponent;
  AABBColliderComponent* mColliderComponent;
};
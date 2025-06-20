#include "Item.h"


Item::Item(Game* game, const std::string& name, ItemType type, const std::string& texturePath, int quantity)
    : Actor(game), 
      mName(name),
      mType(type),
      mTexturePath(texturePath),
      mQuantity(quantity)
{}


Item::~Item() {
    
}
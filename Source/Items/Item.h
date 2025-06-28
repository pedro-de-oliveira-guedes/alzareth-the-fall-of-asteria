#pragma once
#include <string>
#include <SDL.h>

#include "../Actors/Actor.h"

enum class ItemType {
    None,
    Weapon,
    Consumable
};

class Item : public Actor {
  public:
    // NOVO: Adicionado textureInventoryPath
    Item(class Game* game, const std::string &name = "Unknown", ItemType type = ItemType::None,
         const std::string& texturePath = "", const std::string& textureInventoryPath = "", const std::string& spriteSheetData = "", int quantity = 1);
    ~Item() override;

    const std::string& GetName() const { return mName; }
    ItemType GetType() const { return mType; }
    const std::string& GetTexturePath() const { return mTexturePath; }
    const std::string& GetInventoryTexturePath() const { return mInventoryTexturePath; }
    int GetQuantity() const { return mQuantity; }

    void SetQuantity(int quantity) {mQuantity = quantity;}
    void AddQuantity(int quantity) { mQuantity += quantity;}
    void RemoveQuantity(int quantity) { mQuantity -= quantity; if (mQuantity < 0) mQuantity = 0; }
    virtual void Use(class Player* player) = 0; 

  private:
    std::string mName;
    ItemType mType;
    std::string mTexturePath;
    std::string mInventoryTexturePath;
    std::string mSpriteSheetData; 
    int mQuantity;
    int mMaxQuantity = 5;
};
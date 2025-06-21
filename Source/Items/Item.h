#pragma once
#include <string>
#include <SDL2/SDL.h> 

#include "../Actors/Actor.h" 

enum class ItemType {
    None,
    Weapon,
    Consumable
};

class Item : public Actor {
  public:
    Item(class Game* game, const std::string &name = "Unknown", ItemType type = ItemType::None, const std::string& texturePath = "", int quantity = 1);
    ~Item() override; 

    const std::string& GetName() const { return mName; }
    ItemType GetType() const { return mType; }
    const std::string& GetTexturePath() const { return mTexturePath; }
    int GetQuantity() const { return mQuantity; }

    void SetQuantity(int quantity) {mQuantity = quantity;}
    void AddQuantity(int quantity) { mQuantity += quantity;}
    void RemoveQuantity(int quantity) { mQuantity -= quantity; if (mQuantity < 0) mQuantity = 0; }

  private:
    std::string mName;          
    ItemType mType;             
    std::string mTexturePath;   
    int mQuantity; 
    int mMaxQuantity = 5;    
};
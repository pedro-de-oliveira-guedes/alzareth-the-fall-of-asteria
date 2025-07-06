#pragma once
#include <string>
#include <vector>
#include <memory> 
#include "../Actors/Items/Item.h"

class Item;
class Inventory{
  public:
    Inventory();
    ~Inventory();

    Item* AddItem(Item* newItem); 
    bool RemoveItem(const std::string& itemName);
    Item* RemoveItemAtIndex(size_t index);
    bool InventoryFull() const;
    
    Item* GetItem(const std::string& itemName) const;
    bool HasItem(const std::string& itemName) const;

    int GetInventorySize() const {
        int count = 0;
        for (const auto& item_ptr : mItems) {
            if (item_ptr != nullptr) {
                count++;
            }
        }
        return count;
    };
    int GetMaxItems() const { return mMaxItems; }

    const std::vector<Item*>& GetItems() const { return mItems; }
    Item* GetItemAtIndex(size_t index) const;

    int ReturnWeaponIndex();

  private:
    std::vector<Item*> mItems; 
    int mMaxItems = 6; 
};
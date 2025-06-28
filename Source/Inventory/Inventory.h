#pragma once
#include <string>
#include <vector>
#include <memory> 
#include "../Items/Item.h"

class Item;
class Inventory{
  public:
    Inventory();
    ~Inventory();

    void AddItem(Item* newItem); 
    bool RemoveItem(const std::string& itemName);
    bool RemoveItemAtIndex(size_t index);
    
    Item* GetItem(const std::string& itemName) const;
    bool HasItem(const std::string& itemName) const;

    const std::vector<Item*>& GetItems() const { return mItems; }
    Item* GetItemAtIndex(size_t index) const;

  private:
    std::vector<Item*> mItems; 
    int mMaxItems = 5; 
};
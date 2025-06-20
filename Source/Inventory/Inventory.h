#pragma once
#include <string>
#include <vector>
#include <memory> // Adicione esta linha
#include "../Items/Item.h"

class Inventory{
  public:
    Inventory();
    ~Inventory();

    void AddItem(std::unique_ptr<Item> newItem); 
    bool RemoveItem(const std::string& itemName, int quantity = 1);

    const std::vector<std::unique_ptr<Item>>& GetItems() const { return mItems; }
    int GetItemQuantity(const std::string& itemName) const;

    bool HasItem(const std::string& itemName) const;

  private:
    std::vector<std::unique_ptr<Item>> mItems; 
};
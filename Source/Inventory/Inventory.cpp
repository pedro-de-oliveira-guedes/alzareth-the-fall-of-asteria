#include "Inventory.h"
#include <algorithm>

Inventory::Inventory() {}

Inventory::~Inventory() {}

void Inventory::AddItem(std::unique_ptr<Item> newItem){ 
  // for(auto& item_ptr : mItems){ 
  //   if(item_ptr->GetName() == newItem->GetName() && item_ptr->GetType() == newItem->GetType()){ 
  //     item_ptr->AddQuantity(newItem->GetQuantity()); 
  //     return;
  //   }
  // }

  mItems.push_back(std::move(newItem)); 
}

bool Inventory::RemoveItem(const std::string& itemName, int quantity){
  for(auto it = mItems.begin(); it != mItems.end(); ++it){
    if((*it)->GetName() == itemName){ 
      if((*it)->GetQuantity() > quantity){ 
        (*it)->RemoveQuantity(quantity); 
        if((*it)->GetQuantity() == 0){ 
          mItems.erase(it);
        }
        return true;
      }
      if ((*it)->GetQuantity() <= quantity) { 
          mItems.erase(it);
          return true;
      }
      return false;
    }
  }
  return false;
}

int Inventory::GetItemQuantity(const std::string& itemName) const {
  for (const auto& item_ptr : mItems) { 
    if (item_ptr->GetName() == itemName) { 
      return item_ptr->GetQuantity(); 
    }
  }
  return 0;
}

bool Inventory::HasItem(const std::string& itemName) const {
  for(const auto& item_ptr : mItems) { 
    if (item_ptr->GetName() == itemName) { 
      return true;
    }
  }
  return false;
}
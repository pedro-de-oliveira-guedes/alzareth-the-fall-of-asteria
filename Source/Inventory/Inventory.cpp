#include "Inventory.h"
#include <algorithm>

Inventory::Inventory() {
  mItems.resize(mMaxItems, nullptr);
}

Inventory::~Inventory() {
  mItems.clear();
}

int Inventory::ReturnWeaponIndex() {
  for (size_t i = 0; i < mItems.size(); ++i) {
    if (mItems[i] != nullptr && mItems[i]->GetType() == Item::ItemType::Weapon) {
      //SDL_Log("Índice da arma encontrada: %zu", i + 1);
      return i; // Retorna o índice da arma
    }
  }
  //SDL_Log("Nenhuma arma encontrada no inventário.");
  return -1; // Retorna -1 se nenhuma arma for encontrada
}

void Inventory::AddItem(Item* newItem) {
  if (newItem == nullptr) {
    return;
  }
  auto it = std::find(mItems.begin(), mItems.end(), nullptr);

  if (it != mItems.end()) {
    *it = newItem;
  }
  else {
    return;
  }
}

bool Inventory::RemoveItem(const std::string& itemName) {
  if (mItems.empty()) {
    return false;
  }

  for (size_t i = 0; i < mItems.size(); ++i) {
    if (mItems[i] != nullptr && mItems[i]->GetName() == itemName) {
      mItems[i] = nullptr; // Define o slot como vazio
      return true;
    }
  }
  return false;
}

bool Inventory::RemoveItemAtIndex(size_t index) {
    if (index >= mItems.size() || mItems[index] == nullptr) {
        return false;
    }

    Item* itemToRemove = mItems[index];
    mItems[index] = nullptr;
    return true;
}

Item* Inventory::GetItem(const std::string& itemName) const {
    for (Item* item : mItems) {
        if (item != nullptr && item->GetName() == itemName) {
            return item;
        }
    }
    return nullptr;
}

Item* Inventory::GetItemAtIndex(size_t index) const {
    if (index < mItems.size()) {
        return mItems[index]; 
    }
    return nullptr; 
}

bool Inventory::HasItem(const std::string& itemName) const {
  for (const auto& item_ptr : mItems) {
    if (item_ptr->GetName() == itemName) {
      return true;
    }
  }
  return false;
}
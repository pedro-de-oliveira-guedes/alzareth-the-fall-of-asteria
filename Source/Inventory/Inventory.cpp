#include "Inventory.h"
#include <algorithm>

Inventory::Inventory() {
  SDL_Log("Criando inventário com capacidade máxima de %d itens.", mMaxItems);
  mItems.resize(mMaxItems, nullptr);
}

Inventory::~Inventory() {
  mItems.clear();
}

void Inventory::AddItem(Item* newItem) {
  if (newItem == nullptr) {
    return;
  }
  auto it = std::find(mItems.begin(), mItems.end(), nullptr);

  if (it != mItems.end()) {
    *it = newItem;
    SDL_Log("Item '%s' adicionado ao inventário na posição %zu.", newItem->GetName().c_str(), std::distance(mItems.begin(), it) + 1);
  }
  else {
    SDL_Log("Inventário cheio, não é possível adicionar '%s'.", newItem->GetName().c_str());
    return;
  }
}

bool Inventory::RemoveItem(const std::string& itemName) {
  if (mItems.empty()) {
    SDL_Log("Inventory is empty, cannot remove items.");
    return false;
  }

  for (size_t i = 0; i < mItems.size(); ++i) {
    if (mItems[i] != nullptr && mItems[i]->GetName() == itemName) {
      mItems[i] = nullptr; // Define o slot como vazio
      SDL_Log("Item '%s' removido do inventário (slot %zu).", itemName.c_str(), i + 1);
      return true;
    }
  }
  return false;
}

bool Inventory::RemoveItemAtIndex(size_t index) {
    if (index >= mItems.size() || mItems[index] == nullptr) {
        SDL_Log("Nenhum item na posição %zu para remover.", index + 1);
        return false;
    }

    Item* itemToRemove = mItems[index];
    SDL_Log("Item '%s' removido completamente do inventário (slot %zu).", itemToRemove->GetName().c_str(), index + 1);
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
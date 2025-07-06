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
        if (mItems[i] != nullptr && 
            (mItems[i]->GetType() == Item::ItemType::Weapon || mItems[i]->GetType() == Item::ItemType::RangedWeapon)){
              return i;
        }
    }

    return -1;
}

void Inventory::AddItem(Item* newItem) {
    if (newItem == nullptr) {
        return;
    }

    auto it = std::find(mItems.begin(), mItems.end(), nullptr);
    if (it != mItems.end()) {
        *it = newItem;
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

    mItems[index] = nullptr;
    return true;
}

bool Inventory::InventoryFull() const {
    for (const auto& item_ptr : mItems) {
        if (item_ptr == nullptr) {
            return false;
        }
    }
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
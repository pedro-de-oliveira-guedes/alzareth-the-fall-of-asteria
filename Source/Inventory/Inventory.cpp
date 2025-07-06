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

Item* Inventory::AddItem(Item* newItem) {
    if (newItem == nullptr) {
        return nullptr;
    }

    Item* droppedItem = nullptr;

    // Se o novo item for uma arma (ou arma de longo alcance)
    if (newItem->GetType() == Item::ItemType::Weapon || newItem->GetType() == Item::ItemType::RangedWeapon) {
        // Verifica se o slot 0 já está ocupado
        if (mItems[0] != nullptr) {
            droppedItem = mItems[0]; // Armazena o item atual no slot 0 para ser dropado
            mItems[0] = nullptr;     // Limpa o slot
        }
        mItems[0] = newItem; // Coloca a nova arma no slot 0
        return droppedItem;  // Retorna o item que foi dropado (pode ser nullptr)
    } else { // Novo item NÃO é uma arma (consumível, etc.)
        // Tenta adicionar em qualquer slot EXCETO o slot 0
        size_t firstEmptySlot = mMaxItems; // Inicializa com um valor fora dos limites
        for (size_t i = 1; i < mMaxItems; ++i) { // Começa a procurar a partir do índice 1
            if (mItems[i] == nullptr) {
                firstEmptySlot = i;
                break;
            }
        }

        if (firstEmptySlot < mMaxItems) {
            mItems[firstEmptySlot] = newItem;
            return nullptr; // Nenhum item foi dropado
        } else {
            // Inventário cheio para itens não-arma (slots 1 a mMaxItems-1 estão cheios)
            return nullptr; // Nenhum item foi adicionado, nenhum item foi dropado
        }
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

Item* Inventory::RemoveItemAtIndex(size_t index) {
    if (index >= mItems.size() || mItems[index] == nullptr) {
        return nullptr; // Índice inválido ou slot vazio, nada para remover
    }

    Item* removedItem = mItems[index]; // Armazena o item a ser retornado
    mItems[index] = nullptr;           // Limpa o slot
    return removedItem;                // Retorna o item removido
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
#pragma once

#include "../DrawComponents/DrawComponent.h"
#include "../../UIElements/UIFont.h"
#include "../../UIElements/UIImage.h"
#include "../../Inventory/Inventory.h"
#include <vector>

class HUDComponent : public DrawComponent {
    public:
        explicit HUDComponent(Actor *owner, float maxHealth, float maxEnergy, int drawOrder = 101);

        void UpdateStats(float maxHealth, float currentHealth, float maxEnergy, float currentEnergy);
        void Update(float deltaTime) override;

        void UpdateInventoryDisplay();

        void Draw(SDL_Renderer *renderer) override;

    protected:
        void DrawHealthBar(SDL_Renderer *renderer)const;
        void DrawEnergyBar(SDL_Renderer *renderer)const;
        void DrawInventoryImage(SDL_Renderer *renderer)const;
        void DrawInventory(SDL_Renderer *renderer)const;
        void DrawEnemiesCount(SDL_Renderer *renderer)const;

        float mMaxHealth;
        float mCurrentHealth;
        float mMaxEnergy;
        float mCurrentEnergy;

        UIImage* mHealthBarOutlineImage;
        SDL_Color mHealthColor{};

        UIImage* mEnergyBarOutlineImage;
        SDL_Color mEnergyColor{};

        UIImage* mInventoryImage;
        std::vector<UIImage*> mInventoryItems;

        UIImage *mEnemiesCountImage;
        UIImage *mEnemiesBoardImage;
};

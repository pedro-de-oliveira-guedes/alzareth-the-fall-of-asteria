#pragma once

#include "../DrawComponents/DrawComponent.h"

class HUDComponent : public DrawComponent {
    public:
        explicit HUDComponent(Actor *owner, float maxHealth, float maxEnergy, int drawOrder = 101);

        void UpdateStats(float maxHealth, float currentHealth, float maxEnergy, float currentEnergy);

        void Draw(SDL_Renderer *renderer) override;

    protected:
        void DrawHealthBar(SDL_Renderer *renderer);
        void DrawEnergyBar(SDL_Renderer *renderer);

        float mMaxHealth;
        float mCurrentHealth;
        float mMaxEnergy;
        float mCurrentEnergy;

        SDL_Color mHealthColor;
        SDL_Color mEnergyColor;
        SDL_Color mBarBkgColor;

        SDL_Rect mHealthBarBkg;
        SDL_Rect mEnergyBarBkg;
};

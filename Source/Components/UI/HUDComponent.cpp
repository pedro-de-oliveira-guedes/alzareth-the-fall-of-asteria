#include "HUDComponent.h"

#include "../../Game.h"
#include "../../Actors/Actor.h"
#include "../../Utils/Math.h"

HUDComponent::HUDComponent(
    Actor *owner,
    const float maxHealth,
    const float maxEnergy,
    const int drawOrder
) : DrawComponent(owner, drawOrder) {
    mMaxHealth = maxHealth;
    mCurrentHealth = maxHealth;
    mMaxEnergy = maxEnergy;
    mCurrentEnergy = maxEnergy;

    mHealthColor = {255, 0, 0, 255}; // Red
    mEnergyColor = {255, 255, 0, 255}; // Yellow
    mBarBkgColor = {255, 255, 255, 255}; // Black

    mHealthBarBkg = {0, 0, 0, 0};
    mEnergyBarBkg = {0, 0, 0, 0};
}

void HUDComponent::UpdateStats(
    const float maxHealth,
    const float currentHealth,
    const float maxEnergy,
    const float currentEnergy
) {
    mMaxHealth = maxHealth;
    mCurrentHealth = currentHealth;
    mMaxEnergy = maxEnergy;
    mCurrentEnergy = currentEnergy;
}

void HUDComponent::DrawHealthBar(SDL_Renderer *renderer) {
    const int screenWidth = mOwner->GetGame()->GetWindowWidth();
    const int screenHeight = mOwner->GetGame()->GetWindowHeight();

    SDL_SetRenderDrawColor(renderer, mBarBkgColor.r, mBarBkgColor.g, mBarBkgColor.b, mBarBkgColor.a);
    mHealthBarBkg = {
        static_cast<int>(screenWidth * 0.05f),
        static_cast<int>(screenHeight * 0.05f),
        static_cast<int>(screenWidth * 0.15f),
        static_cast<int>(screenHeight * 0.02f)
    };
    SDL_RenderFillRect(renderer, &mHealthBarBkg);

    SDL_SetRenderDrawColor(renderer, mHealthColor.r, mHealthColor.g, mHealthColor.b, mHealthColor.a);
    const SDL_Rect healthBarRect = {
        static_cast<int>(mHealthBarBkg.x + mHealthBarBkg.x * 0.01f),
        static_cast<int>(mHealthBarBkg.y + mHealthBarBkg.y * 0.01f),
        static_cast<int>((mHealthBarBkg.w - mHealthBarBkg.w * 0.01f) * (mCurrentHealth / mMaxHealth)),
        static_cast<int>(mHealthBarBkg.h - mHealthBarBkg.h * 0.01f)
    };
    SDL_RenderFillRect(renderer, &healthBarRect);
}

void HUDComponent::DrawEnergyBar(SDL_Renderer *renderer) {
    const int healthBarBottom = mHealthBarBkg.y + mHealthBarBkg.h;
    const int screenHeight = mOwner->GetGame()->GetWindowHeight();

    SDL_SetRenderDrawColor(renderer, mBarBkgColor.r, mBarBkgColor.g, mBarBkgColor.b, mBarBkgColor.a);
    mEnergyBarBkg = {
        mHealthBarBkg.x,
        static_cast<int>(healthBarBottom + screenHeight * 0.02f),
        mHealthBarBkg.w,
        mHealthBarBkg.h
    };
    SDL_RenderFillRect(renderer, &mEnergyBarBkg);

    SDL_SetRenderDrawColor(renderer, mEnergyColor.r, mEnergyColor.g, mEnergyColor.b, mEnergyColor.a);
    const SDL_Rect energyBarRect = {
        static_cast<int>(mEnergyBarBkg.x + mEnergyBarBkg.x * 0.01f),
        static_cast<int>(mEnergyBarBkg.y + mEnergyBarBkg.y * 0.01f),
        static_cast<int>((mEnergyBarBkg.w - mEnergyBarBkg.w * 0.01f) * (mCurrentEnergy / mMaxEnergy)),
        static_cast<int>(mEnergyBarBkg.h - mEnergyBarBkg.h * 0.01f)
    };
    SDL_RenderFillRect(renderer, &energyBarRect);
}

void HUDComponent::Draw(SDL_Renderer *renderer) {
    DrawHealthBar(renderer);
    DrawEnergyBar(renderer);
}

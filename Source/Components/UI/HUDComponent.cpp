#include "HUDComponent.h"

#include "../../Game.h"
#include "../../Actors/Actor.h"
#include "../../Utils/Math.h"
#include "../../UIElements/UIImage.h" 

HUDComponent::HUDComponent(
    Actor* owner,
    const float maxHealth,
    const float maxEnergy,
    const int drawOrder
) : DrawComponent(owner, drawOrder) {
    mMaxHealth = maxHealth;
    mCurrentHealth = maxHealth;
    mMaxEnergy = maxEnergy;
    mCurrentEnergy = maxEnergy;

    mHealthColor = { 255, 0, 0, 255 }; // Red
    mEnergyColor = { 255, 255, 0, 255 }; // Yellow
    mBarBkgColor = { 255, 255, 255, 255 }; // Black

    mHealthBarBkg = { 0, 0, 0, 0 };
    mEnergyBarBkg = { 0, 0, 0, 0 };

    int windowHeight = mOwner->GetGame()->GetWindowHeight();

    mInventoryImage = new UIImage(
        "../Assets/Inventory/Inventory.png",
        Vector2(100.0f, static_cast<float>(windowHeight) - 144.0f - 40.0f),
        Vector2(300.0f, 244.0f));
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

void HUDComponent::Update(float deltaTime) {
    UpdateInventoryDisplay();
}

void HUDComponent::DrawHealthBar(SDL_Renderer* renderer) {
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

void HUDComponent::DrawEnergyBar(SDL_Renderer* renderer) {
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

void HUDComponent::DrawInventoryImage(SDL_Renderer* renderer) {
    mInventoryImage->Draw(renderer, Vector2::Zero);
}

void HUDComponent::DrawInventory(SDL_Renderer* renderer) {
    for (const auto& image : mInventoryItems) {
        if (image) {
            image->Draw(renderer, Vector2::Zero);
        }
    }
}

void HUDComponent::Draw(SDL_Renderer* renderer) {
    DrawHealthBar(renderer);
    DrawEnergyBar(renderer);
    DrawInventoryImage(renderer);
    DrawInventory(renderer);
}

void HUDComponent::UpdateInventoryDisplay() {
    for (auto image : mInventoryItems) {
        delete image;
    }
    mInventoryItems.clear();
    
    const Inventory& playerInventory = static_cast<Player*>(mOwner->GetGame()->GetPlayer())->GetInventory();
    
    float itemDisplayY = mInventoryImage->GetPosition().y + 95.0f; 

    // Espaçamento e tamanho dos itens
    float initialPaddingX = 18.0f; 
    float itemSpacingX = 10.5f; 
    float itemImageSize = 44.0f; 

    
    float currentX = mInventoryImage->GetPosition().x + initialPaddingX;
   
    int item_count = 0;
    for (const auto& item : playerInventory.GetItems()) { 

        Vector2 itemPos = Vector2(currentX, itemDisplayY);
        Vector2 itemSize = Vector2(itemImageSize, itemImageSize);

        UIImage* itemImage = new UIImage(item->GetTexturePath(),
            itemPos,
            itemSize);
        mInventoryItems.push_back(itemImage);

        currentX += itemImageSize + itemSpacingX; 
        
        item_count++;
    }
}

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

    const int windowHeight = mOwner->GetGame()->GetWindowHeight();

    mInventoryImage = new UIImage(
        "../Assets/Sprites/HUD/Inventory.png",
        Vector2(100.0f, static_cast<float>(windowHeight) - 144.0f - 40.0f),
        Vector2(300.0f, 244.0f)
    );

    mHealthBarOutlineImage = new UIImage("../Assets/Sprites/HUD/HealthBar.png", Vector2::Zero, Vector2::Zero);
    mEnergyBarOutlineImage = new UIImage("../Assets/Sprites/HUD/EnergyBar.png", Vector2::Zero, Vector2::Zero);

    mEnemiesCountImage = new UIImage("../Assets/Sprites/Golem/BaseGolem.png", Vector2::Zero, Vector2::Zero);
    mEnemiesBoardImage = new UIImage("../Assets/Sprites/HUD/EnemiesBoard.png", Vector2::Zero, Vector2::Zero);
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

void HUDComponent::DrawHealthBar(SDL_Renderer* renderer) const {
    const int screenWidth = mOwner->GetGame()->GetWindowWidth();
    const int screenHeight = mOwner->GetGame()->GetWindowHeight();

    mHealthBarOutlineImage->SetPosition(Vector2(
        static_cast<float>(screenWidth) * 0.03f,
        static_cast<float>(screenHeight) * 0.03f
    ));
    mHealthBarOutlineImage->SetSize(Vector2(
        static_cast<float>(screenWidth) * 0.25f,
        static_cast<float>(screenHeight) * 0.07f
    ));
    mHealthBarOutlineImage->Draw(renderer, Vector2::Zero);

    const Vector2 outlinePos = mHealthBarOutlineImage->GetPosition();
    const Vector2 outlineSize = mHealthBarOutlineImage->GetSize();

    SDL_SetRenderDrawColor(renderer, mHealthColor.r, mHealthColor.g, mHealthColor.b, mHealthColor.a);
    const SDL_Rect healthBarRect = {
        static_cast<int>(outlinePos.x + 0.23f * outlineSize.x),
        static_cast<int>(outlinePos.y + 0.32f * outlineSize.y),
        static_cast<int>((outlineSize.x - 0.26f * outlineSize.x) * (mCurrentHealth / mMaxHealth)),
        static_cast<int>(outlineSize.y - 2*(0.32f * outlineSize.y))
    };
    SDL_RenderFillRect(renderer, &healthBarRect);
}

void HUDComponent::DrawEnergyBar(SDL_Renderer* renderer) const {
    const int healthBarBottom = mHealthBarOutlineImage->GetPosition().y + mHealthBarOutlineImage->GetSize().y;

    mEnergyBarOutlineImage->SetPosition(Vector2(
        mHealthBarOutlineImage->GetPosition().x,
        static_cast<float>(healthBarBottom + 10)
    ));
    mEnergyBarOutlineImage->SetSize(Vector2(
        mHealthBarOutlineImage->GetSize().x,
        mHealthBarOutlineImage->GetSize().y
    ));
    mEnergyBarOutlineImage->Draw(renderer, Vector2::Zero);

    const Vector2 outlinePos = mEnergyBarOutlineImage->GetPosition();
    const Vector2 outlineSize = mEnergyBarOutlineImage->GetSize();

    SDL_SetRenderDrawColor(renderer, mEnergyColor.r, mEnergyColor.g, mEnergyColor.b, mEnergyColor.a);
    const SDL_Rect energyBarRect = {
        static_cast<int>(outlinePos.x + 0.23f * outlineSize.x),
        static_cast<int>(outlinePos.y + 0.32f * outlineSize.y),
        static_cast<int>((outlineSize.x - 0.26f * outlineSize.x) * (mCurrentEnergy / mMaxEnergy)),
        static_cast<int>(outlineSize.y - 2*(0.32f * outlineSize.y))
    };
    SDL_RenderFillRect(renderer, &energyBarRect);
}

void HUDComponent::DrawInventoryImage(SDL_Renderer* renderer) const {
    mInventoryImage->Draw(renderer, Vector2::Zero);
}

void HUDComponent::DrawInventory(SDL_Renderer* renderer) const {
    for (const auto& image : mInventoryItems) {
        if (image) {
            image->Draw(renderer, Vector2::Zero);
        }
    }
}

void HUDComponent::DrawEnemiesCount(SDL_Renderer* renderer) const {
    const int screenWidth = mOwner->GetGame()->GetWindowWidth();

    mEnemiesBoardImage->SetPosition(Vector2(screenWidth - 180.0f, 20.0f));
    mEnemiesBoardImage->SetSize(Vector2(175.0f, 60.0f));
    mEnemiesBoardImage->Draw(renderer, Vector2::Zero);

    mEnemiesCountImage->SetPosition(Vector2(screenWidth - 165.0f, 30.0f));
    mEnemiesCountImage->SetSize(Vector2(Game::TILE_SIZE, Game::TILE_SIZE));
    mEnemiesCountImage->Draw(renderer, Vector2::Zero);

    const auto [defeated, total] = mOwner->GetGame()->GetEnemiesCount();
    UIFont *font = mOwner->GetGame()->LoadFont("../Assets/Fonts/PixelifySans.ttf");

    std::string text;
    if (std::to_string(defeated).size() == std::to_string(total).size()) {
        text = std::to_string(defeated) + " / " + std::to_string(total);
    } else {
        text = "0" + std::to_string(defeated) + " / " + std::to_string(total);
    }

    const auto enemiesCountText = new UIText(
        text,
        font,
        24,
        0,
        Vector2(screenWidth - 165.0f + Game::TILE_SIZE + 10.0f, 35.0f),
        Vector2(text.size() * 24.f * 0.6f, 24.f)
    );
    enemiesCountText->Draw(renderer, Vector2::Zero);
}

void HUDComponent::Draw(SDL_Renderer* renderer) {
    DrawHealthBar(renderer);
    DrawEnergyBar(renderer);

    DrawInventoryImage(renderer);
    DrawInventory(renderer);

    DrawEnemiesCount(renderer);
}

void HUDComponent::UpdateInventoryDisplay() {
    for (auto image : mInventoryItems) {
        if (image) { 
            delete image;
        }
    }
    mInventoryItems.clear();

    const Inventory& playerInventory = mOwner->GetGame()->GetPlayer()->GetInventory();

    float itemDisplayY = mInventoryImage->GetPosition().y + 95.0f;

    float initialPaddingX = 18.0f;
    float itemSpacingX = 10.5f;
    float itemImageSize = 44.0f;

    float currentX = mInventoryImage->GetPosition().x + initialPaddingX;
    const auto& inventorySlots = playerInventory.GetItems(); 

    for (const auto& item_ptr : inventorySlots) { 
        if (item_ptr != nullptr) { 
            Vector2 itemPos = Vector2(currentX, itemDisplayY);
            Vector2 itemSize = Vector2(itemImageSize, itemImageSize);

            UIImage* itemImage = new UIImage(
                item_ptr->GetInventoryTexturePath(), 
                itemPos,
                itemSize
            );
            mInventoryItems.push_back(itemImage);

            currentX += itemImageSize + itemSpacingX;
        } else {
            currentX += itemImageSize + itemSpacingX; 
        }
    }
}

#include "HUDComponent.h"

#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Systems/SceneManager/SceneManagerSystem.h"
#include "../../UIElements/UIImage.h"
#include "../../Utils/Math.h"

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
    mBossColor = { 0, 0, 255, 255 }; // Blue

    const int windowWidth = mOwner->GetGame()->GetWindowWidth();
    const int windowHeight = mOwner->GetGame()->GetWindowHeight();

    mInventoryImage = new UIImage(
        "../Assets/Sprites/HUD/Inventory.png",
        Vector2(static_cast<float>(windowWidth) - 300.0f - 40.0f, -40.f),
        Vector2(300.0f, 244.0f)
    );

    mHealthBarOutlineImage = new UIImage("../Assets/Sprites/HUD/HealthBar.png", Vector2::Zero, Vector2::Zero);
    mEnergyBarOutlineImage = new UIImage("../Assets/Sprites/HUD/EnergyBar.png", Vector2::Zero, Vector2::Zero);

    mBossHealthBarOutlineImage = new UIImage(
        "../Assets/Sprites/HUD/BossHealthBar.png",
        Vector2((windowWidth - 900) / 2, windowHeight - 40.f - 20.f),
        Vector2(900.0f, 40.0f)
    );
    mBossHealthBarStagesTicksImage = new UIImage(
        "../Assets/Sprites/HUD/BossHealthBarTicks.png",
        Vector2((windowWidth - 900) / 2, windowHeight - 40.f - 20.f),
        Vector2(900.0f, 40.0f)
    );
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

void HUDComponent::DrawBossHealthBar(SDL_Renderer *renderer) const {
    mBossHealthBarOutlineImage->Draw(renderer, Vector2::Zero);

    const auto [currentHealth, maxHealth] = mOwner->GetGame()->GetSceneManager()->GetBossHealth();
    const float healthPercentage = static_cast<float>(currentHealth) / maxHealth;
    const Vector2 outlinePos = mBossHealthBarOutlineImage->GetPosition();
    const Vector2 outlineSize = mBossHealthBarOutlineImage->GetSize();
    const SDL_Rect healthBarRect = {
        static_cast<int>(outlinePos.x + 10.f),
        static_cast<int>(outlinePos.y + 7.f),
        static_cast<int>((outlineSize.x - 20.f) * healthPercentage),
        static_cast<int>(outlineSize.y - 14.f)
    };
    SDL_SetRenderDrawColor(renderer, mBossColor.r, mBossColor.g, mBossColor.b, mBossColor.a);
    SDL_RenderFillRect(renderer, &healthBarRect);

    mBossHealthBarStagesTicksImage->Draw(renderer, Vector2::Zero);
}

void HUDComponent::Draw(SDL_Renderer* renderer) {
    DrawHealthBar(renderer);
    DrawEnergyBar(renderer);

    DrawInventoryImage(renderer);
    DrawInventory(renderer);

    DrawBossHealthBar(renderer);
}

void HUDComponent::UpdateInventoryDisplay() {
    for (auto image : mInventoryItems) {
        if (image) { 
            delete image;
        }
    }
    mInventoryItems.clear();

    const Inventory& playerInventory = mOwner->GetGame()->GetPlayer()->GetInventory();

    float itemDisplayY = mInventoryImage->GetPosition().y + 99.0f;

    float initialPaddingX = 39.0f;
    float itemSpacingX = 0.01f;
    // float itemImageSize = 35.0f; // Esta variável será definida dinamicamente agora

    float largeItemSpacingX = 48.0f; 

    float currentX = mInventoryImage->GetPosition().x + initialPaddingX;
    const auto& inventorySlots = playerInventory.GetItems(); 

    int itemIndex = 0; 

    for (const auto& item_ptr : inventorySlots) { 
        float currentItemImageSize; // Declaração da variável para o tamanho atual da imagem

        // Define o tamanho da imagem com base no índice do item
        if (itemIndex == 0) {
            currentItemImageSize = 36.0f; // Tamanho para o primeiro item
        } else {
            currentItemImageSize = 34.0f; // Tamanho para os outros itens
        }

        if (item_ptr != nullptr) { 
            Vector2 itemPos = Vector2(currentX, itemDisplayY);
            Vector2 itemSize = Vector2(currentItemImageSize, currentItemImageSize); // Usa o tamanho definido condicionalmente

            UIImage* itemImage = new UIImage(
                item_ptr->GetInventoryTexturePath(), 
                itemPos,
                itemSize
            );
            mInventoryItems.push_back(itemImage);
        }
        
        // Atualiza a posição X usando o tamanho da imagem atual para o espaçamento
        if (itemIndex == 0) {
            currentX += currentItemImageSize + largeItemSpacingX; 
        } else {
            currentX += currentItemImageSize + itemSpacingX; 
        }
        itemIndex++; 
    }
}

#include "MagicToken.h"
#include "../../../../Game.h"
#include "../../../Projectile/BallAttackToken/BallAttackToken.h" 
#include "../../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../../../Actors/Player/Player.h"

MagicToken::MagicToken(Game* game,
                       const std::string& name,
                       const std::string& texturePath,
                       const std::string& textureInventoryPath,
                       const std::string& spriteSheetData,
                       const Vector2& position,
                       int quantity)
    : Item(game, name, ItemType::RangedWeapon, texturePath, textureInventoryPath, spriteSheetData, quantity) { 

    mDamage = 550; 
    mEnergyCost = 5; 

    SetPosition(position);

    int tokenSpriteSize = 50;
    int colliderSize = 40; 

    int offsetX = (tokenSpriteSize - colliderSize) / 2;
    int offsetY = (tokenSpriteSize - colliderSize) / 2;

    mColliderComponent = new AABBColliderComponent(
        this,
        offsetX,
        offsetY,
        colliderSize,
        colliderSize,
        ColliderLayer::Collectible, 
        false,
        true
    );
    mDrawComponent = new DrawAnimatedComponent(
        this,
        texturePath,
        spriteSheetData,
        100 // drawOrder
    );

    mDrawComponent->AddAnimation("default", {0}); 
    mDrawComponent->SetAnimation("default");
    
    mDrawComponent->SetIsVisible(true);
    mDrawComponent->SetIsPaused(true);
    mDrawComponent->SetLooping(false);
    mGame->SetMagicTokenInWorld(true);
}

MagicToken::~MagicToken() {}

void MagicToken::Use(Player* player) {
    if (player->GetCurrentEnergy() >= mEnergyCost) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        Vector2 mousePos(static_cast<float>(mouseX) + mGame->GetCameraPos().x, static_cast<float>(mouseY) + mGame->GetCameraPos().y);

        Vector2 playerPos = player->GetPosition();
        Vector2 direction = mousePos - playerPos;
        direction.Normalize();

        Vector2 projectileStartPos = playerPos + direction * 50.0f; 

        float projectileSpeed = 500.0f;
        new BallAttackToken(mGame, projectileStartPos, direction * projectileSpeed, static_cast<float>(mDamage)); 

        player->SetCurrentEnergy(player->GetCurrentEnergy() - static_cast<float>(mEnergyCost)); 
        // mGame->GetAudioSystem()->PlaySound("magic_token_shoot.wav", false); 
    } else {
        mGame->GetAudioSystem()->PlaySound("menu_click.ogg", false); 
    }
}

void MagicToken::Collect() {
    mDrawComponent->SetIsVisible(false);
    mColliderComponent->SetEnabled(false);
}
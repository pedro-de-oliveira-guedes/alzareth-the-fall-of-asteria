// Conteúdo do arquivo: Source/Actors/Items/Weapons/Melee/Sword.cpp
#include "Sword.h"
#include "../../../Player/Player.h"

// Remova as definições de EAST, SOUTH_EAST, etc., se elas só forem usadas para definir animações que não serão mais usadas para a espada de ataque.
// Se elas forem usadas para UpdateSwordPosition, mantenha-as ou defina-as globalmente.
// Para este exemplo, vou mantê-las aqui se existirem, mas o SetAnimation(newDirection) será removido.
const std::string EAST = "east";
const std::string SOUTH_EAST = "south_east";
const std::string SOUTH = "south";
const std::string SOUTH_WEST = "south_west";
const std::string WEST = "west";
const std::string NORTH_WEST = "north_west";
const std::string NORTH = "north";
const std::string NORTH_EAST = "north_east";

Sword::Sword(Game* game, const std::string& name,
               const std::string& texturePath, const std::string& textureInventoryPath, const std::string& spriteSheetData, const Vector2& position, int quantity)
    : Item(game, name, ItemType::Weapon, texturePath, textureInventoryPath, spriteSheetData, quantity) {

    mDamage = 100;

    SetPosition(position);

    int swordSpriteSize = 100;
    
    int colliderSize = 60; // About 60% of sprite size
    
    int offsetX = (swordSpriteSize - colliderSize) / 2;
    int offsetY = (swordSpriteSize - colliderSize) / 2;

    mColliderComponent = new AABBColliderComponent(
        this,
        offsetX,
        offsetY,
        colliderSize,
        colliderSize,
        ColliderLayer::MeleeWeapon,
        false,
        true
    );
    mDrawComponent = new DrawAnimatedComponent(
        this,
        texturePath,
        spriteSheetData,
        100 // drawOrder
    );

    // Adicione a animação de ataque com todos os 10 frames
    mDrawComponent->AddAnimation(ATTACK_SWORD_ANIMATION, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});     
    
    mDrawComponent->SetIsVisible(true);
    mDrawComponent->SetIsPaused(true);
    mDrawComponent->SetLooping(false); 

    mDrawComponent->SetAnimation(ATTACK_SWORD_ANIMATION);
    mDrawComponent->SetAnimTimer(0.0f);
    
    // mTimeAttack = 1.0f; // Remova esta linha
}

Sword::~Sword() {}

void Sword::Use(Player* player) {}

void Sword::OnCollision(float minOverlap, AABBColliderComponent *other) { }

int Sword::GetDamage() const {
    if (mHasHitThisAttack) {
        return 0;
    }
    return mDamage;
}

void Sword::OnUpdate(float deltaTime) {
    // Remova a lógica do mTimeAttack
    // if (mIsAttacking) {
    //     mTimeAttack -= deltaTime;
    //     if (mTimeAttack <= 0.0f) {
    //         mDrawComponent->SetIsVisible(false);
    //         mIsAttacking = false;
    //         mHasHitThisAttack = false;
    //         mDrawComponent->SetIsPaused(true);
    //     }
    // }
    
    // Atualiza o componente de desenho (incluindo o temporizador da animação)
    mDrawComponent->Update(deltaTime);

    // Verifica se a animação terminou e pausa
    if (mIsAttacking && mDrawComponent->GetIsPaused()) {
        mDrawComponent->SetIsVisible(false);
        mIsAttacking = false;
        mHasHitThisAttack = false;
    }
}

void Sword::DrawForAttack() {
    mIsAttacking = true;
    mDrawComponent->SetIsVisible(true);
    mDrawComponent->SetAnimation(ATTACK_SWORD_ANIMATION); // Define a animação de ataque
    mDrawComponent->SetAnimFPS(35.0f); // Ajuste o FPS para uma animação mais suave ou rápida/lenta
    mDrawComponent->SetIsPaused(false); // Garante que a animação não está pausada
    mDrawComponent->SetAnimTimer(0.0f); // Reinicia a animação do início
    mDrawComponent->SetLooping(false); // Define para não repetir
    // mTimeAttack = 1.0f; // Remova esta linha
    UpdateDirection(); // Isso ainda orientará a espada e definirá sua posição
    // mTimeAttack já foi definido no construtor e será gerenciado por OnUpdate
}

void Sword::Collect() {
    mDrawComponent->SetIsVisible(false);
    mColliderComponent->SetEnabled(false);
}

void Sword::SetRotation(float angle) {
    mRotation = angle;
    if (mDrawComponent) {
        mDrawComponent->SetRotation(angle);
    }
}
void Sword::UpdateDirection() {
    Vector2 direction = mMousePos - mPlayerPos;
    
    // get angle in radians using Atan2
    float angle = Math::Atan2(direction.y, direction.x);
    
    // convert to degrees
    float degrees = Math::ToDegrees(angle);
    
    // normalize degrees to 0-360 range
    if (degrees < 0) {
        degrees += 360.0f;
    }
    
    std::string newDirection; // Esta variável ainda é usada para UpdateSwordPosition
    
    if (degrees >= 337.5f || degrees < 22.5f) {
        newDirection = EAST;           
    } else if (degrees >= 22.5f && degrees < 67.5f) {
        newDirection = SOUTH_EAST;     
    } else if (degrees >= 67.5f && degrees < 112.5f) {
        newDirection = SOUTH;          
    } else if (degrees >= 112.5f && degrees < 157.5f) {
        newDirection = SOUTH_WEST;     
    } else if (degrees >= 157.5f && degrees < 202.5f) {
        newDirection = WEST;           
    } else if (degrees >= 202.5f && degrees < 247.5f) {
        newDirection = NORTH_WEST;     
    } else if (degrees >= 247.5f && degrees < 292.5f) {
        newDirection = NORTH;          
    } else if (degrees >= 292.5f && degrees < 337.5f) {
        newDirection = NORTH_EAST;     
    }

    // Define a rotação do componente de desenho para o ângulo calculado
    if (mDrawComponent) {
        mDrawComponent->SetRotation(degrees);
    }

    // Remova a linha abaixo, pois a animação agora é "attack" e não depende da direção
    // mDrawComponent->SetAnimation(newDirection); 
    
    UpdateSwordPosition(newDirection);
}

void Sword::UpdateSwordPosition(const std::string& direction) {
    float offsetDistance = 50.0f; // this variable is used to change how far the sword appears from player
    
    Vector2 swordOffset;
    
    if (direction == EAST) {
        swordOffset = Vector2(offsetDistance, 0);
    } else if (direction == SOUTH_EAST) {
        swordOffset = Vector2(offsetDistance * 0.707f, offsetDistance * 0.707f); // cos(45°) = sin(45°) = 0.707
    } else if (direction == SOUTH) {
        swordOffset = Vector2(0, offsetDistance);
    } else if (direction == SOUTH_WEST) {
        swordOffset = Vector2(-offsetDistance * 0.707f, offsetDistance * 0.707f);
    } else if (direction == WEST) {
        swordOffset = Vector2(-offsetDistance, 0);
    } else if (direction == NORTH_WEST) {
        swordOffset = Vector2(-offsetDistance * 0.707f, -offsetDistance * 0.707f);
    } else if (direction == NORTH) {
        swordOffset = Vector2(0, -offsetDistance);
    } else if (direction == NORTH_EAST) {
        swordOffset = Vector2(offsetDistance * 0.707f, -offsetDistance * 0.707f);
    }
    
    SetPosition(mPlayerPos + swordOffset);
    this->SetPosition(mPosition);
}
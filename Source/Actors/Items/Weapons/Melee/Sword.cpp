#include "Sword.h"
#include "../../../Player/Player.h"

const std::string SOUTH = "south";
const std::string EAST = "east";
const std::string SOUTH_EAST = "southEast";
const std::string SOUTH_WEST = "southWest";
const std::string NORTH = "north";
const std::string NORTH_EAST = "northEast";
const std::string NORTH_WEST = "northWest";
const std::string WEST = "west";

Sword::Sword(Game* game, const std::string& name,
               const std::string& texturePath, const std::string& textureInventoryPath, const std::string& spriteSheetData, const Vector2& position, int quantity)
    : Item(game, name, ItemType::Weapon, texturePath, textureInventoryPath, spriteSheetData, quantity) {

    mDamage = 100;

    mPosition = position;

    int swordSpriteSize = 100;
    
    int colliderSize = 60; // About 60% of sprite size
    
    int offsetX = (swordSpriteSize - colliderSize) / 2;
    int offsetY = (swordSpriteSize - colliderSize) / 2;

    mColliderComponent = new AABBColliderComponent(this, offsetX, offsetY, colliderSize, colliderSize, ColliderLayer::MeleeWeapon, false, true);


    mDrawComponent = new DrawAnimatedComponent(
        this,
        texturePath,
        spriteSheetData, 
        100 // drawOrder
    );

    mDrawComponent->AddAnimation(SOUTH, {0});
    mDrawComponent->AddAnimation(EAST, {1});
    mDrawComponent->AddAnimation(SOUTH_EAST, {7});
    mDrawComponent->AddAnimation(SOUTH_WEST, {2});
    mDrawComponent->AddAnimation(NORTH, {3});
    mDrawComponent->AddAnimation(NORTH_EAST, {5});
    mDrawComponent->AddAnimation(NORTH_WEST, {4});
    mDrawComponent->AddAnimation(WEST, {6});
    mDrawComponent->SetAnimation(SOUTH);
    mDrawComponent->SetAnimFPS(10.0f);
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

    if (mIsAttacking) {
        mTimeAttack -= deltaTime;
        DrawForAttack();
    }

    // Update the draw component
    mDrawComponent->Update(deltaTime);
}

void Sword::DrawForAttack() {

    mIsAttacking = true;
    mDrawComponent->SetIsVisible(true);
    UpdateDirection();

    if (mTimeAttack < 0.0f) {
        mDrawComponent->SetIsVisible(false);
        mTimeAttack = 1.0f; // Reset the attack time
        mIsAttacking = false;
        mHasHitThisAttack = false;
    }
    
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
    
    std::string newDirection;
    
    if (degrees >= 337.5f || degrees < 22.5f) {
        newDirection = EAST;           // frame 1
    } else if (degrees >= 22.5f && degrees < 67.5f) {
        newDirection = SOUTH_EAST;     // frame 7
    } else if (degrees >= 67.5f && degrees < 112.5f) {
        newDirection = SOUTH;          // frame 0
    } else if (degrees >= 112.5f && degrees < 157.5f) {
        newDirection = SOUTH_WEST;     // frame 2
    } else if (degrees >= 157.5f && degrees < 202.5f) {
        newDirection = WEST;           // frame 6
    } else if (degrees >= 202.5f && degrees < 247.5f) {
        newDirection = NORTH_WEST;     // frame 4
    } else if (degrees >= 247.5f && degrees < 292.5f) {
        newDirection = NORTH;          // frame 3
    } else if (degrees >= 292.5f && degrees < 337.5f) {
        newDirection = NORTH_EAST;     // frame 5
    }

    //SDL_Log("Degrees: %f", degrees);
    //SDL_Log("New direction: %s", newDirection.c_str());

    mDrawComponent->SetAnimation(newDirection);
    
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
    
    mPosition = mPlayerPos + swordOffset;
    this->SetPosition(mPosition);

}

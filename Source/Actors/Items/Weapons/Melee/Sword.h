#pragma once

#include "../../Item.h"
#include "../../../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../../../Components/DrawComponents/DrawSpriteComponent.h"
#include "../../../../Game.h"


class Sword : public Item {
public:
    Sword(class Game* game, const std::string& name,
           const std::string& texturePath, const std::string& textureInventoryPath, const std::string& spriteSheetData, const Vector2& position, int quantity = 1  );
    ~Sword() override;

    int GetDamage() const { return mDamage; }
    void SetDamage(int damage) { mDamage = damage; }

    float GetRangeX() const { return mRangeX; }
    float GetRangeY() const { return mRangeY; }
   
    void Use(class Player* player) override;

    void OnCollision(float minOverlap, AABBColliderComponent *other) override;

    void Collect();

    int GetEnergyCost() const { return mEnergyCost; }

    void DrawForAttack();

    void OnUpdate(float deltaTime) override;

    void SetRotation(float angle) ;


protected:
    int mDamage;
    int mRangeY = 100;
    int mRangeX = 100;

    int mEnergyCost =10;

    float mTimeAttack = 1.0f;
    bool mIsAttacking = false;

    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;  
};

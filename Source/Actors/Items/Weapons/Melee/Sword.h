#pragma once

#include "../../Item.h"
#include "../../../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../../../Components/DrawComponents/DrawSpriteComponent.h"
#include "../../../../Game.h"


class Sword : public Item {
public:
    Sword(
        class Game* game,
        const std::string& name,
        const std::string& texturePath,
        const std::string& textureInventoryPath,
        const std::string& spriteSheetData,
        const Vector2& position,
        int quantity = 1
    );
    ~Sword() override;

    int GetDamage() const;
    void SetDamage(int damage) { mDamage = damage; }

    float GetRangeX() const { return mRangeX; }
    float GetRangeY() const { return mRangeY; }
   
    void Use(class Player* player) override;

    void OnCollision(float minOverlap, AABBColliderComponent *other) override;

    void Collect() override;

    int GetEnergyCost() const { return mEnergyCost; }

    void DrawForAttack();

    void OnUpdate(float deltaTime) override;

    void SetRotation(float angle) ;

    void UpdateDirection();
    void UpdateSwordPosition(const std::string& direction);

    void SetPlayerPos(const Vector2& playerPos) { mPlayerPos = playerPos; }
    void SetMousePos(const Vector2& mousePos) { mMousePos = mousePos; }

    bool GetHasHitThisAttack() const { return mHasHitThisAttack; }
    void SetHasHitThisAttack(bool hasHitThisAttack) { mHasHitThisAttack = hasHitThisAttack; }


protected:
    int mDamage;
    int mRangeY = 100;
    int mRangeX = 100;
    std::string mCurrentDirection;

    int mEnergyCost =10;

    float mTimeAttack = 0.3f;
    bool mIsAttacking = false;

    Vector2 mPlayerPos;
    Vector2 mMousePos;

    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;  

    bool mHasHitThisAttack = false;
};

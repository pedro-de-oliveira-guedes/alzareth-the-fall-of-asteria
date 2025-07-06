#pragma once

#include "../../Item.h"
#include "../../../../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../../../../Components/ColliderComponents/AABBColliderComponent.h"
#include "../../../../Utils/Math.h"

class MagicToken : public Item {
public:
    MagicToken(
        class Game* game,
        const std::string& name,
        const std::string& texturePath,
        const std::string& textureInventoryPath,
        const std::string& spriteSheetData,
        const Vector2& position,
        int quantity = 1
    );
    ~MagicToken() override;

    void Use(class Player* player) override;
    void Collect() override;

    int GetDamage() const { return mDamage; }
    int GetEnergyCost() const { return mEnergyCost; }

protected:
    int mDamage;
    int mEnergyCost;

    DrawAnimatedComponent* mDrawComponent;
    AABBColliderComponent* mColliderComponent;
};
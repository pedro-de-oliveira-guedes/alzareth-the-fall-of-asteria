#pragma once
#include "../Enemy.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

class Golem : public Enemy {
    public:
        explicit Golem(
            Game *game
        );


        void Attack();

        void OnUpdate(float deltaTime) override;

        void OnCollision(float minOverlap, AABBColliderComponent *other) override;

        void Kill() override;
        float GetDamageAttack() override { return mDamageAttack; }


    private:

        // Update handling

        void ManageAnimations() const;
        

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
};

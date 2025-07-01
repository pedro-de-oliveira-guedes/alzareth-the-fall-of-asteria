#pragma once
#include "../Enemy.h"
#include "../../Components/DrawComponents/DrawAnimatedComponent.h"

class Golem : public Enemy {
    public:
        explicit Golem(
            Game *game,
            Vector2 position
        );


        void Attack();

        void OnUpdate(float deltaTime) override;

        void OnCollision(float minOverlap, AABBColliderComponent *other) override;

        void Kill() override;

        void DebugColliderPosition() const;
    private:

        // Update handling

        void ManageAnimations() const;
        

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;
};

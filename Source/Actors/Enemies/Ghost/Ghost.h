#pragma once
#include "../Enemy.h"
#include "../../../Components/DrawComponents/DrawAnimatedComponent.h"

class Ghost : public Enemy {
    public:
        explicit Ghost(
            Game *game,
            Vector2 position
        );

        void Attack();

        void OnUpdate(float deltaTime) override;

        void OnCollision(float minOverlap, AABBColliderComponent *other) override;

        void Kill() override;
    private:
        void ManageAnimations() const;

        RigidBodyComponent *mRigidBodyComponent;
        AABBColliderComponent *mColliderComponent;
        DrawAnimatedComponent *mDrawComponent;

        SoundHandle mAttackSound;
};

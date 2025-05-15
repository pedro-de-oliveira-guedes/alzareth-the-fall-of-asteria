#pragma once
#include "Component.h"
#include "../Math.h"

class RigidBodyComponent : public Component {
    constexpr float MAX_SPEED_X = 750.0f;
    constexpr float MAX_SPEED_Y = 750.0f;
    constexpr float GRAVITY = 2000.0f;

    public:
        // Lower update order to update first
        RigidBodyComponent(
            class Actor *owner,
            float mass = 1.0f,
            float friction = 0.0f,
            bool applyGravity = true,
            int updateOrder = 10
        );

        void Update(float deltaTime) override;

        const Vector2& GetVelocity() const { return mVelocity; }
        void SetVelocity(const Vector2 &velocity) { mVelocity = velocity; }

        const Vector2& GetAcceleration() const { return mAcceleration; }
        void SetAcceleration(const Vector2 &acceleration) { mAcceleration = acceleration; }

        void SetApplyGravity(const bool applyGravity) { mApplyGravity = applyGravity; }

        void ApplyForce(const Vector2 &force);

    private:
        bool mApplyGravity;

        // Physical properties
        float mFrictionCoefficient;
        float mMass;
        float m;

        Vector2 mVelocity;
        Vector2 mAcceleration;
};

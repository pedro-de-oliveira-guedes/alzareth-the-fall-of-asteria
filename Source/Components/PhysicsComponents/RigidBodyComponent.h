#pragma once

#include "../Component.h"
#include "../../Utils/Math.h"

class RigidBodyComponent : public Component {
    public:
        // Lower update order to update first
        RigidBodyComponent(
            Actor *owner,
            float mass = 1.0f,
            float friction = 0.0f,
            int updateOrder = 10
        );

        void Update(float deltaTime) override;

        const Vector2& GetVelocity() const { return mVelocity; }
        void SetVelocity(const Vector2 &velocity) { mVelocity = velocity; }

        const Vector2& GetAcceleration() const { return mAcceleration; }
        void SetAcceleration(const Vector2 &acceleration) { mAcceleration = acceleration; }


        void ApplyForce(const Vector2 &force);

    private:
        // Physical properties
        float mFrictionCoefficient;
        float mMass;

        Vector2 mVelocity;
        Vector2 mAcceleration;
};

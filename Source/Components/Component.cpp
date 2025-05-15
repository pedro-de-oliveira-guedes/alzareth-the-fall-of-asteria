#include "Component.h"
#include "../Actors/Actor.h"

Component::Component(Actor *owner, const int updateOrder) {
    mOwner = owner;
    mUpdateOrder = updateOrder;
    mIsEnabled = true;

    // Add to actor's vector of components
    mOwner->AddComponent(this);
}

Component::~Component() {}

void Component::Update(float deltaTime) {}

void Component::ProcessInput(const Uint8 *keyState) {}

Game* Component::GetGame() const {
    return mOwner->GetGame();
}



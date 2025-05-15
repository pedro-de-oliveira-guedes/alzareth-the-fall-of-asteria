#include "Actor.h"
#include "../Game.h"
#include "../Components/Component.h"
#include <algorithm>

Actor::Actor(Game *game) {
    mState = ActorState::Active;
    mPosition = Vector2::Zero;
    mScale = 1.0f;
    mRotation = 0.0f;
    mGame = game;
    mGame->AddActor(this);
}

Actor::~Actor() {
    mGame->RemoveActor(this);

    for (const auto component : mComponents)
        delete component;

    mComponents.clear();
}

void Actor::Update(const float deltaTime) {
    if (mState == ActorState::Active) {
        for (auto comp : mComponents) {
            if(comp->IsEnabled())
                comp->Update(deltaTime);
        }

        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime) {}

void Actor::OnCollision(const float minOverlap, AABBColliderComponent *other) {}

void Actor::Kill() {}

void Actor::ProcessInput(const Uint8 *keyState) {
    if (mState == ActorState::Active) {
        for (const auto comp : mComponents)
            comp->ProcessInput(keyState);

        OnProcessInput(keyState);
    }
}

void Actor::OnProcessInput(const Uint8 *keyState) {}

void Actor::AddComponent(Component *c) {
    mComponents.emplace_back(c);
    std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
        return a->GetUpdateOrder() < b->GetUpdateOrder();
    });
}

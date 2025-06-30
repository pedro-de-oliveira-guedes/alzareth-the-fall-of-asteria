#include "DrawComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawComponent::DrawComponent(Actor *owner, const int drawOrder) : Component(owner) {
    mDrawOrder = drawOrder;
    mIsVisible = true;
}

DrawComponent::~DrawComponent() {}


void DrawComponent::Draw(SDL_Renderer *renderer) {}
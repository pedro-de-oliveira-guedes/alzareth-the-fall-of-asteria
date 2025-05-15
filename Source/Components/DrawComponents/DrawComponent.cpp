#include "DrawComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawComponent::DrawComponent(Actor *owner, const int drawOrder) : Component(owner) {
    mDrawOrder = drawOrder;
    mIsVisible = true;

    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent() {
    mOwner->GetGame()->RemoveDrawable(this);
}


void DrawComponent::Draw(SDL_Renderer *renderer) {}
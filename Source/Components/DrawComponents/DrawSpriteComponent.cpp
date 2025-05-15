//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"


DrawSpriteComponent::DrawSpriteComponent(
    Actor *owner,
    const std::string &texturePath,
    const int width,
    const int height,
    const int drawOrder
) : DrawComponent(owner, drawOrder) {
    mWidth = width;
    mHeight = height;

    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);
}

void DrawSpriteComponent::Draw(SDL_Renderer *renderer) {
    const Vector2 &cameraPos = mOwner->GetGame()->GetCameraPos();
    const Vector2 &ownerPos = mOwner->GetPosition();
    SDL_Rect rect;
    rect.x = static_cast<int>(ownerPos.x - cameraPos.x);
    rect.y = static_cast<int>(ownerPos.y - cameraPos.y);
    rect.w = mWidth;
    rect.h = mHeight;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mOwner->GetRotation() > 0) flip = SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx(
        renderer,
        mSpriteSheetSurface,
        nullptr,
        &rect,
        0,
        nullptr,
        flip
    );
}
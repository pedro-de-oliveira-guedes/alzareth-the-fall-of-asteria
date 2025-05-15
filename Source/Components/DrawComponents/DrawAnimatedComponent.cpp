#include "DrawAnimatedComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Utils/Json.h"
#include <fstream>

DrawAnimatedComponent::DrawAnimatedComponent(
    Actor *owner,
    const std::string &spriteSheetPath,
    const std::string &spriteSheetData,
    const int drawOrder
) : DrawSpriteComponent(owner, spriteSheetPath, 0, 0, drawOrder) {
    LoadSpriteSheet(spriteSheetPath, spriteSheetData);
}

DrawAnimatedComponent::~DrawAnimatedComponent() {
    for (const auto &rect : mSpriteSheetData)
        delete rect;

    mSpriteSheetData.clear();
}

void DrawAnimatedComponent::LoadSpriteSheet(const std::string &texturePath, const std::string &dataPath) {
    // Load sprite sheet texture
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);

    // Load sprite sheet data
    std::ifstream spriteSheetFile(dataPath);
    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    SDL_Rect *rect = nullptr;
    for(const auto &frame : spriteSheetData["frames"]) {
        const int x = frame["frame"]["x"].get<int>();
        const int y = frame["frame"]["y"].get<int>();
        const int w = frame["frame"]["w"].get<int>();
        const int h = frame["frame"]["h"].get<int>();
        rect = new SDL_Rect({x, y, w, h});

        mSpriteSheetData.emplace_back(rect);
    }
}

void DrawAnimatedComponent::Draw(SDL_Renderer *renderer) {
    int spriteIdx = static_cast<int>(mAnimTimer) % mAnimations[mAnimName].size();
    spriteIdx = mAnimations[mAnimName][spriteIdx];

    SDL_Rect screenRegion;
    screenRegion.x = static_cast<int>(mOwner->GetPosition().x - mOwner->GetGame()->GetCameraPos().x);
    screenRegion.y = static_cast<int>(mOwner->GetPosition().y - mOwner->GetGame()->GetCameraPos().y);
    screenRegion.w = mSpriteSheetData[spriteIdx]->w;
    screenRegion.h = mSpriteSheetData[spriteIdx]->h;

    const SDL_RendererFlip flip = mOwner->GetRotation() > 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(
        renderer,
        mSpriteSheetSurface,
        mSpriteSheetData[spriteIdx],
        &screenRegion,
        0,
        nullptr,
        flip
    );
}

void DrawAnimatedComponent::Update(const float deltaTime) {
    if (mIsPaused) return;

    mAnimTimer += deltaTime * mAnimFPS;
    while (static_cast<int>(mAnimTimer) >= mAnimations[mAnimName].size())
        mAnimTimer -= static_cast<float>(mAnimations[mAnimName].size());
}

void DrawAnimatedComponent::SetAnimation(const std::string &name) {
    mAnimName = name;
    Update(0.0f);
}

void DrawAnimatedComponent::AddAnimation(const std::string &name, const std::vector<int> &images) {
    mAnimations.emplace(name, images);
}

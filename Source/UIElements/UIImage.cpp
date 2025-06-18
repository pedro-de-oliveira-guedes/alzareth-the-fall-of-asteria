#include "UIImage.h"

UIImage::UIImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color)
    : UIElement(pos, size, color),
    mTexture(nullptr)
{
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return;
    }
    mTexture = SDL_CreateTextureFromSurface(SDL_GetRenderer(SDL_GetWindowFromID(1)), surface);
    SDL_FreeSurface(surface);
    if (!mTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return;
    }
    
}

UIImage::~UIImage()
{
    if(mTexture){
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void UIImage::Draw(SDL_Renderer* renderer, const Vector2 &screenPos)
{
    if(mTexture == nullptr) {
        SDL_Log("Texture is null. Cannot draw UIImage.");
        return;
    }


    SDL_Rect imageRect;
    imageRect.x = static_cast<int>(screenPos.x + mPosition.x);
    imageRect.y = static_cast<int>(screenPos.y + mPosition.y);
    imageRect.w = static_cast<int>(mSize.x);
    imageRect.h = static_cast<int>(mSize.y);

    SDL_RenderCopy(renderer, mTexture, nullptr, &imageRect);
}
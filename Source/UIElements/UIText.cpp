#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector2 &size, const Vector3 &color)
   :UIElement(pos, size, color)
   ,mFont(font)
   ,mPointSize(pointSize)
   ,mWrapLength(wrapLength)
   ,mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{

}

void UIText::SetText(const std::string &text)
{
    if(mTextTexture){
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }

    mText = text;
    if(mFont){
        mTextTexture = mFont->RenderText(mText, mColor, mPointSize, mWrapLength);
        if(!mTextTexture) {
            SDL_Log("Failed to render text: %s", TTF_GetError());
        }
    }
}

void UIText::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    if (!mTextTexture) {
        SDL_Log("Text texture is null. Can't draw text!");
        return;
    }
    SDL_Rect titleQuad;
    titleQuad.x = static_cast<int>(screenPos.x + mPosition.x);
    titleQuad.y = static_cast<int>(screenPos.y + mPosition.y);
    titleQuad.w = static_cast<int>(mSize.x);
    titleQuad.h = static_cast<int>(mSize.y);

    SDL_RenderCopyEx(renderer, mTextTexture, nullptr, &titleQuad, 0.0, nullptr, SDL_FLIP_NONE);


}
#include "UIButton.h"

UIButton::UIButton(
    const std::string& text,
    class UIFont* font,
    const std::function<void()> &onClick,
    const Vector2 &pos,
    const Vector2 &size,
    const Vector3 &color,
    const int pointSize ,
    const unsigned wrapLength,
    const Vector2 &textPos,
    const Vector2 &textSize,
    const Vector3 &textColor
) : UIElement(pos, size, color),
    mText(text, font, pointSize, wrapLength, textPos, textSize, textColor) {
    mOnClick = onClick;
    mHighlighted = false;
}

UIButton::~UIButton() { }

void UIButton::Draw(SDL_Renderer *renderer, const Vector2 &screenPos) {
    SDL_Rect titleQuad;
    titleQuad.x = static_cast<int>(screenPos.x + mPosition.x);
    titleQuad.y = static_cast<int>(screenPos.y + mPosition.y);  
    titleQuad.w = static_cast<int>(mSize.x);
    titleQuad.h = static_cast<int>(mSize.y);
  
    if (mHighlighted) {
        SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
        SDL_RenderFillRect(renderer, &titleQuad);
    } else {
        SDL_SetRenderDrawColor(renderer, static_cast<Uint8>(mColor.x), static_cast<Uint8>(mColor.y), static_cast<Uint8>(mColor.z), 255);
    }

    mText.Draw(renderer, mPosition);
}

void UIButton::OnClick() {
    if (mOnClick)  {
        mOnClick();
    }    
}

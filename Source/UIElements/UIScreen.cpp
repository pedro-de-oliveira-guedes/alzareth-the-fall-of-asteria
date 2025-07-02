#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName) {
	mGame = game;
    mGame->PushUI(this);

	mPos = Vector2(0.f, 0.f);
	mSize = Vector2(0.f, 0.f);
	mState = UIState::Active;
    mSelectedButtonIndex = -1;

    mFont = mGame->LoadFont(fontName);
    if (!mFont) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }
}

UIScreen::~UIScreen() {
    for(const auto text : mTexts) {
        delete text;
    }
    mTexts.clear();

    for(const auto button : mButtons) {
        delete button;
    }
    mButtons.clear();

    for(const auto image : mImages) {
        delete image;
    }
    mImages.clear();
}

void UIScreen::Update(float deltaTime) { }

void UIScreen::Draw(SDL_Renderer *renderer) {
    std::vector<UIElement*> elements;

    for(const auto text : mTexts) {
        elements.push_back(text);
    }

    for(const auto button : mButtons) {
        elements.push_back(button);
    }

    for(const auto image : mImages) {
        elements.push_back(image);
    }

    std::sort(
        elements.begin(),
        elements.end(),
        [](const UIElement* a, const UIElement* b) {
            return a->GetDrawOrder() < b->GetDrawOrder();
        }
    );
    for (const auto element : elements) {
        element->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys) { }

void UIScreen::HandleKeyPress(const int key) {
    if (mState != UIState::Active)
        return;

    if (key == SDLK_w) {
        mSelectedButtonIndex--;
        mGame->GetAudioSystem()->PlaySound("menu_click.ogg", false);
        if (mSelectedButtonIndex < 0) {
            mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
        }
    }
    else if (key == SDLK_s) {
        mSelectedButtonIndex++;
        mGame->GetAudioSystem()->PlaySound("menu_click.ogg", false);
        if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())) {
            mSelectedButtonIndex = 0;
        }
    }
    else if (key == SDLK_RETURN) {
        mGame->GetAudioSystem()->PlaySound("menu_click.ogg", false);
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }
    for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(i == static_cast<size_t>(mSelectedButtonIndex));
    }
}

void UIScreen::Close() {
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(
    const std::string &name,
    const Vector2 &pos,
    const Vector2 &dims,
    const int pointSize,
    const int unsigned wrapLength
) {
    auto *t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, Color::White);
    mTexts.push_back(t);
    return t;
}

UIButton* UIScreen::AddButton(
    const std::string& text,
    const Vector2 &pos,
    const Vector2& dims,
    const std::function<void()> &onClick
) {
    const auto textSize = Vector2(text.size() * 0.6f * 40, 40); // Approximate width based on character count and point size
    auto *b = new UIButton(
        text,
        mFont,
        onClick,
        pos,
        dims,
        Vector3(1.0f, 0.5f, 0.0f),
        40, // Default point size
        dims.x,
        pos + (dims - textSize) * 0.5f, // Center text in button
        textSize
    );
    mButtons.push_back(b);

    if(mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(
    const std::string &imagePath,
    const Vector2 &pos,
    const Vector2 &dims,
    const Vector3 &color
) {
    auto *img = new UIImage(imagePath, pos, dims, color);
    mImages.push_back(img);
    return img;
}
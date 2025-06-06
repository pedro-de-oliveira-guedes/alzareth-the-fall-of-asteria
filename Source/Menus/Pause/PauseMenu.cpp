#include "PauseMenu.h"

#include "../../Game.h"

PauseMenu::PauseMenu(Game *game, const int width, const int height) : BaseMenu(game, width, height) {
    const int startX = (game->GetWindowWidth() - width) / 2;
    const int startY = (game->GetWindowHeight() - height) / 2;

    const auto continueGame = MenuItem{
        static_cast<int>(startX + (0.25f * width)),
        static_cast<int>(startY + (0.1f * height)),
        static_cast<int>(0.5f * width),
        static_cast<int>(0.15f * height),
        false,
        "Continuar",
        [this]() {mState = MenuState::STOPPED;}
    };
    mOptions.emplace_back(continueGame);

    const auto anotherOne = MenuItem{
        static_cast<int>(startX + (0.25f * width)),
        static_cast<int>(continueGame.start_y + continueGame.height + 0.05f * mGame->GetWindowHeight()),
        static_cast<int>(0.5f * width),
        static_cast<int>(0.15f * height),
        false,
        "Outra opção",
        [this]() {SDL_Log("DJ Khalid!");}
    };
    mOptions.emplace_back(anotherOne);

    const auto quitGame = MenuItem{
        static_cast<int>(startX + (0.25f * width)),
        static_cast<int>(anotherOne.start_y + anotherOne.height + 0.05f * mGame->GetWindowHeight()),
        static_cast<int>(0.5f * width),
        static_cast<int>(0.15f * height),
        false,
        "Sair do jogo",
        [this]() {mGame->Quit();}
    };
    mOptions.emplace_back(quitGame);

    mOptions[mCurrentOption % mOptions.size()].selected = true;

    mType = MenuType::PAUSE_MENU;
}

PauseMenu::~PauseMenu() {
    mGame->RemoveMenu(this);
}

void PauseMenu::ProcessInput(const SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
        const SDL_Keycode pressedKey = event.key.keysym.sym;
        if (pressedKey == SDLK_ESCAPE) {
            mState = MenuState::STOPPED;
        }
        else if (pressedKey == SDLK_SPACE || pressedKey == SDLK_RETURN) {
            mOptions[mCurrentOption % mOptions.size()].action();
        }
        else if (pressedKey == SDLK_s || pressedKey == SDLK_DOWN) {
            mCurrentOption++;
        }
        else if (pressedKey == SDLK_w || pressedKey == SDLK_UP) {
            mCurrentOption--;
        }
    }

    if (mCurrentOption >= mOptions.size()) mCurrentOption = 0;
    else if (mCurrentOption < 0) mCurrentOption = mOptions.size() - 1;

    for (int i = 0; i < mOptions.size(); i++) {
        if (i == mCurrentOption) mOptions[i].selected = true;
        else mOptions[i].selected = false;
    }
}

void PauseMenu::Draw(SDL_Renderer *renderer) {
    SDL_Rect background;
    background.x = (mGame->GetWindowWidth() - mMenuWidth) / 2;
    background.y = (mGame->GetWindowHeight() - mMenuHeight) / 2;
    background.w = mMenuWidth;
    background.h = mMenuHeight;

    SDL_SetRenderDrawColor(renderer, 114, 114, 114, 255);
    SDL_RenderFillRect(renderer, &background);

    for (const auto &option : mOptions) {
        SDL_Rect optionBkg;
        optionBkg.x = option.start_x;
        optionBkg.y = option.start_y;
        optionBkg.w = option.width;
        optionBkg.h = option.height;

        SDL_Color bkgColor, borderColor;
        if (option.selected) {
            bkgColor = SDL_Color{58, 58, 58, 255};
            borderColor = SDL_Color{0, 0, 0, 255};
        }
        else {
            bkgColor = SDL_Color{0, 0, 0, 255};
            borderColor = SDL_Color{255, 255, 255, 255};
        }

        SDL_SetRenderDrawColor(renderer, bkgColor.r, bkgColor.g, bkgColor.b, bkgColor.a);
        SDL_RenderFillRect(renderer, &optionBkg);

        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(renderer, &optionBkg);
    }

    SDL_RenderPresent(renderer);
}

#include "BaseMenu.h"

#include <SDL_events.h>

#include "../Game.h"

BaseMenu::BaseMenu(Game *game, const int width, const int height) {
    mGame = game;
    mGame->AddMenu(this);

    mMenuWidth = width;
    mMenuHeight = height;

    mState = MenuState::STOPPED;

    mOptions = std::vector<MenuItem>();
    mCurrentOption = 0;
}

BaseMenu::~BaseMenu() {
    // Clean up menu items
    mOptions.clear();
}

void BaseMenu::ProcessInput(SDL_Event event) {}

void BaseMenu::Draw(SDL_Renderer *renderer) {}

void BaseMenu::MenuLoop(SDL_Renderer *renderer) {
    while (mState == MenuState::RUNNING && mGame->GetGameState() != Game::GameState::QUITTING) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                mState = MenuState::STOPPED;
                mGame->Quit();
                return;
            }

            ProcessInput(event);
        }

        Draw(renderer);
    }
}

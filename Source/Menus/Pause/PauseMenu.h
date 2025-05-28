#pragma once

#include "../BaseMenu.h"
#include "../../Game.h"

class PauseMenu : public BaseMenu {
    public:
        explicit PauseMenu(Game* game, int width = 0.5f * Game::SCREEN_WIDTH, int height = 0.5f * Game::SCREEN_HEIGHT);

        ~PauseMenu() override;

        void ProcessInput(SDL_Event event) override;
        void Draw(SDL_Renderer *renderer) override;
};

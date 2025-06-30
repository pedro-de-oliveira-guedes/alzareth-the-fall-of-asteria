#pragma once

#include <functional>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <string>
#include <vector>

struct MenuItem {
    int start_x, start_y;
    int width, height;
    bool selected;
    std::string text;
    std::function<void()> action;
};

class BaseMenu {
    public:
        enum MenuState {
            RUNNING,
            STOPPED
        };

        enum MenuType {
            MAIN_MENU,
            PAUSE_MENU
        };

        explicit BaseMenu(class Game *game, int width, int height);

        virtual ~BaseMenu();

        virtual void ProcessInput(SDL_Event event);
        virtual void Draw(SDL_Renderer *renderer);
        virtual void MenuLoop(SDL_Renderer *renderer);

        void SetState(const MenuState state) {mState = state;}
        MenuType GetType() const { return mType; }

    protected:
        Game *mGame;

        int mMenuWidth;
        int mMenuHeight;

        MenuState mState;
        MenuType mType;

        std::vector<MenuItem> mOptions;
        int mCurrentOption;
};

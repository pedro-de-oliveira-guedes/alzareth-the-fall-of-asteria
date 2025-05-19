#define SDL_MAIN_HANDLED
#include "Game.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

int main(int argc, char *argv[]) {
    auto game = Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (game.Initialize()) {
        game.RunLoop();
    }

    game.Shutdown();

    return 0;
}

#define SDL_MAIN_HANDLED
#include "Game.h"

int main(int argc, char *argv[]) {
    auto game = Game();

    if (game.Initialize()) {
        game.RunLoop();
    }

    game.Shutdown();

    return 0;
}

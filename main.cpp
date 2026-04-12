#include "Game.h"


Game* game = nullptr;


int main(int argc, char* argv[]) {

    game = new Game();

    game->init("Air Force", screenW, screenH);


    while (game->running()) {
        game->handleEvents();
        game->update();
        game->render();


        SDL_Delay(16);
    }


    game->clean();

    return 0;
}

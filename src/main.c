#include "game.h"
#include "log.h"
#include <stdint.h>

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    srand(time(0));
    Game *theGame = createGame();
    runGame(theGame);
    destroyGame(theGame);
}

#include "game.h"
#include "log.h"
#include <stdint.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    srand(time(0));
    Game *theGame = createGame();
    if (!initMedia(&theGame->media)) {
        log_error("Failed to initialize media\n");
        return 0;
    }
    Game *theGame2 = createGame();
    while (!stepGame(theGame))
        ;
    // runGame(theGame);
    destroyGame(theGame);
}

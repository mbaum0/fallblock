#include "game.h"
#include "log.h"
#include <stdint.h>

int main(void) {
    srand(time(0));
    Game *theGame = createGame();
    runGame(theGame);
    destroyGame(theGame);
}

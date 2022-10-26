#include "game.h"
#include "log.h"
#include <stdint.h>

int main(void) {
    Game *theGame = createGame();

    destroyGame(theGame);
}

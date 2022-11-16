#include "game.h"
#include "log.h"
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    srand(time(0));
    GameRunner *game = createGame(MULTI_PLAYER);
    runGame(game);
    destroyGame(game);
}

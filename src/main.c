#include "display.h"
#include "game.h"
#include "input.h"
#include "log.h"
#include "media.h"
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    srand(time(0));
    GameRunner *game = createGame(SINGLE_PLAYER);
    runGame(game);
    destroyGame(game);
}

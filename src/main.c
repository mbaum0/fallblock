#include "game.h"
#include "log.h"
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (argc < 2) {
        log_warn("Game mode not specified [single/multi], defaulting to single "
                 "player.");
    }

    GameMode mode = SINGLE_PLAYER;
    uint16_t localPort = 0;
    uint16_t remotePort = 0;
    if (argc > 1 && strncmp(argv[1], "multi", 5) == 0) {
        mode = MULTI_PLAYER;
        log_info("Starting a new multi player game");
        localPort = atoi(argv[2]);
        remotePort = atoi(argv[3]);
    } else {
        log_info("Started a new single player game");
    }

    GameRunner *game = createGame(mode, localPort, remotePort);
    srand(time(0));

    runGame(game);
    destroyGame(game);
}

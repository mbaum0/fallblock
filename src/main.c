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
    MPType mptype = MP_CLIENT;
    if (argc > 1 && strncmp(argv[1], "multi", 5) == 0) {
        if (argc < 3) {
            log_error("multiplayer mode requires a client/server specification "
                      "[client/server]");
            exit(0);
        }

        if (strncmp(argv[2], "server", 6) == 0) {
            mptype = MP_SERVER;
        }
        mode = MULTI_PLAYER;
        log_info("Starting a new multi player game");
    } else {
        log_info("Started a new single player game");
    }

    GameRunner *game = createGame(mode, mptype);
    srand(time(0));

    runGame(game);
    destroyGame(game);
}

#include "game.h"
#include "log.h"
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    if (argc < 2){
        log_warn("Game mode not specified [single/multi], defaulting to single player.");
    }
    
    GameMode mode = SINGLE_PLAYER;
    if (strncmp(argv[1], "multi", 5) == 0){
        mode = MULTI_PLAYER;
        log_info("Starting a new multi player game");
    } else {
        log_info("Started a new single player game");
    }

    GameRunner* game = createGame(mode);
    srand(time(0));
    
    runGame(game);
    destroyGame(game);
}

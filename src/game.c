#include "game.h"

Game *createGame(void) {
    Game *newGame = calloc(1, sizeof(Game));
    if (!initMedia(&newGame->media)) {
        log_error("Failed to initialize media\n");
        return NULL;
    }

    newGame->board.nextPieceType = getRandomPieceType();
    newGame->board.score = 0;
    newGame->board.level = 1;

    newGame->keyboard = (Keyboard){0};
    newGame->ticks = 0;

    return newGame;
}

void destroyGame(Game *game) {
    destroyMedia(&game->media);
    free(game);
}

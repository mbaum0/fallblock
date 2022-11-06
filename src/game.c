#include "game.h"

void createNextPiece(Game *game);

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

void createNextPiece(Game *game) {
    PieceType type = getRandomPieceType();
    uint32_t spawnx = GAME_WIDTH / 2;
    uint32_t spawny = GAME_HEIGHT +1;
    game->board.activePiece = createNewPiece(spawnx, spawny, type);
}

void dropActivePiece(Game *game){
    for(int32_t i = 0; i < 4; i++){
        game->board.activePiece->tiles[i]->y -= 1;
    }
}

void doPieceLogic(Game *game) {
    // if piece doesn't exist, create one
    if (game->board.activePiece == NULL) {
        createNextPiece(game);
    } else {
        dropActivePiece(game);
    }
}

void runGame(Game *game) {
    while (!processInput(&game->keyboard)) {
        updateDisplay(&game->media, &game->board);
        SDL_Delay(1000);
        doPieceLogic(game);
    };
}

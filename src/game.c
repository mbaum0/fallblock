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
    newGame->lastTick = SDL_GetPerformanceCounter();
    newGame->lastDrop = SDL_GetPerformanceCounter();

    return newGame;
}

void destroyGameBoard(GameBoard *gameBoard) {
    for (int32_t x = 0; x < GAME_WIDTH; x++) {
        for (int32_t y = 0; y < GAME_HEIGHT; y++) {
            if (gameBoard->playField[x][y] != NULL) {
                free(gameBoard->playField[x][y]);
            }
        }
    }

    if (gameBoard->activePiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            free(gameBoard->activePiece->tiles[i]);
        }
        free(gameBoard->activePiece);
    }
}

void destroyGame(Game *game) {
    destroyMedia(&game->media);
    destroyGameBoard(&game->board);
    free(game);
}

void createNextPiece(Game *game) {
    PieceType type = getRandomPieceType();
    uint32_t spawnx = GAME_WIDTH / 2;
    uint32_t spawny = GAME_HEIGHT + 1;
    game->board.activePiece = createNewPiece(spawnx, spawny, type);
}

bool doesTileExistAtCoordianate(Game *game, int32_t x, int32_t y) {
    if (y >= GAME_HEIGHT) {
        // we are allowed to spawn above game height, and no collision will
        // occur here.
        return false;
    }
    return (game->board.playField[x][y] != NULL);
}

bool canActivePieceDrop(Game *game) {
    for (uint32_t i = 0; i < 4; i++) {
        Tile *tile = game->board.activePiece->tiles[i];
        int32_t newY = tile->y - 1;
        if (doesTileExistAtCoordianate(game, tile->x, newY)) {
            return false;
        } else if (newY < 0) {
            return false;
        }
    }
    return true;
}

bool canMoveActivePiece(Game *game, int32_t dx) {
    for (uint32_t i = 0; i < 4; i++) {
        int32_t newX = game->board.activePiece->tiles[i]->x + dx;
        int32_t y = game->board.activePiece->tiles[i]->y;
        if (newX < 0 || newX >= GAME_WIDTH) {
            return false;
        }

        if (game->board.playField[newX][y] != NULL){
            return false;
        }
    }
    return true;
}

void dropActivePiece(Game *game) {
    movePiece(game->board.activePiece, 0, -1);
}

void lockActivePieceOnBoard(Game *game) {
    for (int32_t i = 0; i < 4; i++) {
        Tile *tile = game->board.activePiece->tiles[i];
        game->board.playField[tile->x][tile->y] = tile;
    }
    free(game->board.activePiece);
    game->board.activePiece = NULL;
}

void doInputLogic(Game *game) {
    if (game->board.activePiece != NULL) {
        if (game->keyboard.keys[SDL_SCANCODE_RIGHT].pressed) {
            if (canMoveActivePiece(game, 1)) {
                movePiece(game->board.activePiece, 1, 0);
            }
            game->keyboard.keys[SDL_SCANCODE_RIGHT].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_LEFT].pressed) {
            if (canMoveActivePiece(game, -1)) {
                movePiece(game->board.activePiece, -1, 0);
            }

            game->keyboard.keys[SDL_SCANCODE_LEFT].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_DOWN].pressed) {
            if (canActivePieceDrop(game)) {
                movePiece(game->board.activePiece, 0, -1);
            }
            game->keyboard.keys[SDL_SCANCODE_DOWN].pressed = false;
        }
    }
}

/**
 * @brief Reset pressed keys to false
 * 
 * @param game game instance
 */
void resetInputKeys(Game *game){
    game->keyboard.keys[SDL_SCANCODE_RIGHT].pressed = false;
    game->keyboard.keys[SDL_SCANCODE_LEFT].pressed = false;
    game->keyboard.keys[SDL_SCANCODE_DOWN].pressed = false;
}

void doPieceLogic(Game *game) {
    // if piece doesn't exist, create one
    if (game->board.activePiece == NULL) {
        resetInputKeys(game);
        createNextPiece(game);
    } else {
        if (canActivePieceDrop(game)) {
            dropActivePiece(game);
        } else {
            lockActivePieceOnBoard(game);
        }
    }
}

bool shouldTick(Game *game) {
    uint64_t currentTick = SDL_GetPerformanceCounter();
    uint64_t elapsedTicks = (currentTick - game->lastTick);
    float tickFreq = (float)SDL_GetPerformanceFrequency();
    float elapsedMS = (elapsedTicks / tickFreq) * 1000;

    // we want to tick at 60fps, so only return true if it has been 16.66ms
    // since last tick
    if (elapsedMS >= 16.666f) {
        game->lastTick = currentTick;
        return true;
    }
    return false;
}

bool shouldDropPiece(Game *game, uint32_t dropDelayMS) {
    uint64_t currentTick = SDL_GetPerformanceCounter();
    uint64_t elapsedTicks = (currentTick - game->lastDrop);
    float tickFreq = (float)SDL_GetPerformanceFrequency();
    float elapsedMS = (elapsedTicks / tickFreq) * 1000;

    if (elapsedMS >= dropDelayMS) {
        game->lastDrop = currentTick;
        return true;
    }
    return false;
}

void runGame(Game *game) {
    while (!processInput(&game->keyboard)) {
        if (shouldTick(game)) {
            updateDisplay(&game->media, &game->board);
            if (shouldDropPiece(game, 500)) {
                doPieceLogic(game);
            }
        }
        doInputLogic(game);
    };
}

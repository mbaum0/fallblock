#include "game.h"

void createNextPiece(Game *game);
bool shouldDropPiece(Game *game, uint32_t dropDelayMS);
void updateDropDelay(Game *game);
bool canPieceDrop(Game *game, Piece *piece);
void hardDropPiece(Game *game, Piece *piece);

Game *createGame(void) {
    Game *newGame = calloc(1, sizeof(Game));
    if (!initMedia(&newGame->media)) {
        log_error("Failed to initialize media\n");
        return NULL;
    }

    newGame->board.nextPieceType = getRandomPieceType();
    newGame->board.score = 0;
    newGame->board.level = 1;
    newGame->board.ghostPiece = NULL;

    newGame->keyboard = (Keyboard){0};
    newGame->lastTick = SDL_GetPerformanceCounter();
    newGame->lastDrop = SDL_GetPerformanceCounter();
    newGame->board.lastSuccessfulMove = SDL_GetPerformanceCounter();

    updateDropDelay(newGame);

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

    for (uint32_t i = 0; i < 4; i++) {
        free(gameBoard->ghostPiece->tiles[i]);
    }
    free(gameBoard->ghostPiece);
}

void destroyGame(Game *game) {
    destroyMedia(&game->media);
    destroyGameBoard(&game->board);
    free(game);
}

void createNextPiece(Game *game) {
    uint32_t spawnx = GAME_WIDTH / 2;
    uint32_t spawny = GAME_HEIGHT;
    game->board.activePiece =
        createNewPiece(spawnx, spawny, game->board.nextPieceType);
    game->board.nextPieceType = getRandomPieceType();

    game->board.ghostPiece = createNewPiece(0, 0, game->board.nextPieceType);
}

void updateGhostPiece(Game *game) {
    Piece *ghost = game->board.ghostPiece;
    if (ghost != NULL) {
        Piece *active = game->board.activePiece;
        ghost->pieceState = active->pieceState;
        ghost->pieceType = active->pieceType;
        for (uint32_t i = 0; i < 4; i++) {
            ghost->tiles[i]->x = active->tiles[i]->x;
            ghost->tiles[i]->y = active->tiles[i]->y;
        }
        hardDropPiece(game, ghost);
    }
}

bool doesTileExistAtCoordianate(Game *game, int32_t x, int32_t y) {
    if (y >= GAME_HEIGHT) {
        // we are allowed to spawn above game height, and no collision will
        // occur here.
        return false;
    }
    return (game->board.playField[x][y] != NULL);
}

bool canPieceDrop(Game *game, Piece *piece) {
    for (uint32_t i = 0; i < 4; i++) {
        Tile *tile = piece->tiles[i];
        int32_t newY = tile->y - 1;
        if (doesTileExistAtCoordianate(game, tile->x, newY)) {
            return false;
        } else if (newY < 0) {
            return false;
        }
    }
    return true;
}

bool canActivePieceDrop(Game *game) {
    return canPieceDrop(game, game->board.activePiece);
}

void hardDropPiece(Game *game, Piece *piece) {
    while (canPieceDrop(game, piece)) {
        movePiece(piece, 0, -1);
    }
}

void hardDropActivePiece(Game *game) {
    hardDropPiece(game, game->board.activePiece);
}

bool canMovePiece(Game *game, Piece *piece, int32_t dx, int32_t dy) {
    for (uint32_t i = 0; i < 4; i++) {
        int32_t newX = piece->tiles[i]->x + dx;
        int32_t newY = piece->tiles[i]->y + dy;
        if (newX < 0 || newX >= GAME_WIDTH || newY < 0) {
            return false;
        }

        if (game->board.playField[newX][newY] != NULL) {
            return false;
        }
    }
    return true;
}

void dropActivePiece(Game *game) {
    movePiece(game->board.activePiece, 0, -1);
}

void dropTilesAboveRow(Game *game, uint32_t row) {
    for (uint32_t y = row + 1; y < GAME_HEIGHT; y++) {
        for (uint32_t x = 0; x < GAME_WIDTH; x++) {
            Tile *tile = game->board.playField[x][y];
            if (tile != NULL) {
                tile->y -= 1;
                game->board.playField[x][y - 1] = tile;
                game->board.playField[x][y] = NULL;
            }
        }
    }
}

uint32_t getCompletedRowScoreValue(uint32_t numCompletedRows) {
    switch (numCompletedRows) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 3;
    case 3:
        return 5;
    case 4:
        return 8;
    default:
        return 0;
    }
}

void updateLevel(Game *game) {
    // go to next level is score exceeds current level * 5
    if (game->board.score >= game->board.level * 5) {
        game->board.level += 1;
    }
}

void updateDropDelay(Game *game) {
    uint32_t level = game->board.level;
    float timeInSeconds = pow((0.8f - ((level - 1) * (0.007f))), level - 1);
    uint32_t timeInMS = (uint32_t)(timeInSeconds * 1000);
    game->board.dropDelayMS = timeInMS;
    log_debug("New drop delay: %d", timeInMS);
}

void processCompletedRows(Game *game) {
    uint32_t numCompletedRows = 0;
    // check if a row is completed
    for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
        uint32_t rowSum = 0;
        for (uint32_t x = 0; x < GAME_WIDTH; x++) {
            if (game->board.playField[x][y] != NULL) {
                rowSum++;
            }
        }
        if (rowSum >= GAME_WIDTH) {
            // completed row :)
            numCompletedRows += 1;
            for (uint32_t x = 0; x < GAME_WIDTH; x++) {
                free(game->board.playField[x][y]);
                game->board.playField[x][y] = NULL;
            }
            dropTilesAboveRow(game, y);
        }
    }
    game->board.score += getCompletedRowScoreValue(numCompletedRows);

    // if we completed some rows, update the level and drop delay
    if (numCompletedRows > 0) {
        updateLevel(game);
        updateDropDelay(game);
    }
}

void lockActivePieceOnBoard(Game *game) {
    for (int32_t i = 0; i < 4; i++) {
        Tile *tile = game->board.activePiece->tiles[i];
        game->board.playField[tile->x][tile->y] = tile;
    }
    free(game->board.activePiece);
    free(game->board.ghostPiece);
    game->board.activePiece = NULL;
    game->board.ghostPiece = NULL;
}

bool attemptKick(Game *game, Piece *piece, int32_t dx, int32_t dy) {
    if (canMovePiece(game, piece, dx, dy)) {
        movePiece(piece, dx, dy);
        return true;
    }
    return false;
}

bool attemptKick_I(Game *game, Piece *piece, bool clockwise) {
    if (clockwise) {
        switch (piece->pieceState) {
        case PS_A:
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 1A successful");
                return true;
            }
            if (attemptKick(game, piece, -2, 0)) {
                log_debug("Kick test 2A successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -2)) {
                log_debug("Kick test 3A successful");
                return true;
            }
            if (attemptKick(game, piece, -2, 1)) {
                log_debug("Kick test 4A successful");
                return true;
            }
            break;
        case PS_B:
            if (attemptKick(game, piece, -2, 0)) {
                log_debug("Kick test 1B successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 2B successful");
                return true;
            }
            if (attemptKick(game, piece, -2, -1)) {
                log_debug("Kick test 3B successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 2)) {
                log_debug("Kick test 4B successful");
                return true;
            }
            break;
        case PS_C:
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 1C successful");
                return true;
            }
            if (attemptKick(game, piece, 2, 0)) {
                log_debug("Kick test 2C successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 2)) {
                log_debug("Kick test 3C successful");
                return true;
            }
            if (attemptKick(game, piece, 2, -1)) {
                log_debug("Kick test 4C successful");
                return true;
            }
            break;
        case PS_D:
            if (attemptKick(game, piece, 2, 0)) {
                log_debug("Kick test 1D successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 2D successful");
                return true;
            }
            if (attemptKick(game, piece, 2, 1)) {
                log_debug("Kick test 3D successful");
                return true;
            }
            if (attemptKick(game, piece, -1, -2)) {
                log_debug("Kick test 4D successful");
                return true;
            }
            break;
        }
    } else {
        switch (piece->pieceState) {
        case PS_A:
            if (attemptKick(game, piece, 2, 0)) {
                log_debug("Kick test 1A successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 2A successful");
                return true;
            }
            if (attemptKick(game, piece, 2, 1)) {
                log_debug("Kick test 3A successful");
                return true;
            }
            if (attemptKick(game, piece, -1, -2)) {
                log_debug("Kick test 4A successful");
                return true;
            }
            break;
        case PS_B:
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 1B successful");
                return true;
            }
            if (attemptKick(game, piece, -2, 0)) {
                log_debug("Kick test 2B successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -2)) {
                log_debug("Kick test 3B successful");
                return true;
            }
            if (attemptKick(game, piece, -2, 1)) {
                log_debug("Kick test 4B successful");
                return true;
            }
            break;
        case PS_C:
            if (attemptKick(game, piece, -2, 0)) {
                log_debug("Kick test 1C successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 2C successful");
                return true;
            }
            if (attemptKick(game, piece, -2, -1)) {
                log_debug("Kick test 3C successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 2)) {
                log_debug("Kick test 4C successful");
                return true;
            }
            break;
        case PS_D:
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 1D successful");
                return true;
            }
            if (attemptKick(game, piece, 2, 0)) {
                log_debug("Kick test 2D successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 2)) {
                log_debug("Kick test 3D successful");
                return true;
            }
            if (attemptKick(game, piece, 2, -1)) {
                log_debug("Kick test 4D successful");
                return true;
            }
            break;
        }
    }
    return false;
}

bool attemptKick_JLTSZ(Game *game, Piece *piece, bool clockwise) {
    if (clockwise) {
        switch (piece->pieceState) {
        case PS_A:
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 1A successful");
                return true;
            }
            if (attemptKick(game, piece, -1, -1)) {
                log_debug("Kick test 2A successful");
                return true;
            }
            if (attemptKick(game, piece, 0, 2)) {
                log_debug("Kick test 3A successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -2)) {
                log_debug("Kick test 4A successful");
                return true;
            }
            break;
        case PS_B:
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 1B successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 1)) {
                log_debug("Kick test 2B successful");
                return true;
            }
            if (attemptKick(game, piece, 0, -2)) {
                log_debug("Kick test 3B successful");
                return true;
            }
            if (attemptKick(game, piece, -1, -2)) {
                log_debug("Kick test 4B successful");
                return true;
            }
            break;
        case PS_C:
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 1C successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -1)) {
                log_debug("Kick test 2C successful");
                return true;
            }
            if (attemptKick(game, piece, 0, 2)) {
                log_debug("Kick test 3C successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 2)) {
                log_debug("Kick test 4C successful");
                return true;
            }
            break;
        case PS_D:
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 1D successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 1)) {
                log_debug("Kick test 2D successful");
                return true;
            }
            if (attemptKick(game, piece, 0, -2)) {
                log_debug("Kick test 3D successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -2)) {
                log_debug("Kick test 4D successful");
                return true;
            }
            break;
        }
    } else {
        switch (piece->pieceState) {
        case PS_A:
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 1A successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -1)) {
                log_debug("Kick test 2A successful");
                return true;
            }
            if (attemptKick(game, piece, 0, 2)) {
                log_debug("Kick test 3A successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 2)) {
                log_debug("Kick test 4A successful");
                return true;
            }
            break;
        case PS_B:
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 1B successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 1)) {
                log_debug("Kick test 2B successful");
                return true;
            }
            if (attemptKick(game, piece, 0, -2)) {
                log_debug("Kick test 3B successful");
                return true;
            }
            if (attemptKick(game, piece, -1, -2)) {
                log_debug("Kick test 4B successful");
                return true;
            }
            break;
        case PS_C:
            if (attemptKick(game, piece, -1, 0)) {
                log_debug("Kick test 1C successful");
                return true;
            }
            if (attemptKick(game, piece, -1, -1)) {
                log_debug("Kick test 2C successful");
                return true;
            }
            if (attemptKick(game, piece, 0, 2)) {
                log_debug("Kick test 3C successful");
                return true;
            }
            if (attemptKick(game, piece, -1, 2)) {
                log_debug("Kick test 4C successful");
                return true;
            }
            break;
        case PS_D:
            if (attemptKick(game, piece, 1, 0)) {
                log_debug("Kick test 1D successful");
                return true;
            }
            if (attemptKick(game, piece, 1, 1)) {
                log_debug("Kick test 2D successful");
                return true;
            }
            if (attemptKick(game, piece, 0, -2)) {
                log_debug("Kick test 3D successful");
                return true;
            }
            if (attemptKick(game, piece, 1, -2)) {
                log_debug("Kick test 4D successful");
                return true;
            }
            break;
        }
    }
    return false;
}

bool attemptActivePieceRotation(Game *game, bool clockwise) {
    Piece *activePiece = game->board.activePiece;
    if (clockwise) {
        rotatePieceClockwise(activePiece);
        if (canMovePiece(game, activePiece, 0, 0)) {
            return true;
        }
        log_debug("Basic rotation failed, attempting kicks");
        // attempt kicks
        switch (game->board.activePiece->pieceType) {
        case Piece_J:
        case Piece_L:
        case Piece_T:
        case Piece_S:
        case Piece_Z:
            if (!attemptKick_JLTSZ(game, activePiece, clockwise)) {
                // all kicks failed, revert
                rotatePieceCounterClockwise(activePiece);
                log_debug("Kicks failed. Piece will not rotate");
                return false;
            }
            break;
        case Piece_I:
            if (!attemptKick_I(game, activePiece, clockwise)) {
                rotatePieceCounterClockwise(activePiece);
                return false;
            }
        case Piece_O:
            return false;
        }
    } else {
        rotatePieceCounterClockwise(activePiece);
        if (canMovePiece(game, activePiece, 0, 0)) {
            return true;
        }
        log_debug("Basic rotation failed, attempting kicks");
        // attempt kicks
        switch (game->board.activePiece->pieceType) {
        case Piece_J:
        case Piece_L:
        case Piece_T:
        case Piece_S:
        case Piece_Z:
            if (!attemptKick_JLTSZ(game, activePiece, clockwise)) {
                // all kicks failed, revert
                rotatePieceClockwise(activePiece);
                log_debug("Kicks failed. Piece will not rotate");
                return false;
            }
            break;
        case Piece_I:
            if (!attemptKick_I(game, activePiece, clockwise)) {
                rotatePieceClockwise(activePiece);
                return false;
            }
        case Piece_O:
            return false;
        }
    }
    return false;
}

void doInputLogic(Game *game) {
    if (game->board.activePiece != NULL) {
        if (game->keyboard.keys[SDL_SCANCODE_RIGHT].pressed) {
            if (canMovePiece(game, game->board.activePiece, 1, 0)) {
                movePiece(game->board.activePiece, 1, 0);
                game->board.lastSuccessfulMove = SDL_GetPerformanceCounter();
            }
            game->keyboard.keys[SDL_SCANCODE_RIGHT].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_LEFT].pressed) {
            if (canMovePiece(game, game->board.activePiece, -1, 0)) {
                movePiece(game->board.activePiece, -1, 0);
                game->board.lastSuccessfulMove = SDL_GetPerformanceCounter();
            }

            game->keyboard.keys[SDL_SCANCODE_LEFT].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_DOWN].pressed) {
            if (canActivePieceDrop(game)) {
                movePiece(game->board.activePiece, 0, -1);
            }
            game->keyboard.keys[SDL_SCANCODE_DOWN].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_UP].pressed ||
            game->keyboard.keys[SDL_SCANCODE_X].pressed) {
            if (attemptActivePieceRotation(game, true)) {
                game->board.lastSuccessfulMove = SDL_GetPerformanceCounter();
            }
            game->keyboard.keys[SDL_SCANCODE_UP].pressed = false;
            game->keyboard.keys[SDL_SCANCODE_X].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_Z].pressed) {
            if (attemptActivePieceRotation(game, false)) {
                game->board.lastSuccessfulMove = SDL_GetPerformanceCounter();
            }
            game->keyboard.keys[SDL_SCANCODE_Z].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_SPACE].pressed) {
            hardDropActivePiece(game);
            game->keyboard.keys[SDL_SCANCODE_SPACE].pressed = false;
        }
    }
}

bool timeHasOccurredSinceLastSuccessfulMove(Game *game, uint32_t time_ms) {
    uint64_t currentTick = SDL_GetPerformanceCounter();
    uint64_t elapsedTicks = (currentTick - game->board.lastSuccessfulMove);
    float tickFreq = (float)SDL_GetPerformanceFrequency();
    float elapsedMS = (elapsedTicks / tickFreq) * 1000;

    return (elapsedMS >= time_ms);
}

void doPieceLogic(Game *game) {
    // if piece doesn't exist, create one
    if (game->board.activePiece == NULL) {
        createNextPiece(game);
    }

    if (shouldDropPiece(game, game->board.dropDelayMS)) {
        if (canActivePieceDrop(game)) {
            dropActivePiece(game);
        } else {
            if (timeHasOccurredSinceLastSuccessfulMove(game, 200)) {
                lockActivePieceOnBoard(game);
            } else {
                log_debug("Lock delay preventing piece from locking.");
            }
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

            doPieceLogic(game);
        }
        doInputLogic(game);
        processCompletedRows(game);
        updateGhostPiece(game);
    };
}

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

void lockActivePieceOnBoard(Game *game) {
    for (int32_t i = 0; i < 4; i++) {
        Tile *tile = game->board.activePiece->tiles[i];
        game->board.playField[tile->x][tile->y] = tile;
    }
    free(game->board.activePiece);
    game->board.activePiece = NULL;
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
            }
            game->keyboard.keys[SDL_SCANCODE_RIGHT].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_LEFT].pressed) {
            if (canMovePiece(game, game->board.activePiece, -1, 0)) {
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

        if (game->keyboard.keys[SDL_SCANCODE_UP].pressed ||
            game->keyboard.keys[SDL_SCANCODE_X].pressed) {
            attemptActivePieceRotation(game, true);
            game->keyboard.keys[SDL_SCANCODE_UP].pressed = false;
            game->keyboard.keys[SDL_SCANCODE_X].pressed = false;
        }

        if (game->keyboard.keys[SDL_SCANCODE_Z].pressed) {
            attemptActivePieceRotation(game, false);
            game->keyboard.keys[SDL_SCANCODE_Z].pressed = false;
        }
    }
}

void doPieceLogic(Game *game) {
    // if piece doesn't exist, create one
    if (game->board.activePiece == NULL) {
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
            if (shouldDropPiece(game, 800)) {
                doPieceLogic(game);
            }
        }
        doInputLogic(game);
    };
}

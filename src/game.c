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

void destroyGameBoard(GameBoard *gameBoard){
    for (int32_t x = 0; x < GAME_WIDTH; x++){
        for (int32_t y = 0; y < GAME_HEIGHT; y++){
            if (gameBoard->playField[x][y] != NULL){
                free(gameBoard->playField[x][y]);
            }
        }
    }

    if (gameBoard->activePiece != NULL){
        for (uint32_t i = 0; i < 4; i++){
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
    uint32_t spawny = GAME_HEIGHT +1;
    game->board.activePiece = createNewPiece(spawnx, spawny, type);
}

bool doesTileExistAtCoordianate(Game *game, int32_t x, int32_t y){
    if (y >= GAME_HEIGHT){
        // we are allowed to spawn above game height, and no collision will occur here.
        return false;
    }
    return (game->board.playField[x][y] != NULL);
}

bool canActivePieceDrop(Game *game){
    for(uint32_t i = 0; i < 4; i++){
        Tile* tile = game->board.activePiece->tiles[i];
        int32_t newY = tile->y - 1;
        if (doesTileExistAtCoordianate(game, tile->x, newY)){
            return false;
        } else if(newY < 0){
            return false;
        }
    }
    return true;
}

void dropActivePiece(Game *game){
    for(int32_t i = 0; i < 4; i++){
        game->board.activePiece->tiles[i]->y -= 1;
    }
}

void lockActivePieceOnBoard(Game *game){
    for(int32_t i = 0; i < 4; i++){
        Tile* tile = game->board.activePiece->tiles[i];
        game->board.playField[tile->x][tile->y] = tile;
    }
    free(game->board.activePiece);
    game->board.activePiece = NULL;
}

void doPieceLogic(Game *game) {
    // if piece doesn't exist, create one
    if (game->board.activePiece == NULL) {
        createNextPiece(game);
    } else {
        if (canActivePieceDrop(game)){
            dropActivePiece(game);
        } else {
            lockActivePieceOnBoard(game);
        }
    }
}

void runGame(Game *game) {
    while (!processInput(&game->keyboard)) {
        updateDisplay(&game->media, &game->board);
        SDL_Delay(500);
        doPieceLogic(game);
    };
}

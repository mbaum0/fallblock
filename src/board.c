#include "board.h"

#define LOCK_DELAY 200

void stepActivePiece(GameBoard* board);
void stepGhostPiece(GameBoard* board);
uint32_t processCompletedRows(GameBoard* board);
void spawnNextPiece(GameBoard* board);
bool shouldActivePieceDrop(GameBoard* board);
bool canActivePieceDrop(GameBoard* board);
bool canPieceDrop(GameBoard* board, Piece* piece);
bool doesTileExistAtCoordianate(GameBoard *board, uint32_t x, uint32_t y);
void dropActivePiece(GameBoard* board);
bool shouldActivePieceLock(GameBoard* board);
uint32_t getElapsedTimeMS(uint64_t lastTick);
void lockActivePiece(GameBoard* board);



GameBoard* createGameBoard(void){
    GameBoard* newGame = calloc(1, sizeof(GameBoard));
    newGame->activePiece = NULL;
    newGame->ghostPiece = NULL;
    newGame->nextPieceType = getRandomPieceType();
    newGame->score = 0;
    newGame->level = 1;
    newGame->lastSuccessfulMoveTS = SDL_GetPerformanceTimer();
    newGame->lastDropTS = SDL_GetPerformanceTimer();

    return newGame;
}

void destroyGameBoard(GameBoard* board){
    safefree(board->activePiece);
    safefree(board->ghostPiece);
    safefree(board);
}

void dropActivePiece(GameBoard* board){
    movePiece(board->activePiece, 0, -1);
}

bool doesTileExistAtCoordianate(GameBoard *board, uint32_t x, uint32_t y){
    if (y >= GAME_HEIGHT || x > GAME_WIDTH){
        // out of bounds of the playing field
        return false;
    }

    return (board->playField[x][y] != NULL);
}


bool canPieceDrop(GameBoard* game, Piece* piece){
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

bool canActivePieceDrop(GameBoard *board) {
    return canPieceDrop(board, board->activePiece);
}

uint32_t getElapsedTimeMS(uint64_t lastTick){
    uint64_t currentTick = SDL_GetPerformanceCounter(); 
    uint64_t elapsedTicks = (currentTick - lastTick);
    float tickFreq = (float)SDL_GetPerformanceFrequency();
    float elapsedMS = (elapsedTicks / tickFreq) * 1000;
    return (uint32_t)elapsedMS;
}

bool shouldActivePieceDrop(GameBoard* board){
    if (getElapsedTimeMS(board->lastDropTS) >= board->dropDelayMS){
        // update new last drop time
        board->lastDropTS = SDL_GetPerformanceCounter();
        return true;
    }
    return false;
}

bool shouldActivePieceLock(GameBoard* board){
    return (getElapsedTimeMS(board->lastSuccessfulMoveTS) >= LOCK_DELAY);
}

void lockActivePiece(GameBoard* board){
    for (int32_t i = 0; i < 4; i++) {
        Tile *tile = board->activePiece->tiles[i];
        board->playField[tile->x][tile->y] = tile;
    }
    free(board->activePiece);

    for (uint32_t i = 0; i < 4; i++){
        free(board->ghostPiece->tiles[i]);
    }
    free(board->ghostPiece);

    board->activePiece = NULL;
    board->ghostPiece = NULL;
}

void stepActivePiece(GameBoard* board){
    if (board->activePiece == NULL){
        // no active piece, so we create a new one
        spawnNextPiece(board);
    }

    // check if a drop should occur. If so, drop the piece if possible
    if (shouldActivePieceDrop(board)){
        if (canActivePieceDrop(board)){
            dropActivePiece(board);
        }
    }

    // check if a piece can no longer fall. If so, lock it into place if necessary time as passed
    if (!canActivePieceDrop(board) && shouldActivePieceLock(board)){
        lockActivePiece(board);
    }
}

void spawnNextPiece(GameBoard* board){
    int32_t spawnX = GAME_WIDTH / 2;
    int32_t spawnY = GAME_HEIGHT;
    board->activePiece = createNewPiece(spawnX, spawnY, board->nextPieceType);
    board->nextPieceType = getRandomPieceType();
}

bool stepGameBoard(GameBoard* board){
    stepActivePiece(board);
    stepGhostPiece(board);
    processCompletedRows(board);
}

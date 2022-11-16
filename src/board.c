#include "board.h"

#define LOCK_DELAY 200

void stepActivePiece(GameBoard *board);
void stepGhostPiece(GameBoard *board);
void processCompletedRows(GameBoard *board);
void spawnNextPiece(GameBoard *board);
bool shouldActivePieceDrop(GameBoard *board);
bool canActivePieceDrop(GameBoard *board);
bool canPieceDrop(GameBoard *board, Piece *piece);
bool canPieceMove(GameBoard *board, Piece *piece, int32_t dx, int32_t dy);
bool canActivePieceMove(GameBoard *board, int32_t dx, int32_t dy);
bool doesTileExistAtCoordianate(GameBoard *board, uint32_t x, uint32_t y);
void dropActivePiece(GameBoard *board);
bool shouldActivePieceLock(GameBoard *board);
uint32_t getElapsedTimeMS(uint64_t lastTick);
void lockActivePiece(GameBoard *board);
void hardDropPiece(GameBoard *board, Piece *piece);
void dropTilesAboveRow(GameBoard *board, uint32_t row);
void updateScore(GameBoard *board, uint32_t numRows);
void updateLevel(GameBoard *board);
void updateDropDelay(GameBoard *board);
void attemptRotation(GameBoard *board, bool clockwise);
void attemptMoveLeft(GameBoard *board);
void attemptMoveRight(GameBoard *board);

GameBoard *createGameBoard(void) {
    GameBoard *newBoard = calloc(1, sizeof(GameBoard));
    newBoard->activePiece = NULL;
    newBoard->ghostPiece = NULL;
    newBoard->nextPieceType = getRandomPieceType();
    newBoard->score = 0;
    newBoard->level = 1;
    newBoard->lastSuccessfulMoveTS = SDL_GetPerformanceCounter();
    newBoard->lastDropTS = SDL_GetPerformanceCounter();
    updateDropDelay(newBoard);

    return newBoard;
}

void destroyGameBoard(GameBoard *board) {
    safefree(&board->activePiece);
    safefree(&board->ghostPiece);
    safefree(&board);
}

void dropActivePiece(GameBoard *board) {
    movePiece(board->activePiece, 0, -1);
}

bool doesTileExistAtCoordianate(GameBoard *board, uint32_t x, uint32_t y) {
    if (y >= GAME_HEIGHT || x > GAME_WIDTH) {
        // out of bounds of the playing field
        return false;
    }

    return (board->playField[x][y] != NULL);
}

bool canPieceDrop(GameBoard *board, Piece *piece) {
    for (uint32_t i = 0; i < 4; i++) {
        Tile *tile = piece->tiles[i];
        int32_t newY = tile->y - 1;
        if (doesTileExistAtCoordianate(board, tile->x, newY)) {
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

bool canPieceMove(GameBoard *board, Piece *piece, int32_t dx, int32_t dy){
    for (uint32_t i = 0; i < 4; i ++){
        int32_t newX = piece->tiles[i]->x + dx;
        int32_t newY = piece->tiles[i]->y + dy;
        if (newX < 0 || newX >= GAME_WIDTH || newY < 0){
            return false;
        }

        // check for collisions
        if (newY >=0 && newX >=0 && newY < GAME_HEIGHT){
            if (board->playField[newX][newY] != NULL){
                return false;
            }
        }
    }
    return true;
}

bool canActivePieceMove(GameBoard *board, int32_t dx, int32_t dy){
    return canPieceMove(board, board->activePiece, dx, dy);
}

uint32_t getElapsedTimeMS(uint64_t lastTick) {
    uint64_t currentTick = SDL_GetPerformanceCounter();
    uint64_t elapsedTicks = (currentTick - lastTick);
    float tickFreq = (float)SDL_GetPerformanceFrequency();
    float elapsedMS = (elapsedTicks / tickFreq) * 1000;
    return (uint32_t)elapsedMS;
}

bool shouldActivePieceDrop(GameBoard *board) {
    if (getElapsedTimeMS(board->lastDropTS) >= board->dropDelayMS) {
        // update new last drop time
        board->lastDropTS = SDL_GetPerformanceCounter();
        return true;
    }
    return false;
}

bool shouldActivePieceLock(GameBoard *board) {
    return (getElapsedTimeMS(board->lastSuccessfulMoveTS) >= LOCK_DELAY);
}

void lockActivePiece(GameBoard *board) {
    for (int32_t i = 0; i < 4; i++) {
        Tile *tile = board->activePiece->tiles[i];
        board->playField[tile->x][tile->y] = tile;
    }
    safefree(&board->activePiece);

    if (board->ghostPiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            safefree(&board->ghostPiece->tiles[i]);
        }
        safefree(&board->ghostPiece);
    }
}

void hardDropPiece(GameBoard *board, Piece *piece) {
    while (canPieceDrop(board, piece)) {
        movePiece(piece, 0, -1);
    }
}

void stepActivePiece(GameBoard *board) {
    if (board->activePiece == NULL) {
        // no active piece, so we create a new one
        spawnNextPiece(board);
    }

    // check if a drop should occur. If so, drop the piece if possible
    if (shouldActivePieceDrop(board)) {
        if (canActivePieceDrop(board)) {
            dropActivePiece(board);
        }
    }

    // check if a piece can no longer fall. If so, lock it into place if
    // necessary time as passed
    if (!canActivePieceDrop(board) && shouldActivePieceLock(board)) {
        lockActivePiece(board);
    }
}

void stepGhostPiece(GameBoard *board) {
    Piece *ghost = board->ghostPiece;
    Piece *active = board->activePiece;

    if (ghost == NULL && active != NULL) {
        // we need a new ghost piece, create one
        board->ghostPiece = createNewPiece(0, 0, active->pieceType);
        ghost = board->ghostPiece;
    }

    // update the ghost piece if needed
    if (ghost != NULL && active != NULL) {
        ghost->pieceState = active->pieceState;
        ghost->pieceType = active->pieceType;
        for (uint32_t i = 0; i < 4; i++) {
            ghost->tiles[i]->x = active->tiles[i]->x;
            ghost->tiles[i]->y = active->tiles[i]->y;
        }
        hardDropPiece(board, ghost);
    }
}

void spawnNextPiece(GameBoard *board) {
    int32_t spawnX = GAME_WIDTH / 2;
    int32_t spawnY = GAME_HEIGHT;
    board->activePiece = createNewPiece(spawnX, spawnY, board->nextPieceType);
    board->nextPieceType = getRandomPieceType();
}

void dropTilesAboveRow(GameBoard *board, uint32_t row) {
    for (uint32_t y = row + 1; y < GAME_HEIGHT; y++) {
        for (uint32_t x = 0; x < GAME_WIDTH; x++) {
            Tile *tile = board->playField[x][y];
            if (tile != NULL) {
                tile->y -= 1;
                board->playField[x][y - 1] = tile;
                board->playField[x][y] = NULL;
            }
        }
    }
}

void updateScore(GameBoard *board, uint32_t numRows) {
    uint32_t score = 0;
    switch (numRows) {
    case 1:
        score = 1;
        break;
    case 2:
        score = 3;
        break;
    case 3:
        score = 5;
        break;
    case 4:
        score = 8;
    default:
        score = 0;
        break;
    }
    board->score += score;
}

void updateLevel(GameBoard *board) {
    if (board->score >= board->level * 5) {
        board->level += 1;
    }
}

void updateDropDelay(GameBoard *board) {
    uint32_t level = board->level;
    float timeInSeconds = pow((0.8f - ((level - 1) * (0.007f))), level - 1);
    uint32_t timeInMS = (uint32_t)(timeInSeconds * 1000);
    board->dropDelayMS = timeInMS;
    log_debug("New drop delay: %d", timeInMS);
}

void processCompletedRows(GameBoard *board) {
    uint32_t numCompletedRows = 0;
    // check if a row is completed
    for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
        uint32_t rowSum = 0;
        for (uint32_t x = 0; x < GAME_WIDTH; x++) {
            if (board->playField[x][y] != NULL) {
                rowSum++;
            }
        }
        if (rowSum >= GAME_WIDTH) {
            // completed row :)
            numCompletedRows += 1;
            for (uint32_t x = 0; x < GAME_WIDTH; x++) {
                safefree(&board->playField[x][y]);
            }
            dropTilesAboveRow(board, y);
        }
    }

    updateScore(board, numCompletedRows);

    // if we completed some rows, update the level and drop delay
    if (numCompletedRows > 0) {
        updateLevel(board);
        updateDropDelay(board);
    }
}

void attemptRotation(GameBoard *board, bool clockwise){
    Piece* active = board->activePiece;
    if (clockwise){
        rotatePieceClockwise(active);
    } else {
        rotatePieceCounterClockwise(active);
    }

    if (canActivePieceMove(board, 0, 0)){
        // piece is valid
        return;
    }

    int32_t dx, dy = 0;
    getKickOffsets(active->pieceType, active->pieceState, KT_A, clockwise, &dx, &dy);
    if (canActivePieceMove(board, dx, dy)){
        movePiece(active, dx, dy);
        // piece is valid
        return;
    }

    getKickOffsets(active->pieceType, active->pieceState, KT_B, clockwise, &dx, &dy);
    if (canActivePieceMove(board, dx, dy)){
        movePiece(active, dx, dy);
        // piece is valid
        return;
    }

    getKickOffsets(active->pieceType, active->pieceState, KT_C, clockwise, &dx, &dy);
    if (canActivePieceMove(board, dx, dy)){
        movePiece(active, dx, dy);
        // piece is valid
        return;
    }

    getKickOffsets(active->pieceType, active->pieceState, KT_D, clockwise, &dx, &dy);
    if (canActivePieceMove(board, dx, dy)){
        movePiece(active, dx, dy);
        // piece is valid
        return;
    }

    // no kick worked, revert rotation
    if (clockwise){
        rotatePieceCounterClockwise(active);
    } else {
        rotatePieceClockwise(active);
    }

}
void attemptMoveLeft(GameBoard *board){
    if (canActivePieceMove(board, -1, 0)){
        movePiece(board->activePiece, -1, 0);
    }
}
void attemptMoveRight(GameBoard *board){
    if (canActivePieceMove(board, 1, 0)){
        movePiece(board->activePiece, 1, 0);
    }
}

void processCommandEvent(GameBoard *board, CommandEvent cmd) {
    if(board->activePiece == NULL){
        return;
    }
    switch (cmd) {
    case IE_ROTATE_CLOCKWISE:
        attemptRotation(board, true);
        break;
    case IE_ROTATE_COUNTER_CLOCKWISE:
        attemptRotation(board, false);
        break;
    case IE_HARD_DROP:
        hardDropPiece(board, board->activePiece);
    case IE_SOFT_DROP:
        break;
    case IE_MOVE_RIGHT:
        attemptMoveRight(board);
        break;
    case IE_MOVE_LEFT:
        attemptMoveLeft(board);
        break;
    default:
        break;
    }
    board->lastSuccessfulMoveTS = SDL_GetPerformanceCounter();
}

bool stepGameBoard(GameBoard *board) {
    stepActivePiece(board);
    stepGhostPiece(board);
    processCompletedRows(board);
    return false;
}

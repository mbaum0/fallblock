#include "game.h"

static bool movePieceLeft(Game *game, Piece *piece);
static bool movePieceRight(Game *game, Piece *piece);
static bool movePieceDown(Game *game, Piece *piece);
static bool rotatePiece(Game *game, Piece *piece, bool clockwise);
static bool isPieceValid(Game *game, Piece *piece);
static bool spaceIsFree(Game *game, uint32_t x, uint32_t y);
static bool isOffScreen(uint32_t x, uint32_t y);
static uint32_t checkForCompletedRows(Game *game);
static void lockCurrentPieceOnGameBoard(Game *game);
static void dropTilesAboveRow(Game *game, uint32_t row);
static bool createNextPiece(Game *game);
static float computeDropDelay(uint32_t level);
void processControls(Game *game);
void updateScore(Game *game, uint32_t numRows);
static bool shouldStep(Game *game);

const float secondsPerTick = 0.01667;

static bool movePieceLeft(Game *game, Piece *piece) {
    movePiece(piece, -1, 0);
    if (!isPieceValid(game, piece)) {
        movePiece(piece, 1, 0);
        return false;
    }
    return true;
}

static bool movePieceRight(Game *game, Piece *piece) {
    movePiece(piece, 1, 0);
    if (!isPieceValid(game, piece)) {
        movePiece(piece, -1, 0);
        return false;
    }
    return true;
}

static bool movePieceDown(Game *game, Piece *piece) {
    movePiece(piece, 0, 1);
    if (!isPieceValid(game, piece)) {
        movePiece(piece, 0, -1);
        return false;
    }
    return true;
}

static bool rotatePiece(Game *game, Piece *piece, bool clockwise) {
    if (clockwise) {
        rotatePieceClockwise(piece);
    } else {
        rotatePieceCounterClockwise(piece);
    }

    if (!isPieceValid(game, piece)) {
        if (clockwise) {
            rotatePieceCounterClockwise(piece);
        } else {
            rotatePieceClockwise(piece);
        }
        return false;
    }
    return true;
}

void processControls(Game *game) {
    if (game->stage->currentPiece != NULL) {
        if (game->keyboard[SDL_SCANCODE_LEFT]) {
            movePieceLeft(game, game->stage->currentPiece);
            game->keyboard[SDL_SCANCODE_LEFT] = 0;
        } else if (game->keyboard[SDL_SCANCODE_RIGHT]) {
            movePieceRight(game, game->stage->currentPiece);
            game->keyboard[SDL_SCANCODE_RIGHT] = 0;
        } else if (game->keyboard[SDL_SCANCODE_SPACE]) {
            rotatePiece(game, game->stage->currentPiece, true);
            game->keyboard[SDL_SCANCODE_SPACE] = 0;
        } else if (game->keyboard[SDL_SCANCODE_DOWN]) {
            if (game->ticks % 2 == 0) {
                if (!movePieceDown(game, game->stage->currentPiece)) {
                    game->keyboard[SDL_SCANCODE_DOWN] = false;
                }
            }
        }
    }
}

static bool createNextPiece(Game *game) {
    int x = GAME_WIDTH / 2;
    int y = 0;
    Piece *piece = createRandomNewPiece(x, y);
    free(game->stage->currentPiece);
    game->stage->currentPiece = piece;

    return (isPieceValid(game, piece));
}

static void lockCurrentPieceOnGameBoard(Game *game) {
    for (int i = 0; i < 4; i++) {
        Tile *b = game->stage->currentPiece->tiles[i];
        game->stage->board[b->x][b->y] = b;
    }
}

static uint32_t checkForCompletedRows(Game *game) {
    uint32_t numCompletedRows = 0;
    for (int row = 0; row < GAME_HEIGHT; row++) {
        int rowSum = 0;
        for (int col = 0; col < GAME_WIDTH; col++) {
            if (game->stage->board[col][row] != NULL) {
                rowSum++;
            }
        }
        if (rowSum == GAME_WIDTH) {
            printf("Row %d completed!\n", row);
            numCompletedRows++;
            for (int col = 0; col < GAME_WIDTH; col++) {
                free(game->stage->board[col][row]);
                game->stage->board[col][row] = NULL;
            }
            dropTilesAboveRow(game, row);
        }
        rowSum = 0;
    }
    return numCompletedRows;
}

static void dropTilesAboveRow(Game *game, uint32_t row) {
    for (int cRow = row; cRow > 0; cRow--) {
        Tile *t;
        for (int col = 0; col < GAME_WIDTH; col++) {
            if ((t = game->stage->board[col][cRow])) {
                game->stage->board[t->x][t->y] = NULL;
                t->y += 1;
                game->stage->board[t->x][t->y] = t;
            }
        }
    }
}

static float computeDropDelay(uint32_t level) {
    float res = (0.8 - ((level - 1) * 0.007));
    res = pow(res, level - 1);
    return res;
}

void initGame(Game *game) {
    game->stage = calloc(sizeof(Stage), 1);
    game->stage->level = 1;
    game->stage->dropDelay = computeDropDelay(1);
    if (!initMedia(game)) {
        printf("Failed to initialize media!\n");
        exit(1);
    }
    printf("Game initialized! Level %d, dropDelay: %f\n", game->stage->level,
           game->stage->dropDelay);
}

void updateScore(Game *game, uint32_t numRows) {
    if (numRows == 1) {
        game->stage->score += 1;
    } else if (numRows == 2) {
        game->stage->score += 3;
    } else if (numRows == 3) {
        game->stage->score += 5;
    } else if (numRows == 4) {
        game->stage->score += 8;
    }

    if (game->stage->score >= game->stage->level * 5) {
        game->stage->level++;
        game->stage->dropDelay = computeDropDelay(game->stage->level);
        printf("Starting level %d\n", game->stage->level);
        printf("New drop delay: %f\n", game->stage->dropDelay);
    }
}

void destroyGame(Game *game) {
    destroyMedia(game);
    for (uint32_t i = 0; i < GAME_WIDTH; i++) {
        for (uint32_t j = 0; j < GAME_HEIGHT; j++) {
            free(game->stage->board[i][j]);
        }
    }
    for (uint32_t i = 0; i < 4; i++) {
        free(game->stage->currentPiece->tiles[i]);
    }
    // destroyAllPieces(game->stage->currentPiece);
    free(game->stage->currentPiece);
    free(game->stage);
}

static bool shouldStep(Game *game) {
    float elapsedTime = game->ticks * secondsPerTick;
    return (elapsedTime >= game->stage->dropDelay);
}

bool doLogic(Game *game) {
    processControls(game);
    if (shouldStep(game)) {
        if (game->stage->currentPiece == NULL) {
            createNextPiece(game);
        } else {
            Piece *piece = game->stage->currentPiece;

            if (!movePieceDown(game, piece)) {
                // couldn't move piece down. lock in the tiles
                lockCurrentPieceOnGameBoard(game);
                uint32_t numCompletedRows = checkForCompletedRows(game);
                updateScore(game, numCompletedRows);
                if (!createNextPiece(game)){
                    // piece is invalid, game over
                    return true;
                }
            }
        }
        game->ticks = 0;
    } else {
        game->ticks++;
    }
    return false;
}

static bool isOffScreen(uint32_t x, uint32_t y) {
    return (y >= GAME_HEIGHT || x >= GAME_WIDTH);
}

static bool isPieceValid(Game *game, Piece *piece) {
    for (uint32_t i = 0; i < 4; i++) {
        Tile *t = piece->tiles[i];
        if (isOffScreen(t->x, t->y) || !spaceIsFree(game, t->x, t->y)) {
            return false;
        }
    }
    return true;
}

static bool spaceIsFree(Game *game, uint32_t x, uint32_t y) {
    return (game->stage->board[x][y] == NULL);
}

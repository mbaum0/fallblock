#pragma once

typedef struct GameBoard GameBoard;

#include <stdint.h>
#include "piece.h"
#include <stdbool.h>
#include "util.h"
#include "SDL.h"

#define GAME_WIDTH 10
#define GAME_HEIGHT 20

/**
 * A GameBoard is a model of a fallblock game board.
*/
struct GameBoard {
    Piece *activePiece;
    Piece *ghostPiece;
    PieceType nextPieceType;
    Tile *playField[GAME_WIDTH][GAME_HEIGHT];
    uint32_t score;
    uint32_t level;
    uint32_t dropDelayMS;
    uint64_t lastSuccessfulMoveTS;
    uint64_t lastDropTS;
};

typedef enum { IE_ROTATE_CLOCKWISE, IE_ROTATE_COUNTER_CLOCKWISE, IE_HARD_DROP, IE_SOFT_DROP, IE_MOVE_RIGHT, IE_MOVE_LEFT } InputEvent;

/**
 * @brief Create a GameBoard object
 * 
 * @return GameBoard* new initialized Board instance
 */
GameBoard* createGameBoard(void);

/**
 * @brief Destroy a gameboard object and its internals
 * 
 * @param board GameBoard* to destroy
 */
void destroyGameBoard(GameBoard* board);

/**
 * @brief Step gameboard logic. This function should be call
 * repeatedly until true is returned.
 * 
 * @param board The GameBoard to step
 * @return true if the game is over
 * @return false if the game is not over
 */
bool stepGameBoard(GameBoard* board);

/**
 * @brief Send a key event to update the gameboard
 * 
 * @param board 
 * @param key 
 */
void processKeyEvent(GameBoard* board, InputEvent key);

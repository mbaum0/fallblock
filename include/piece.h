#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Piece Piece;
typedef struct Tile Tile;
typedef struct TileLocation TileLocation;

typedef enum {
    Piece_I,
    Piece_J,
    Piece_L,
    Piece_O,
    Piece_S,
    Piece_T,
    Piece_Z
} PieceType;

typedef enum {
    TC_Red,
    TC_Blue,
    TC_Yellow,
    TC_Purple,
    TC_Orange,
    TC_Green,
    TC_Pink
} TileColor;

typedef enum { PS_A, PS_B, PS_C, PS_D } PieceState;

struct Tile {
    int32_t x;
    int32_t y;
    TileColor color;
};

struct Piece {
    Tile *tiles[4];
    PieceType pieceType;
    PieceState pieceState;
};

/**
 * @brief Create a New Piece
 *
 * @param x game x-coordinate
 * @param y game y coordinate
 * @param type PieceType to create
 * @return Piece*
 */
Piece *createNewPiece(int32_t x, int32_t y, PieceType type);

/**
 * @brief Move a Piece
 *
 * @param piece piece to move
 * @param dx number of game coordinates to move horizontally (may be negative)
 * @param dy number of game coordiantes to move vertically (may be negative)
 */
void movePiece(Piece *piece, int32_t dx, int32_t dy);

/**
 * @brief Rotate a piece clockwise
 *
 * @param piece piece to rotate
 */
void rotatePieceClockwise(Piece *piece);

/**
 * @brief Rotate a piece counter-clockwise
 *
 * @param piece piece to rotate
 */
void rotatePieceCounterClockwise(Piece *piece);

/**
 * @brief Get a random PieceType
 *
 * @return PieceType
 */
PieceType getRandomPieceType(void);

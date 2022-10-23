#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Piece Piece;
typedef struct Tile Tile;

typedef enum {
    RightL = 0,
    LeftL = 1,
    Straight = 2,
    Square = 3,
    LeftSkew = 4,
    RightSkew = 5,
    Tee = 6
} PieceType;
typedef enum {
    Yellow = 0,
    Pink = 1,
    Teal = 2,
    Orange = 3,
    Purple = 4,
    Grey = 5,
    Sky = 6
} TileColor;
typedef enum { A, B, C, D } PieceOrientation;

struct Tile {
    uint32_t x;
    uint32_t y;
    TileColor color;
};

struct Piece {
    Tile *tiles[4];
    PieceType pieceType;
    PieceOrientation orientation;
};

Piece *createNewPiece(uint32_t x, uint32_t y, PieceType type);
Piece *createRandomNewPiece(uint32_t x, uint32_t y);
void destroyAllPieces(Piece *piece);
void movePiece(Piece *piece, uint32_t dx, uint32_t dy);
void rotatePieceClockwise(Piece *piece);
void rotatePieceCounterClockwise(Piece *piece);

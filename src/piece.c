#include "piece.h"

void setPieceI(Piece *piece);
void setPieceJ(Piece *piece);
void setPieceL(Piece *piece);
void setPieceO(Piece *piece);
void setPieceS(Piece *piece);
void setPieceT(Piece *piece);
void setPieceZ(Piece *piece);
TileColor getColorForType(PieceType type);

void movePiece(Piece *piece, int32_t dx, int32_t dy) {
    for (uint32_t i = 0; i < 4; i++) {
        piece->tiles[i]->x += dx;
        piece->tiles[i]->y += dy;
    }
}

PieceType getRandomPieceType(void) {
    return (PieceType)(rand() % 7);
}

TileColor getColorForType(PieceType type) {
    switch (type) {
    case Piece_I:
        return TC_Blue;
    case Piece_J:
        return TC_Purple;
    case Piece_L:
        return TC_Orange;
    case Piece_O:
        return TC_Yellow;
    case Piece_S:
        return TC_Green;
    case Piece_T:
        return TC_Pink;
    case Piece_Z:
        return TC_Red;
    }
}

void setPieceI(Piece *piece) {
    piece->tiles[1]->x -= 1;
    piece->tiles[2]->x += 1;
    piece->tiles[3]->x += 2;
}
void setPieceJ(Piece *piece) {
    piece->tiles[1]->x -= 1;
    piece->tiles[2]->x -= 1;
    piece->tiles[2]->y += 1;
    piece->tiles[3]->x += 1;
}
void setPieceL(Piece *piece) {
    piece->tiles[1]->x -= 1;
    piece->tiles[2]->x += 1;
    piece->tiles[3]->x += 1;
    piece->tiles[3]->y += 1;
}
void setPieceO(Piece *piece) {
    piece->tiles[1]->x += 1;
    piece->tiles[2]->x += 1;
    piece->tiles[2]->y += 1;
    piece->tiles[3]->y += 1;
}
void setPieceS(Piece *piece) {
    piece->tiles[1]->x -= 1;
    piece->tiles[2]->y += 1;
    piece->tiles[3]->x += 1;
    piece->tiles[3]->y += 1;
}
void setPieceT(Piece *piece) {
    piece->tiles[1]->x -= 1;
    piece->tiles[2]->x += 1;
    piece->tiles[3]->y += 1;
}
void setPieceZ(Piece *piece) {
    piece->tiles[1]->x += 1;
    piece->tiles[2]->y += 1;
    piece->tiles[3]->x += 1;
    piece->tiles[3]->y += 1;
}

Piece *createNewPiece(int32_t x, int32_t y, PieceType type) {
    Piece *newPiece = calloc(sizeof(Piece), 1);
    newPiece->pieceState = PS_A;
    newPiece->pieceType = type;
    for (uint32_t i = 0; i < 4; i++) {
        newPiece->tiles[i] = calloc(sizeof(Tile), 1);
        newPiece->tiles[i]->color = getColorForType(type);
        newPiece->tiles[i]->x = x;
        newPiece->tiles[i]->y = y;
    }
    switch (type) {
    case Piece_I:
        setPieceI(newPiece);
        break;
    case Piece_J:
        setPieceJ(newPiece);
        break;
    case Piece_L:
        setPieceL(newPiece);
        break;
    case Piece_O:
        setPieceO(newPiece);
        break;
    case Piece_S:
        setPieceS(newPiece);
        break;
    case Piece_T:
        setPieceT(newPiece);
        break;
    case Piece_Z:
        setPieceZ(newPiece);
        break;
    }
    return newPiece;
}

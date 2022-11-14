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
    default:
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
    piece->tiles[1]->y += 1;
    piece->tiles[2]->y += 1;
    piece->tiles[2]->x -= 1;
    piece->tiles[3]->x += 1;
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

void rotatePieceCounterClockwise(Piece *piece) {
    if (piece->pieceType == Piece_O) {
        // don't rotate O type
        return;
    }

    Tile *baseTile = piece->tiles[0];
    PieceState nextState = PS_A;
    for (uint32_t i = 1; i < 4; i++) {
        Tile *tile = piece->tiles[i];
        int32_t shiftX = piece->tiles[i]->x - baseTile->x;
        int32_t shiftY = piece->tiles[i]->y - baseTile->y;

        switch (piece->pieceState) {
        case PS_A:
            tile->x = (shiftY * -1) + baseTile->x;
            tile->y = shiftX + baseTile->y;
            nextState = PS_B;
            break;
        case PS_B:
            tile->x = (shiftY * -1) + baseTile->x;
            tile->y = shiftX + baseTile->y;
            nextState = PS_C;
            break;
        case PS_C:
            tile->x = (shiftY * -1) + baseTile->x;
            tile->y = shiftX + baseTile->y;
            nextState = PS_D;
            break;
        case PS_D:
            tile->x = (shiftY * -1) + baseTile->x;
            tile->y = shiftX + baseTile->y;
            nextState = PS_A;
            break;
        }
    }
    piece->pieceState = nextState;
}

void rotatePieceClockwise(Piece *piece) {
    if (piece->pieceType == Piece_O) {
        // don't rotate O type
        return;
    }

    Tile *baseTile = piece->tiles[0];
    PieceState nextState = PS_A;
    for (uint32_t i = 1; i < 4; i++) {
        Tile *tile = piece->tiles[i];
        int32_t shiftX = piece->tiles[i]->x - baseTile->x;
        int32_t shiftY = piece->tiles[i]->y - baseTile->y;

        switch (piece->pieceState) {
        case PS_A:
            tile->x = shiftY + baseTile->x;
            tile->y = (shiftX * -1) + baseTile->y;
            nextState = PS_D;
            break;
        case PS_B:
            tile->x = shiftY + baseTile->x;
            tile->y = (shiftX * -1) + baseTile->y;
            nextState = PS_A;
            break;
        case PS_C:
            tile->x = shiftY + baseTile->x;
            tile->y = (shiftX * -1) + baseTile->y;
            nextState = PS_B;
            break;
        case PS_D:
            tile->x = shiftY + baseTile->x;
            tile->y = (shiftX * -1) + baseTile->y;
            nextState = PS_C;
            break;
        }
    }
    piece->pieceState = nextState;
}

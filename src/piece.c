#include "piece.h"

static const uint32_t transformRightL[4][8] = {{0, 2, 1, 1, 2, 0, 1, -1},
                                               {1, 0, 0, -1, -1, -2, -2, -1},
                                               {1, -1, 0, 0, -1, 1, 0, 2},
                                               {-2, -1, -1, 0, 0, 1, 1, 0}};

static const uint32_t transformLeftL[4][8] = {{-1, 1, 0, 0, 1, -1, 2, 0},
                                              {0, 1, -1, 0, -2, -1, -1, -2},
                                              {2, 0, 1, 1, 0, 2, -1, 1},
                                              {-1, -2, 0, -1, 1, 0, 0, 1}};

static const uint32_t transformStraight[2][8] = {{-1, 2, 0, 1, 1, 0, 2, -1},
                                                 {1, -2, 0, -1, -1, 0, -2, 1}};

static const uint32_t transformLeftSkew[2][8] = {{2, -1, 1, 0, 0, -1, -1, 0},
                                                 {-2, 1, -1, 0, 0, 1, 1, 0}};

static const uint32_t transformRightSkew[2][8] = {{2, 0, 1, -1, 0, 0, -1, -1},
                                                  {-2, 0, -1, 1, 0, 0, 1, 1}};

static const uint32_t transformTee[4][8] = {{-1, 1, 1, 1, 0, 0, -1, -1},
                                            {1, 1, 1, -1, 0, 0, -1, 1},
                                            {1, -1, -1, -1, 0, 0, 1, 1},
                                            {-1, -1, -1, 1, 0, 0, 1, -1}};

TileColor getTypeColor(PieceType type);
void makePieceRightL(Piece *piece);
void makePieceLeftL(Piece *newPiece);
void makePieceStraight(Piece *newPiece);
void makePieceSquare(Piece *newPiece);
void makePieceLeftSkew(Piece *newPiece);
void makePieceRightSkew(Piece *newPiece);
void makePieceTee(Piece *newPiece);
void rotateRightL(Piece *piece);
void rotateLeftL(Piece *piece);
void rotateStraight(Piece *piece);
void rotateLeftSkew(Piece *piece);
void rotateRightSkew(Piece *piece);
void rotateTee(Piece *piece);
void applyTransformation(Piece *piece, const uint32_t *transformation);

TileColor getTypeColor(PieceType type) {
    switch (type) {
    case RightL:
        return Yellow;
    case LeftL:
        return Pink;
    case Straight:
        return Teal;
    case Square:
        return Orange;
    case LeftSkew:
        return Purple;
    case RightSkew:
        return Grey;
    case Tee:
        return Sky;
    default:
        return Sky;
    }
}
static const PieceType typeArray[] = {RightL,   LeftL,     Straight, Square,
                                      LeftSkew, RightSkew, Tee};

PieceType getRandomPieceType(void) {
    int idx = (rand() % (7));
    return typeArray[idx];
}

Piece *createRandomNewPiece(int32_t x, int32_t y) {
    return createNewPiece(x, y, getRandomPieceType());
}

/**
 * Create a new tetromino piece with coordinates x, y
 */
Piece *createNewPiece(int32_t x, int32_t y, PieceType type) {
    Piece *newPiece = calloc(sizeof(Piece), 1);
    newPiece->orientation = 0;
    newPiece->pieceType = type;
    for (int i = 0; i < 4; i++) {
        newPiece->tiles[i] = calloc(sizeof(Tile), 1);
        newPiece->tiles[i]->color = getTypeColor(type);
        newPiece->tiles[i]->x = x;
        newPiece->tiles[i]->y = y;
    }

    switch (type) {
    case RightL:
        makePieceRightL(newPiece);
        break;
    case LeftL:
        makePieceLeftL(newPiece);
        break;
    case Straight:
        makePieceStraight(newPiece);
        break;
    case Square:
        makePieceSquare(newPiece);
        break;
    case LeftSkew:
        makePieceLeftSkew(newPiece);
        break;
    case RightSkew:
        makePieceRightSkew(newPiece);
        break;
    case Tee:
        makePieceTee(newPiece);
        break;
    }

    return newPiece;
}

void destroyPiece(Piece *piece) {
    for (uint32_t i = 0; i < 4; i++) {
        free(piece->tiles[i]);
    }
    free(piece);
}

void movePiece(Piece *piece, int32_t dx, int32_t dy) {
    for (int i = 0; i < 4; i++) {
        piece->tiles[i]->x += dx;
        piece->tiles[i]->y += dy;
    }
}

void makePieceRightL(Piece *piece) {
    piece->tiles[1]->y += 1;
    piece->tiles[2]->y += 2;
    piece->tiles[3]->y += 2;
    piece->tiles[3]->x += 1;
}

void makePieceLeftL(Piece *piece) {
    piece->tiles[0]->x += 1;
    piece->tiles[1]->x += 1;
    piece->tiles[1]->y += 1;
    piece->tiles[2]->x += 1;
    piece->tiles[2]->y += 2;
    piece->tiles[3]->y += 2;
}

void makePieceStraight(Piece *piece) {
    piece->tiles[0]->x += 1;
    piece->tiles[1]->x += 1;
    piece->tiles[1]->y += 1;
    piece->tiles[2]->x += 1;
    piece->tiles[2]->y += 2;
    piece->tiles[3]->x += 1;
    piece->tiles[3]->y += 3;
}

void makePieceSquare(Piece *piece) {
    piece->tiles[1]->x += 1;
    piece->tiles[2]->y += 1;
    piece->tiles[3]->x += 1;
    piece->tiles[3]->y += 1;
}

void makePieceLeftSkew(Piece *piece) {
    piece->tiles[0]->y += 1;
    piece->tiles[1]->x += 1;
    piece->tiles[1]->y += 1;
    piece->tiles[2]->x += 1;
    piece->tiles[2]->y += 2;
    piece->tiles[3]->x += 2;
    piece->tiles[3]->y += 2;
}

void makePieceRightSkew(Piece *piece) {
    piece->tiles[0]->y += 1;
    piece->tiles[1]->x += 1;
    piece->tiles[1]->y += 1;
    piece->tiles[2]->x += 1;
    piece->tiles[3]->x += 2;
}

void makePieceTee(Piece *piece) {
    piece->tiles[0]->x += 1;
    piece->tiles[1]->y += 1;
    piece->tiles[2]->x += 1;
    piece->tiles[2]->y += 1;
    piece->tiles[3]->x += 2;
    piece->tiles[3]->y += 1;
}

void rotatePieceCounterClockwise(Piece *piece) {
    rotatePieceClockwise(piece);
    rotatePieceClockwise(piece);
    rotatePieceClockwise(piece);
}

void rotatePieceClockwise(Piece *piece) {
    switch (piece->pieceType) {
    case RightL:
        rotateRightL(piece);
        break;
    case LeftL:
        rotateLeftL(piece);
        break;
    case Straight:
        rotateStraight(piece);
        break;
    case Square:
        break;
    case LeftSkew:
        rotateLeftSkew(piece);
        break;
    case RightSkew:
        rotateRightSkew(piece);
        break;
    case Tee:
        rotateTee(piece);
        break;
    }
}

void applyTransformation(Piece *piece, const uint32_t *transformation) {
    for (int i = 0; i < 4; i++) {
        piece->tiles[i]->x += transformation[i * 2];
        piece->tiles[i]->y += transformation[(i * 2) + 1];
    }
}
void rotateRightL(Piece *piece) {
    switch (piece->orientation) {
    case A:
        applyTransformation(piece, transformRightL[0]);
        piece->orientation = B;
        break;
    case B:
        applyTransformation(piece, transformRightL[1]);
        piece->orientation = C;
        break;
    case C:
        applyTransformation(piece, transformRightL[2]);
        piece->orientation = D;
        break;
    case D:
        applyTransformation(piece, transformRightL[3]);
        piece->orientation = A;
        break;
    }
}
void rotateLeftL(Piece *piece) {
    switch (piece->orientation) {
    case A:
        applyTransformation(piece, transformLeftL[0]);
        piece->orientation = B;
        break;
    case B:
        applyTransformation(piece, transformLeftL[1]);
        piece->orientation = C;
        break;
    case C:
        applyTransformation(piece, transformLeftL[2]);
        piece->orientation = D;
        break;
    case D:
        applyTransformation(piece, transformLeftL[3]);
        piece->orientation = A;
        break;
    }
}
void rotateStraight(Piece *piece) {
    switch (piece->orientation) {
    case A:
        applyTransformation(piece, transformStraight[0]);
        piece->orientation = B;
        break;
    case B:
        applyTransformation(piece, transformStraight[1]);
        piece->orientation = A;
        break;
    default:
        break;
    }
}
void rotateLeftSkew(Piece *piece) {
    switch (piece->orientation) {
    case A:
        applyTransformation(piece, transformLeftSkew[0]);
        piece->orientation = B;
        break;
    case B:
        applyTransformation(piece, transformLeftSkew[1]);
        piece->orientation = A;
        break;
    default:
        break;
    }
}
void rotateRightSkew(Piece *piece) {
    switch (piece->orientation) {
    case A:
        applyTransformation(piece, transformRightSkew[0]);
        piece->orientation = B;
        break;
    case B:
        applyTransformation(piece, transformRightSkew[1]);
        piece->orientation = A;
        break;
    default:
        break;
    }
}
void rotateTee(Piece *piece) {
    switch (piece->orientation) {
    case A:
        applyTransformation(piece, transformTee[0]);
        piece->orientation = B;
        break;
    case B:
        applyTransformation(piece, transformTee[1]);
        piece->orientation = C;
        break;
    case C:
        applyTransformation(piece, transformTee[2]);
        piece->orientation = D;
        break;
    case D:
        applyTransformation(piece, transformTee[3]);
        piece->orientation = A;
        break;
    }
}

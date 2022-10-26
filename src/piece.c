#include "piece.h"

PieceType getRandomPieceType(void) {
    return (PieceType)(rand() % 7);
}

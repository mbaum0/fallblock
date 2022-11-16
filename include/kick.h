#pragma once

#include "piece.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KT_A = 0,
    KT_B = 1,
    KT_C = 2,
    KT_D = 3,
} KickTest;

/**
 * @brief Returns the kick deltas for each kick type. There are eight kicks
 * total for each tetrimino. Four for each rotation direction
 *
 * @param pt The piece type to get kick offsets for
 * @param ps The piece state
 * @param kt The kick test to get offsets for
 * @param clockwise The rotation direction
 * @param dx Sets to x kick offset
 * @param dy Sets to y kick offset
 */
void getKickOffsets(PieceType pt, PieceState ps, KickTest kt, bool clockwise,
                    int32_t *dx, int32_t *dy);

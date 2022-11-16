#pragma once
#include "board.h"
#include <stdint.h>
#include <stdio.h>

typedef struct WirePacket WirePacket;
typedef struct WireTile WireTile;

struct WireTile {
    uint8_t x;
    uint8_t y;
    uint8_t color;
    uint8_t padding;
};

struct WirePacket {
    uint32_t level;
    uint32_t score;
    uint32_t numTiles;
    WireTile tiles[];
};

/**
 * Maximum packet size is space for the level, score, num tiles, and
 * a tile for each place on the gameboard.
 */
#define MAX_PACKET_SIZE (32 + 32 + 32 + 32 * GAME_HEIGHT * GAME_WIDTH)

/**
 * @brief Serialize the board and return a pointer to the data
 *
 * Serialization strategy is as follows
 * | 32 - Level | 32 - Score | 32 - Num Tiles |
 * | Num Tiles * 32 ...|
 *
 * tiles are serialized as follows:
 * | 8 - x | 8 - y | 8 - color | 8 - not used |
 * @param board The game board to serialize
 * @param dst Pointer to destination of serialized data
 * @return length of serialized data
 */
uint8_t *serializeBoard(GameBoard *board);

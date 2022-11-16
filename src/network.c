#include "network.h"

uint32_t serializeBoard(GameBoard *board, uint8_t** dst) {
    uint8_t *data = calloc(1, MAX_PACKET_SIZE);
    uint32_t dataLen = 0;

    memset(data, board->level, sizeof(uint32_t));
    dataLen += sizeof(uint32_t);

    memset(data + dataLen, board->score, sizeof(uint32_t));
    dataLen += sizeof(uint32_t);

    WireTile wt;
    for (uint32_t x = 0; x < GAME_WIDTH; x++) {
        for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
            Tile* tile = board->playField[x][y];
            if (tile != NULL) {
                wt.color = tile->color;
                wt.x = tile->x;
                wt.y = tile->y;
                memset(data + dataLen, &wt, sizeof(WireTile));
                dataLen += sizeof(WireTile);
            }
        }
    }
    *dst = data;
    return dataLen;
}

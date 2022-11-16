#pragma once
#include "board.h"
#include <SDL_net.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>

typedef struct WirePacket WirePacket;
typedef struct WireTile WireTile;
typedef struct NetKit NetKit;

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

struct NetKit {
    IPaddress remoteIP;
    UDPsocket udpsock;
    UDPpacket *recv_packet;
    SDLNet_SocketSet socketset;
    uint32_t numused;
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
uint32_t serializeBoard(GameBoard *board, uint8_t **dst);

/**
 * @brief Deserialize byte data to a target gameboard
 *
 * @param target GameBoard instance to update with src data
 * @param src Byte data containing a serialized WirePacket
 */
void deserializeBoard(GameBoard *target, uint8_t *src);

NetKit *initNetworking(void);
void destroyNetworking(NetKit *kit);

void checkForPackets(NetKit *kit);

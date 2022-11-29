#pragma once
#include "board.h"
#include <SDL_net.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>

#define PACKED __attribute__((__packed__))

typedef struct WireBoard WireBoard;
typedef struct WireTile WireTile;
typedef struct NetKit NetKit;
typedef struct SyncPacket SyncPacket;

struct SyncPacket {
    uint32_t magic;
    uint32_t stage;
    uint32_t gameNum;
};

struct WireTile {
    uint8_t x;
    uint8_t y;
    uint8_t color;
};

struct WireBoard {
    uint32_t level;
    uint32_t score;
    uint32_t numTiles;
    WireTile tiles[GAME_HEIGHT * GAME_WIDTH];
};

struct NetKit {
    IPaddress remoteIP;
    UDPsocket sock;
    UDPpacket *recvPacket;
    SDLNet_SocketSet socketset;
    uint32_t gameNum;
    uint16_t localPort;
    uint16_t remotePort;
    bool connected;
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
// uint32_t serializeBoard(GameBoard *board, uint8_t **dst);

/**
 * @brief Deserialize byte data to a target gameboard
 *
 * @param target GameBoard instance to update with src data
 * @param src Byte data containing a serialized WirePacket
 */
// void deserializeBoard(GameBoard *target, uint8_t *src);

NetKit *initNetworking(uint16_t localPort, uint16_t remotePort);
void destroyNetworking(NetKit *kit);

void checkForBoardPacket(NetKit *kit, GameBoard *board);
void sendBoardPacket(NetKit *kit, GameBoard *board);
void configureClient(NetKit *kit);
void configureServer(NetKit *kit);
void listenForConnections(NetKit *kit);

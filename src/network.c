#include "network.h"

uint32_t serializeBoard(GameBoard *board, uint8_t **dst) {
    uint8_t *data = calloc(1, MAX_PACKET_SIZE);
    uint32_t dataLen = 0;
    uint32_t numTiles = 0;
    uint8_t *numTilesLoc;

    memcpy(data, &board->level, sizeof(uint32_t));
    dataLen += sizeof(uint32_t);

    memcpy(data + dataLen, &board->score, sizeof(uint32_t));
    dataLen += sizeof(uint32_t);

    // can't write this until we know how many tiles we have, so skip
    // for now and save the pointer for later use
    numTilesLoc = data + dataLen;
    dataLen += sizeof(uint32_t);

    WireTile wt;
    for (uint32_t x = 0; x < GAME_WIDTH; x++) {
        for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
            Tile *tile = board->playField[x][y];
            if (tile != NULL) {
                wt.color = tile->color;
                wt.x = tile->x;
                wt.y = tile->y;
                memcpy(data + dataLen, &wt, sizeof(WireTile));
                dataLen += sizeof(WireTile);
                numTiles++;
            }
        }
    }

    memcpy(numTilesLoc, &numTiles, sizeof(uint32_t));

    *dst = data;
    return dataLen;
}

void deserializeBoard(GameBoard *target, uint8_t *src) {
    WirePacket *wp = (WirePacket *)(src);
    target->level = wp->level;
    target->score = wp->score;

    for (uint32_t i = 0; i < wp->numTiles; i++) {
        WireTile t = wp->tiles[i];
        target->playField[t.x][t.y]->color = t.color;
        target->playField[t.x][t.y]->x = t.x;
        target->playField[t.x][t.y]->y = t.y;
    }
}

NetKit *initNetworking(void) {
    NetKit *kit = calloc(1, sizeof(NetKit));
    if (SDLNet_Init() < 0) {
        log_error("Couldn't initialize SDL: %s", SDLNet_GetError());
        return false;
    }

    kit->udpsock = SDLNet_UDP_Open(3333);
    if (!kit->udpsock) {
        log_error("Failed to open socket: %s", SDLNet_GetError());
        return false;
    } else {
        log_debug("Listening for remotes on 0.0.0.0:3333");
    }

    kit->socketset = SDLNet_AllocSocketSet(2);
    if (kit->socketset == NULL) {
        log_error("Couldn't create socket set: %s", SDLNet_GetError());
        return false;
    }

    kit->recv_packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (kit->recv_packet == NULL) {
        log_error("Failed to allocate packet: %s", SDLNet_GetError());
    }
    return kit;
}

void destroyNetworking(NetKit *kit) {
    SDLNet_FreePacket(kit->recv_packet);
    SDLNet_Quit();
    safefree(&kit);
}

void checkForPackets(NetKit *kit) {
    SDLNet_CheckSockets(kit->socketset, ~0);
    if (SDLNet_SocketReady(kit->udpsock)) {
        if (SDLNet_UDP_Recv(kit->udpsock, kit->recv_packet)) {
            log_debug("Got %u bytes", kit->recv_packet->len);
        }
    }
}

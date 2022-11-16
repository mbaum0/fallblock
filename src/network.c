#include "network.h"

void getIPAsString(uint32_t ipaddr, char *dst);

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

    if (board->activePiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = board->activePiece->tiles[i];
            wt.color = tile->color;
            wt.x = tile->x;
            wt.y = tile->y;
            memcpy(data + dataLen, &wt, sizeof(WireTile));
            dataLen += sizeof(WireTile);
            numTiles++;
        }
    }

    memcpy(numTilesLoc, &numTiles, sizeof(uint32_t));

    *dst = data;
    return dataLen;
}

void deserializeBoard(GameBoard *target, uint8_t *src) {
    WirePacket *wp = (WirePacket *)src;
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
        return NULL;
    }
    kit->socks = SDLNet_AllocSocketSet(1);
    return kit;
}

void configureServer(NetKit *kit) {
    if (SDLNet_ResolveHost(&kit->localIP, NULL, 3333) == -1) {
        log_error("Couldn't resolve host: %s", SDLNet_GetError());
        return;
    }

    kit->socket = SDLNet_TCP_Open(&kit->localIP);
    if (kit->socket == NULL) {
        log_error("Couldn't open server: %s", SDLNet_GetError());
        return;
    }

    char str[INET_ADDRSTRLEN];
    getIPAsString(kit->localIP.host, str);
    log_debug("Listening for client on %s:%u", str, 3333);
    while (!kit->connected) {
        SDL_Delay(1000);
        TCPsocket tmp = SDLNet_TCP_Accept(kit->socket);
        if (tmp == NULL) {
            // no client accepted
            SDL_Delay(100);
            continue;
        }
        kit->socket = tmp;

        IPaddress *remoteip;
        remoteip = SDLNet_TCP_GetPeerAddress(kit->socket);
        if (!remoteip) {
            log_error("Couldn't resolve peer address: %s", SDLNet_GetError());
            continue;
        }
        kit->remoteIP = *remoteip;

        // get ip and port number
        uint32_t ipaddr;
        ipaddr = SDL_SwapBE32(kit->remoteIP.host);
        log_debug("Accepted connection from %d.%d.%d.%d port %hu", ipaddr >> 24,
                  (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff,
                  kit->remoteIP.host);
        SDLNet_TCP_AddSocket(kit->socks, kit->socket);
        kit->connected = true;
    }
}

void getIPAsString(uint32_t ipaddr, char *dst) {
    sprintf(dst, "%u.%u.%u.%u", (ipaddr >> 24), (ipaddr >> 16) & 0xff,
            (ipaddr >> 8) & 0xff, ipaddr & 0xff);
}

void configureClient(NetKit *kit) {
    while (!kit->connected) {
        SDL_Delay(1000);
        if (SDLNet_ResolveHost(&kit->remoteIP, "localhost", 3333) == -1) {
            log_error("Couldn't resolve remote ip: %s", SDLNet_GetError());
            continue;
        }

        kit->socket = SDLNet_TCP_Open(&kit->remoteIP);
        if (!kit->socket) {
            log_error("Failed to open socket: %s", SDLNet_GetError());
            continue;
        }
        SDLNet_TCP_AddSocket(kit->socks, kit->socket);
        kit->connected = true;
    }
}

void destroyNetworking(NetKit *kit) {
    SDLNet_TCP_Close(kit->socket);
    SDLNet_FreeSocketSet(kit->socks);
    SDLNet_Quit();
    safefree(&kit);
}

void checkForBoardPacket(NetKit *kit, GameBoard *board) {
    uint8_t data[MAX_PACKET_SIZE];
    if (SDLNet_CheckSockets(kit->socks, 0)) {
        uint32_t dataLen = SDLNet_TCP_Recv(kit->socket, data, MAX_PACKET_SIZE);
        if (dataLen == 0) {
            printf("Failed to receive data: %s", SDLNet_GetError());
            return;
        }
        log_debug("Received %u bytes from remote", dataLen);
        deserializeBoard(board, data);
    }
}

void sendBoardPacket(NetKit *kit, GameBoard *board) {
    uint8_t *data;
    uint32_t len = serializeBoard(board, &data);

    uint32_t res = SDLNet_TCP_Send(kit->socket, data, len);
    if (res < len) {
        log_error("Failed to send data: %s", SDLNet_GetError());
    }
    safefree(&data);
}

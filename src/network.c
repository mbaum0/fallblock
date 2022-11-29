#include "network.h"

void getIPAsString(uint32_t ipaddr, char *dst);

uint32_t serializeSyncPacket(SyncPacket *syncPacket, uint8_t **dst) {
    uint32_t dataLen = sizeof(SyncPacket);
    uint8_t *data = calloc(1, dataLen);
    *dst = data;

    uint32_t magic = htonl(syncPacket->magic);
    uint32_t stage = htonl(syncPacket->stage);
    uint32_t gameNum = htonl(syncPacket->gameNum);
    memcpy(data, &magic, sizeof(uint32_t));
    data += sizeof(uint32_t);
    memcpy(data, &stage, sizeof(uint32_t));
    data += sizeof(uint32_t);
    memcpy(data, &gameNum, sizeof(uint32_t));
    return dataLen;
}

void deserializeSyncPacket(uint8_t *data, SyncPacket *syncPacket) {
    uint32_t* magic = (uint32_t*)data;
    data += sizeof(uint32_t);
    uint32_t* stage = (uint32_t*)data;
    data += sizeof(uint32_t);
    uint32_t* gameNum = (uint32_t*)data;
    syncPacket->magic = ntohl(*magic);
    syncPacket->stage = ntohl(*stage);
    syncPacket->gameNum = ntohl(*gameNum);
}

uint32_t serializeWireTile(WireTile *wireTile, uint8_t **dst) {
    uint32_t dataLen = sizeof(WireTile);
    uint8_t *data = calloc(1, dataLen);
    *dst = data;
    memcpy(data, &wireTile, dataLen);
    return dataLen;
}

void deserializeWireTile(uint8_t *data, WireTile *wireTile) {
    wireTile->x = (uint8_t)*data;
    data += sizeof(uint8_t);
    wireTile->y = (uint8_t)*data;
    data += sizeof(uint8_t);
    wireTile->color = (uint8_t)*data;
}

uint32_t serializeWireBoard(WireBoard *wireBoard, uint8_t **dst) {
    uint32_t dataLen =
        (sizeof(uint32_t) * 3) + (sizeof(WireTile) * wireBoard->numTiles);
    uint8_t *data = calloc(1, dataLen);
    *dst = data;
    uint32_t level = htonl(wireBoard->level);
    uint32_t score = htonl(wireBoard->score);
    uint32_t numTiles = htonl(wireBoard->numTiles);
    memcpy(data, &level, sizeof(level));
    data += sizeof(level);
    memcpy(data, &score, sizeof(score));
    data += sizeof(score);
    memcpy(data, &numTiles, sizeof(numTiles));
    data += sizeof(numTiles);

    for (uint32_t i = 0; i < wireBoard->numTiles; i++) {
        WireTile t = wireBoard->tiles[i];
        uint32_t len = serializeWireTile(&t, &data);
        data += len;
    }
    return dataLen;
}

void deserializeWireBoard(uint8_t *data, WireBoard *wireBoard) {
    uint32_t* level = (uint32_t*)data;
    data += sizeof(uint32_t);
    uint32_t* score = (uint32_t*)data;
    data += sizeof(uint32_t);
    uint32_t* numTiles = (uint32_t*)data;
    data += sizeof(uint32_t);
    wireBoard->level = ntohl(*level);
    wireBoard->score = ntohl(*score);
    wireBoard->numTiles = ntohl(*numTiles);
    for (uint32_t i = 0; i < wireBoard->numTiles; i++) {
        WireTile t;
        deserializeWireTile(data, &t);
        wireBoard->tiles[i] = t;
        data += sizeof(WireTile);
    }
}

void initWireBoardFromGameBoard(WireBoard *wb, GameBoard *board) {
    wb->level = board->level;
    wb->score = board->score;
    wb->numTiles = 0;
    for (uint32_t x = 0; x < GAME_WIDTH; x++) {
        for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
            Tile *tile = board->playField[x][y];
            if (tile != NULL) {
                wb->tiles[wb->numTiles] =
                    (WireTile){tile->x, tile->y, tile->color};
                wb->numTiles++;
            }
        }
    }

    if (board->activePiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = board->activePiece->tiles[i];
            wb->tiles[wb->numTiles] = (WireTile){tile->x, tile->y, tile->color};
            wb->numTiles++;
        }
    }
}

void updateGameBoardFromWireBoard(GameBoard *target, WireBoard *src) {
    target->level = src->level;
    target->score = src->score;

    for (uint32_t i = 0; i < src->numTiles; i++) {
        WireTile t = src->tiles[i];
        target->playField[t.x][t.y]->color = t.color;
        target->playField[t.x][t.y]->x = t.x;
        target->playField[t.x][t.y]->y = t.y;
    }
}

NetKit *initNetworking(uint16_t localPort, uint16_t remotePort) {
    NetKit *kit = calloc(1, sizeof(NetKit));
    if (SDLNet_Init() < 0) {
        log_error("Couldn't initialize SDL: %s", SDLNet_GetError());
        return NULL;
    }

    kit->localPort = localPort;
    kit->remotePort = remotePort;

    kit->sock = SDLNet_UDP_Open(localPort);
    if (kit->sock == NULL) {
        log_error("Couldn't open listening socket: %s", SDLNet_GetError());
        return NULL;
    } else {
        log_info("Listening on 0.0.0.0:%u", localPort);
    }

    kit->socketset = SDLNet_AllocSocketSet(2);
    if (kit->socketset == NULL) {
        log_error("Couldn't create socket set: %s", SDLNet_GetError());
        return NULL;
    }

    kit->recvPacket = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (kit->recvPacket == NULL) {
        log_error("Couldn't allocate packet: %s", SDLNet_GetError());
        return NULL;
    }

    kit->connected = false;

    SDLNet_UDP_AddSocket(kit->socketset, kit->sock);

    // clear out recv buffer
    while (SDLNet_UDP_Recv(kit->sock, kit->recvPacket) > 0);
    return kit;
}

bool checkForSyncPacket(NetKit *kit, SyncPacket *pkt, uint32_t stage) {
    SDLNet_CheckSockets(kit->socketset, 0);
    if (SDLNet_SocketReady(kit->sock) == 0) {
        // didn't get any packets
        return false;
    }
    if (SDLNet_UDP_Recv(kit->sock, kit->recvPacket) == 0) {
        // got 0 bytes
        return false;
    }
    SyncPacket p;
    deserializeSyncPacket(kit->recvPacket->data, &p);
    if (p.magic != 0xDECAFBAD || p.stage != stage) {
        // packet was missing the magic bytes
        return false;
    }
    *pkt = p;
    // pkt->address = kit->recvPacket->address;
    return true;
}

void sendSyncPacket(NetKit *kit, uint32_t gameNum, uint32_t stage){
    SyncPacket pkt = {0xDECAFBAD, stage, gameNum};
    uint8_t* data;
    uint32_t dataLen = serializeSyncPacket(&pkt, &data);
    UDPpacket sendPkt;
    sendPkt.data = data;
    sendPkt.len = dataLen;

    if (stage == 0){
        SDLNet_ResolveHost(&kit->remoteIP, "localhost", kit->remotePort);
        sendPkt.address = kit->remoteIP;
    }

    SDLNet_UDP_Send(kit->sock, -1, &sendPkt);
}

void listenForConnections(NetKit *kit) {
    SyncPacket pkt;
    uint32_t gameID = 0;
    while (!kit->connected) {
        // try to find a peer
        log_debug("Listening for peer...");
        if (checkForSyncPacket(kit, &pkt, 0)) {
            // we got a sync packet
            log_debug("Peer found! Game # %u", pkt.gameNum);
            sendSyncPacket(kit, pkt.gameNum, 1);

            // wait for peer to confirm
            log_debug("Waiting for peer to confim..");
            for (uint32_t i = 0; i < 1; i++) {
                log_debug("Waiting... %u/10", i);
                if (checkForSyncPacket(kit, &pkt, 2)) {
                    // peer confirmed
                    log_debug("Peer confirmed. Ready to start");
                    kit->connected = true;
                    return;
                }
                SDL_Delay(1000);
            }
            if (!kit->connected) {
                log_debug("Peer didn't confirm.");
            }
        } else {
            // try to initiate connection
            gameID = rand() % 500;
            sendSyncPacket(kit, gameID, 0);

            // wait for confirmation
            for (uint32_t i = 0; i < 1; i++) {
                if (checkForSyncPacket(kit, &pkt, 1)) {
                    log_debug("Got response for peer. Sending confirmation. "
                              "Ready to start");
                    sendSyncPacket(kit, pkt.gameNum, 2);

                    return;
                }
                SDL_Delay(1000);
            }
        }
        SDL_Delay(1000);
    }
}

void getIPAsString(uint32_t ipaddr, char *dst) {
    sprintf(dst, "%u.%u.%u.%u", (ipaddr >> 24), (ipaddr >> 16) & 0xff,
            (ipaddr >> 8) & 0xff, ipaddr & 0xff);
}

void destroyNetworking(NetKit *kit) {
    SDLNet_UDP_Close(kit->sock);
    SDLNet_FreeSocketSet(kit->socketset);
    SDLNet_Quit();
    safefree(&kit);
}

void checkForBoardPacket(NetKit *kit, GameBoard *board) {
    SDLNet_CheckSockets(kit->socketset, 0);
    if (SDLNet_SocketReady(kit->sock) == 0) {
        return;
    }
    if (SDLNet_UDP_Recv(kit->sock, kit->recvPacket) == 0) {
        return;
    }
    WireBoard wb;
    deserializeWireBoard(kit->recvPacket->data, &wb);
    updateGameBoardFromWireBoard(board, &wb);
}

void sendBoardPacket(NetKit *kit, GameBoard *board) {
    uint8_t *data;
    WireBoard wb;
    initWireBoardFromGameBoard(&wb, board);
    uint32_t len = serializeWireBoard(&wb, &data);
    UDPpacket sendPkt;
    sendPkt.address = kit->remoteIP;
    sendPkt.data = data;
    sendPkt.len = len;
    SDLNet_UDP_Send(kit->sock, -1, &sendPkt);
}

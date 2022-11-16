#pragma once

typedef struct GameRunner GameRunner;

#include "board.h"
#include "display.h"
#include "input.h"
#include "media.h"
#include "network.h"
#include "util.h"
#include <stdio.h>

typedef enum { PLAYER_ONE, PLAYER_TWO } PlayerNum;

#define PLAYER_RIGHT(x) x == PLAYER_ONE ? SDL_SCANCODE_RIGHT : SDL_SCANCODE_D
#define PLAYER_LEFT(x) x == PLAYER_ONE ? SDL_SCANCODE_LEFT : SDL_SCANCODE_A
#define PLAYER_ROTATE_CC(x) x == PLAYER_ONE ? SDL_SCANCODE_Z : SDL_SCANCODE_E
#define PLAYER_ROTATE_C(x) x == PLAYER_ONE ? SDL_SCANCODE_X : SDL_SCANCODE_E
#define PLAYER_HARD_DROP(x)                                                    \
    x == PLAYER_ONE ? SDL_SCANCODE_SPACE : SDL_SCANCODE_Q

struct GameRunner {
    GameMode mode;
    GameBoard *board1;
    GameBoard *board2;
    Keyboard keyboard;
    GameMedia *media;
    NetKit *netkit;
};

GameRunner *createGame(GameMode gm);
void runGame(GameRunner *game);
void destroyGame(GameRunner *game);

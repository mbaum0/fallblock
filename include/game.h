#pragma once

typedef struct GameRunner GameRunner;

#include "board.h"
#include "display.h"
#include "input.h"
#include "media.h"
#include "util.h"
#include <stdio.h>
typedef enum { SINGLE_PLAYER, MULTI_PLAYER } GameMode;

struct GameRunner {
    GameMode mode;
    GameBoard *board1;
    GameBoard *board2;
    Keyboard keyboard;
    GameMedia *media;
};

GameRunner *createGame(GameMode gm);
void runGame(GameRunner *game);
void destroyGame(GameRunner *game);

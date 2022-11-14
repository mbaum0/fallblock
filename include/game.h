#pragma once

typedef struct Game Game;
typedef struct Stage Stage;

#include "defs.h"
#include "display.h"
#include "input.h"
#include "media.h"
#include "piece.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

struct Stage {
    Piece *currentPiece;
    PieceType nextPieceType;
    Tile *board[GAME_WIDTH][GAME_HEIGHT];
    uint32_t score;
    uint32_t level;
    float dropDelay;
    SDL_Texture *tileTexture;
    SDL_Texture *backdropTexture;
    TTF_Font *gameFont;
    SDL_Texture *menuTexture;
    SDL_Texture *menuBtnTexture;
};

struct Game {
    SDL_Renderer *renderer;
    SDL_Window *window;
    bool keyboard[MAX_KEYBOARD_KEYS];
    uint32_t ticks;
    bool menu;
    Stage *stage;
};

bool doLogic(Game *game);
void initGame(Game *game);
void destroyGame(Game *game);
void runGame(Game *game);

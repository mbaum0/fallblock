#pragma once

typedef struct Game Game;
typedef struct Stage Stage;

#include "defs.h"
#include "media.h"
#include "piece.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>

struct Stage {
    Piece *currentPiece;
    Tile *board[GAME_WIDTH][GAME_HEIGHT];
    uint32_t score;
    SDL_Texture *tileTexture;
    TTF_Font *gameFont;
};

struct Game {
    SDL_Renderer *renderer;
    SDL_Window *window;
    bool keyboard[MAX_KEYBOARD_KEYS];
    uint32_t ticks;
    Stage *stage;
};

void doLogic(Game *game);
void initGame(Game *game);
void destroyGame(Game *game);

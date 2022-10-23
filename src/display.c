#include "display.h"

void clearScene(Game *game);
void presentScene(Game *game);
void drawTiles(Game *game);
void blitTile(Game *game, Tile *tile);

void clearScene(Game *game) {
    SDL_RenderClear(game->renderer);
}

void presentScene(Game *game) {
    SDL_RenderPresent(game->renderer);
}

void blitTile(Game *game, Tile *tile) {
    uint32_t x = GB_TO_PX(tile->x);
    uint32_t y = GB_TO_PX(tile->y);
    SDL_Rect src;
    src.x = tile->color * BLOCK_SIZE;
    src.y = 0;
    src.w = src.h = BLOCK_SIZE;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = src.w;
    dst.h = src.h;
    SDL_RenderCopy(game->renderer, game->stage->tileTexture, &src, &dst);
}

void drawTiles(Game *game) {
    Tile *t;
    for (uint32_t i = 0; i < GAME_WIDTH; i++) {
        for (uint32_t j = 0; j < GAME_HEIGHT; j++) {
            if ((t = game->stage->board[i][j])) {
                blitTile(game, t);
            }
        }
    }

    if (game->stage->currentPiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *t = game->stage->currentPiece->tiles[i];
            blitTile(game, t);
        }
    }
}

void updateDisplay(Game *game) {
    clearScene(game);
    drawTiles(game);
    presentScene(game);
}

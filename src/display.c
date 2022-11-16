#include "display.h"

#define GB_X_TO_PX(x) (x * TILE_SIZE) + TILE_SIZE * 7
#define GB_Y_TO_PX(y) (TILE_SIZE * GAME_HEIGHT) - (y * TILE_SIZE)

typedef enum { BOARD_1 = 0, BOARD_2 = WINDOW_WIDTH } BoardNum;

void clearScene(GameMedia *gameMedia);
void presentScene(GameMedia *gameMedia);
void drawBackground(GameMedia *gameMedia);
void drawPlayfield(GameMedia *gameMedia, BoardNum bn);
void drawTileOnBoard(GameMedia *gameMedia, Tile *tile, BoardNum bn);
void drawGhostTileOnBoard(GameMedia *gameMedia, Tile *tile, BoardNum bn);
void drawActivePiece(GameMedia *gameMedia, GameBoard *board, BoardNum bn);
void drawGhostPiece(GameMedia *gameMedia, GameBoard *board, BoardNum bn);
void drawText(GameMedia *gameMedia, char *str, uint32_t x, uint32_t y,
              TTF_Font *font, SDL_Color color);
void drawScore(GameMedia *gameMedia, GameBoard *board, BoardNum bn);
void drawLevel(GameMedia *gameMedia, GameBoard *board, BoardNum bn);
void drawNextPiece(GameMedia *gameMedia, GameBoard *board, BoardNum bn);

char scoreString[100];
char levelString[100];

void clearScene(GameMedia *gameMedia) {
    SDL_SetRenderDrawColor(gameMedia->renderer, 255, 0, 0, 255);
    SDL_RenderClear(gameMedia->renderer);
}

void presentScene(GameMedia *gameMedia) {
    SDL_RenderPresent(gameMedia->renderer);
}

void drawBackground(GameMedia *gameMedia) {
    SDL_Rect rect;

    rect.w = gameMedia->windowWidth;
    rect.h = gameMedia->windowHeight;
    rect.x = 0;
    rect.y = 0;
    SDL_SetRenderDrawColor(gameMedia->renderer, 201, 197, 183, 255);
    SDL_RenderFillRect(gameMedia->renderer, &rect);
}

void drawPlayfield(GameMedia *gameMedia, BoardNum bn) {
    SDL_Rect rect;
    rect.w = TILE_SIZE * GAME_WIDTH;
    rect.h = TILE_SIZE * GAME_HEIGHT;
    rect.x = TILE_SIZE * 7 + bn;
    rect.y = TILE_SIZE * 1;
    SDL_SetRenderDrawColor(gameMedia->renderer, 10, 79, 73, 255);
    SDL_RenderFillRect(gameMedia->renderer, &rect);
}

void drawTileOnBoard(GameMedia *gameMedia, Tile *tile, BoardNum bn) {
    if (tile->y >= GAME_HEIGHT) {
        // don't dry if tile isn't on the game board
        return;
    }

    int32_t x = GB_X_TO_PX(tile->x) + bn;
    int32_t y = GB_Y_TO_PX(tile->y);

    SDL_Rect src;
    src.x = tile->color * TILE_SIZE;
    src.y = 0;
    src.w = src.h = TILE_SIZE;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = src.w;
    dst.h = src.h;
    SDL_RenderCopy(gameMedia->renderer, gameMedia->textures.tiles, &src, &dst);
}

void drawGhostTileOnBoard(GameMedia *gameMedia, Tile *tile, BoardNum bn) {
    int32_t x = GB_X_TO_PX(tile->x) + bn;
    int32_t y = GB_Y_TO_PX(tile->y);

    SDL_Rect rect;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
    rect.x = x;
    rect.y = y;
    SDL_SetRenderDrawColor(gameMedia->renderer, 255, 255, 255, 200);
    SDL_RenderDrawRect(gameMedia->renderer, &rect);
}

void drawActivePiece(GameMedia *gameMedia, GameBoard *board, BoardNum bn) {
    if (board->activePiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = board->activePiece->tiles[i];
            drawTileOnBoard(gameMedia, tile, bn);
        }
    }
}

void drawGhostPiece(GameMedia *gameMedia, GameBoard *board, BoardNum bn) {
    if (board->ghostPiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = board->ghostPiece->tiles[i];
            drawGhostTileOnBoard(gameMedia, tile, bn);
        }
    }
}

void drawLockedTiles(GameMedia *gameMedia, GameBoard *board, BoardNum bn) {
    for (uint32_t x = 0; x < GAME_WIDTH; x++) {
        for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
            Tile *tile = board->playField[x][y];
            if (tile != NULL) {
                drawTileOnBoard(gameMedia, tile, bn);
            }
        }
    }
}

void drawText(GameMedia *gameMedia, char *str, uint32_t x, uint32_t y,
              TTF_Font *font, SDL_Color color) {
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, str, color);
    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(gameMedia->renderer, textSurface);
    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(gameMedia->renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
}

void drawScore(GameMedia *gameMedia, GameBoard *board, BoardNum bn) {
    snprintf(scoreString, 100, "Score: %u", board->score);
    drawText(gameMedia, scoreString, 32 + bn, 32, gameMedia->fonts.gameFont,
             gameMedia->colors.white);
}

void drawLevel(GameMedia *gameMedia, GameBoard *board, BoardNum bn) {
    snprintf(levelString, 100, "Level: %u", board->level);
    drawText(gameMedia, levelString, 32 + bn, 64, gameMedia->fonts.gameFont,
             gameMedia->colors.white);
}

void drawNextPiece(GameMedia *gameMedia, GameBoard *board, BoardNum bn) {
    Piece *nextPiece = createNewPiece(0, 0, board->nextPieceType);
    for (uint32_t i = 0; i < 4; i++) {
        Tile *tile = nextPiece->tiles[i];

        SDL_Rect src;
        src.x = tile->color * TILE_SIZE;
        src.y = 0;
        src.w = src.h = TILE_SIZE;

        SDL_Rect dst;
        dst.x = 64 + (tile->x * TILE_SIZE) + bn;
        dst.y = 128 + (tile->y * TILE_SIZE);
        dst.w = src.w;
        dst.h = src.h;
        SDL_RenderCopy(gameMedia->renderer, gameMedia->textures.tiles, &src,
                       &dst);
    }
    for (uint32_t i = 0; i < 4; i++) {
        safefree(&nextPiece->tiles[i]);
    }
    safefree(&nextPiece);
}

void updateDisplay(GameMedia *media, GameBoard *board1, GameBoard *board2) {
    clearScene(media);
    drawBackground(media);
    drawPlayfield(media, BOARD_1);
    drawScore(media, board1, BOARD_1);
    drawLevel(media, board1, BOARD_1);
    drawActivePiece(media, board1, BOARD_1);
    drawGhostPiece(media, board1, BOARD_1);
    drawLockedTiles(media, board1, BOARD_1);
    drawNextPiece(media, board1, BOARD_1);
    if (board2 != NULL) {
        drawPlayfield(media, BOARD_2);
        drawScore(media, board2, BOARD_2);
        drawLevel(media, board2, BOARD_2);
        drawActivePiece(media, board2, BOARD_2);
        drawGhostPiece(media, board2, BOARD_2);
        drawLockedTiles(media, board2, BOARD_2);
        drawNextPiece(media, board2, BOARD_2);
    }
    presentScene(media);
}

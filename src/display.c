#include "display.h"

#define GB_X_TO_PX(x) (x * TILE_SIZE) + TILE_SIZE * 7
#define GB_Y_TO_PX(y) (TILE_SIZE * GAME_HEIGHT) - (y * TILE_SIZE)

void clearScene(GameMedia *gameMedia);
void presentScene(GameMedia *gameMedia);
void drawBackground(GameMedia *gameMedia);
void drawTileOnBoard(GameMedia *gameMedia, Tile *tile);
void drawGhostTileOnBoard(GameMedia *gameMedia, Tile *tile);
void drawActivePiece(GameMedia *gameMedia, GameBoard *gameBoard);
void drawGhostPiece(GameMedia *gameMedia, GameBoard *gameBoard);
void drawText(GameMedia *gameMedia, char *str, uint32_t x, uint32_t y,
              TTF_Font *font, SDL_Color color);
void drawScore(GameMedia *gameMedia, GameBoard *gameBoard);
void drawLevel(GameMedia *gameMedia, GameBoard *gameBoard);
void drawNextPiece(GameMedia *gameMedia, GameBoard *gameBoard);

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

    rect.w = WINDOW_WIDTH;
    rect.h = WINDOW_HEIGHT;
    rect.x = 0;
    rect.y = 0;
    SDL_SetRenderDrawColor(gameMedia->renderer, 201, 197, 183, 255);
    SDL_RenderFillRect(gameMedia->renderer, &rect);

    rect.w = TILE_SIZE * GAME_WIDTH;
    rect.h = TILE_SIZE * GAME_HEIGHT;
    rect.x = TILE_SIZE * 7;
    rect.y = TILE_SIZE * 1;
    SDL_SetRenderDrawColor(gameMedia->renderer, 10, 79, 73, 255);
    SDL_RenderFillRect(gameMedia->renderer, &rect);
}

void drawTileOnBoard(GameMedia *gameMedia, Tile *tile) {
    if (tile->y >= GAME_HEIGHT) {
        // don't dry if tile isn't on the game board
        return;
    }

    int32_t x = GB_X_TO_PX(tile->x);
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

void drawGhostTileOnBoard(GameMedia *gameMedia, Tile *tile) {
    int32_t x = GB_X_TO_PX(tile->x);
    int32_t y = GB_Y_TO_PX(tile->y);

    SDL_Rect rect;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
    rect.x = x;
    rect.y = y;
    SDL_SetRenderDrawColor(gameMedia->renderer, 255, 255, 255, 200);
    SDL_RenderDrawRect(gameMedia->renderer, &rect);
}

void drawActivePiece(GameMedia *gameMedia, GameBoard *gameBoard) {
    if (gameBoard->activePiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = gameBoard->activePiece->tiles[i];
            drawTileOnBoard(gameMedia, tile);
        }
    }
}

void drawGhostPiece(GameMedia *gameMedia, GameBoard *gameBoard) {
    if (gameBoard->ghostPiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = gameBoard->ghostPiece->tiles[i];
            drawGhostTileOnBoard(gameMedia, tile);
        }
    }
}

void drawLockedTiles(GameMedia *gameMedia, GameBoard *gameBoard) {
    for (uint32_t x = 0; x < GAME_WIDTH; x++) {
        for (uint32_t y = 0; y < GAME_HEIGHT; y++) {
            Tile *tile = gameBoard->playField[x][y];
            if (tile != NULL) {
                drawTileOnBoard(gameMedia, tile);
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

void drawScore(GameMedia *gameMedia, GameBoard *gameBoard) {
    snprintf(scoreString, 100, "Score: %u", gameBoard->score);
    drawText(gameMedia, scoreString, 32, 32, gameMedia->fonts.gameFont,
             gameMedia->colors.white);
}

void drawLevel(GameMedia *gameMedia, GameBoard *gameBoard) {
    snprintf(levelString, 100, "Level: %u", gameBoard->level);
    drawText(gameMedia, levelString, 32, 64, gameMedia->fonts.gameFont,
             gameMedia->colors.white);
}

void drawNextPiece(GameMedia *gameMedia, GameBoard *gameBoard) {
    Piece *nextPiece = createNewPiece(0, 0, gameBoard->nextPieceType);
    for (uint32_t i = 0; i < 4; i++) {
        Tile *tile = nextPiece->tiles[i];

        SDL_Rect src;
        src.x = tile->color * TILE_SIZE;
        src.y = 0;
        src.w = src.h = TILE_SIZE;

        SDL_Rect dst;
        dst.x = 64 + (tile->x * TILE_SIZE);
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

void updateDisplay(GameMedia *media, GameBoard *board) {
    clearScene(media);
    drawBackground(media);
    drawScore(media, board);
    drawLevel(media, board);
    drawActivePiece(media, board);
    drawGhostPiece(media, board);
    drawLockedTiles(media, board);
    drawNextPiece(media, board);
    presentScene(media);
}

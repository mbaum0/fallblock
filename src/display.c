#include "display.h"

#define GB_X_TO_PX(x) (x * TILE_SIZE) + TILE_SIZE * 7
#define GB_Y_TO_PX(y) (TILE_SIZE * GAME_HEIGHT) - (y* TILE_SIZE)

void clearScene(GameMedia *gameMedia);
void presentScene(GameMedia *gameMedia);
void drawBackground(GameMedia *gameMedia);
void drawTile(Tile *tile, GameMedia *gameMedia);
void drawLockedTiles(GameBoard *gameBoard, GameMedia *gameMedia);
void drawActivePiece(GameBoard *gameBoard, GameMedia *gameMedia);
void drawText(GameMedia *gameMedia, char *str, uint32_t x, uint32_t y,
              TTF_Font *font, SDL_Color color);
void drawScore(GameBoard *gameBoard, GameMedia *gameMedia);
void drawLevel(GameBoard *gameBoard, GameMedia *gameMedia);

char scoreString[20];
char levelString[20];

void clearScene(GameMedia *gameMedia) {
    SDL_RenderClear(gameMedia->renderer);
}

void presentScene(GameMedia *gameMedia) {
    SDL_RenderPresent(gameMedia->renderer);
}

void drawBackground(GameMedia *gameMedia) {
    SDL_Rect rect;

    rect.w = TILE_SIZE * GAME_WIDTH;
    rect.h = TILE_SIZE * GAME_HEIGHT;
    rect.x = TILE_SIZE * 7;
    rect.y = TILE_SIZE * 1;
    SDL_SetRenderDrawColor(gameMedia->renderer, 10, 79, 73, 255);
    SDL_RenderDrawRect(gameMedia->renderer, &rect);

    rect.w = WINDOW_WIDTH;
    rect.h = WINDOW_HEIGHT;
    rect.x = 0;
    rect.y = 0;
    SDL_SetRenderDrawColor(gameMedia->renderer, 201, 197, 183, 255);
    SDL_RenderDrawRect(gameMedia->renderer, &rect);
}

void drawTile(Tile *tile, GameMedia *gameMedia) {
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

void drawActivePiece(GameBoard *gameBoard, GameMedia *gameMedia) {
    if (gameBoard->activePiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *tile = gameBoard->activePiece->tiles[i];
            drawTile(tile, gameMedia);
        }
    }
}

void drawLockedTiles(GameBoard *gameBoard, GameMedia *gameMedia){
    for (uint32_t x = 0; x < GAME_WIDTH; x++){
        for (uint32_t y = 0; y < GAME_HEIGHT; y++){
            Tile *tile = gameBoard->playField[x][y];
            if (tile != NULL){
                drawTile(tile, gameMedia);
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

void drawScore(GameBoard *gameBoard, GameMedia *gameMedia) {
    snprintf(scoreString, 20, "Score: %d", gameBoard->score);
    drawText(gameMedia, scoreString, 32, 32, gameMedia->fonts.gameFont,
             gameMedia->colors.white);
}

void drawLevel(GameBoard *gameBoard, GameMedia *gameMedia) {
    snprintf(levelString, 20, "Level: %d", gameBoard->level);
    drawText(gameMedia, levelString, 32, 64, gameMedia->fonts.gameFont,
             gameMedia->colors.white);
}

void updateDisplay(GameMedia *gameMedia, GameBoard *gameBoard) {
    (void)gameBoard;
    clearScene(gameMedia);
    drawBackground(gameMedia);
    drawScore(gameBoard, gameMedia);
    drawLevel(gameBoard, gameMedia);
    drawActivePiece(gameBoard, gameMedia);
    drawLockedTiles(gameBoard, gameMedia);
    presentScene(gameMedia);
}

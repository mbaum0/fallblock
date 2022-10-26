#include "display.h"

void clearScene(GameMedia *gameMedia);
void presentScene(GameMedia *gameMedia);
void drawBackground(GameMedia *gameMedia);
void drawTile(Tile *tile, GameMedia *gameMedia);
void drawLockedTiles(GameBoard *gameBoard, GameMedia *gameMedia);
void drawActivePiece(GameBoard *gameBoard, GameMedia *gameMedia);
void drawScore(GameBoard *gameBoard, GameMedia *gameMedia);
void drawLevel(GameBoard *gameBoard, GameMedia *gameMedia);

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

void updateDisplay(GameMedia *gameMedia, GameBoard *gameBoard) {
    (void)gameBoard;
    clearScene(gameMedia);
    drawBackground(gameMedia);
    presentScene(gameMedia);
}

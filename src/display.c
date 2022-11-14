#include "display.h"

void clearScene(Game *game);
void presentScene(Game *game);
void drawTiles(Game *game);
void blitTile(Game *game, Tile *tile, bool doOffset);
void drawBackdrop(Game *game);
void drawScore(Game *game);

static SDL_Color whiteColor = {255, 255, 255, 255};
char scoreString[20];
char levelString[20];

void clearScene(Game *game) {
    SDL_RenderClear(game->renderer);
}

void presentScene(Game *game) {
    SDL_RenderPresent(game->renderer);
}

void blitTile(Game *game, Tile *tile, bool doOffset) {
    uint32_t x = GB_TO_PX(tile->x);
    uint32_t y = GB_TO_PX(tile->y);
    if (doOffset) {
        x += GAMEBOARD_WIDTH_OFFSET;
        y += GAMEBOARD_HEIGHT_OFFSET;
    }
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
                blitTile(game, t, true);
            }
        }
    }

    if (game->stage->currentPiece != NULL) {
        for (uint32_t i = 0; i < 4; i++) {
            Tile *t = game->stage->currentPiece->tiles[i];
            blitTile(game, t, true);
            SDL_Rect rect;
            rect.h = BLOCK_SIZE;
            rect.w = BLOCK_SIZE;
            rect.x = (t->x * BLOCK_SIZE) + GAMEBOARD_WIDTH_OFFSET;
            rect.y = (t->y * BLOCK_SIZE) + GAMEBOARD_HEIGHT_OFFSET;
            SDL_SetRenderDrawColor(game->renderer, 245, 245, 66, 100);
            SDL_RenderDrawRect(game->renderer, &rect);
        }
    }
}

void drawBackdrop(Game *game) {
    SDL_Rect dst;
    dst.x = 0;
    dst.y = 0;
    SDL_QueryTexture(game->stage->backdropTexture, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(game->renderer, game->stage->backdropTexture, NULL, &dst);
}

void drawScore(Game *game) {
    snprintf(scoreString, 20, "Score: %d", game->stage->score);
    SDL_Surface *scoreSurface =
        TTF_RenderText_Blended(game->stage->gameFont, scoreString, whiteColor);
    SDL_Texture *scoreTexture =
        SDL_CreateTextureFromSurface(game->renderer, scoreSurface);
    SDL_Rect scoreRect;
    scoreRect.x = 45;
    scoreRect.y = 45;
    scoreRect.w = scoreSurface->w;
    scoreRect.h = scoreSurface->h;
    SDL_RenderCopy(game->renderer, scoreTexture, NULL, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
}

void drawLevel(Game *game) {
    snprintf(levelString, 20, "Level: %d", game->stage->level);
    SDL_Surface *levelSurface =
        TTF_RenderText_Blended(game->stage->gameFont, levelString, whiteColor);
    SDL_Texture *levelTexture =
        SDL_CreateTextureFromSurface(game->renderer, levelSurface);
    SDL_Rect levelRect;
    levelRect.x = 45;
    levelRect.y = 90;
    levelRect.w = levelSurface->w;
    levelRect.h = levelSurface->h;
    SDL_RenderCopy(game->renderer, levelTexture, NULL, &levelRect);

    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);
}

void drawNextPiece(Game *game) {
    if (game->stage->currentPiece != NULL) {
        Piece *piece = createNewPiece(2, 5, game->stage->nextPieceType);
        for (uint32_t i = 0; i < 4; i++) {
            blitTile(game, piece->tiles[i], false);
        }
        destroyPiece(piece);
    }
}

void drawMenu(Game *game) {
    SDL_Rect dst;
    dst.x = 0;
    dst.y = 0;
    SDL_QueryTexture(game->stage->menuTexture, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(game->renderer, game->stage->menuTexture, NULL, &dst);
}

void updateDisplay(Game *game) {
    clearScene(game);
    if (game->menu) {
        drawMenu(game);
    } else {
        drawBackdrop(game);
        drawScore(game);
        drawLevel(game);
        drawTiles(game);
        drawNextPiece(game);
    }

    presentScene(game);
}

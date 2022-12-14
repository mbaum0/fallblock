#pragma once
#include "log.h"
#include "util.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#define TILE_SIZE 32
#define WINDOW_WIDTH 576
#define WINDOW_HEIGHT 704

typedef struct TexturePack TexturePack;
typedef struct FontPack FontPack;
typedef struct ColorPack ColorPack;
// typedef struct SoundPack SoundPack;
typedef struct GameMedia GameMedia;

struct TexturePack {
    SDL_Texture *tiles;
};

struct FontPack {
    TTF_Font *gameFont;
};

struct ColorPack {
    SDL_Color white;
};

// struct SoundPack {
// };

struct GameMedia {
    SDL_Renderer *renderer;
    SDL_Window *window;
    TexturePack textures;
    FontPack fonts;
    ColorPack colors;
    uint32_t windowWidth;
    uint32_t windowHeight;
};

/**
 * @brief Creates and initializes a GameMedia instance. Including the: renderer,
 * window, fonts and textures
 *
 * @param mode Single or Multiplayer mode
 *
 * @return GameMedia* new GameMedia instance
 */
GameMedia *createGameMedia(GameMode mode);

/**
 * @brief Cleans up all initialized game media
 *
 * @param gameMedia GameMedia instance to destroy
 */
void destroyGameMedia(GameMedia **gameMedia);

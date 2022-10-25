#pragma once
#include "logging.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define TILE_SIZE 32
#define WINDOW_WIDTH 576
#define WINDOW_HEIGHT 704

typedef struct TexturePack TexturePack;
typedef struct FontPack FontPack;
// typedef struct SoundPack SoundPack;
typedef struct GameMedia GameMedia;

struct TexturePack {
    SDL_Texture *tiles;
};

struct FontPack {
    TTF_Font *gameFont;
};

// struct SoundPack {
// };

struct GameMedia {
    SDL_Renderer *renderer;
    SDL_Window *window;
    TexturePack *textures;
    FontPack *fonts;
};

/**
 * @brief Initializes the renderer, window, fonts and textures
 *
 * @param gameMedia
 * @return true if media is successfully initialized
 * @return false if media failed to initialize
 */
bool initMedia(GameMedia *gameMedia);

/**
 * @brief Cleans up all initialized game media
 *
 * @param gameMedia GameMedia instance to destroy
 */
void destroyMedia(GameMedia *gameMedia);

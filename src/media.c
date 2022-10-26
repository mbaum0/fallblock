#include "media.h"

bool initSDL(GameMedia *gameMedia);
bool loadFonts(GameMedia *gameMedia);
void initColors(GameMedia *gameMedia);
bool loadTexture(SDL_Texture **dst, GameMedia *gameMedia, char *filename);
bool loadTextures(GameMedia *gameMedia);
void destroyTextures(GameMedia *gameMedia);
void destroyFonts(GameMedia *gameMedia);
void destroySDL(GameMedia *gameMedia);

bool initSDL(GameMedia *gameMedia) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        log_error("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    gameMedia->window = SDL_CreateWindow("FallBlock!", SDL_WINDOWPOS_UNDEFINED,
                                         SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                         WINDOW_HEIGHT, 0);

    if (!gameMedia->window) {
        log_error("Failed to open %d x %d window: %s", WINDOW_WIDTH,
                  WINDOW_HEIGHT, SDL_GetError());
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    gameMedia->renderer =
        SDL_CreateRenderer(gameMedia->window, -1, SDL_RENDERER_ACCELERATED);

    if (!gameMedia->renderer) {
        log_error("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        log_error("Couldn't initialize SDL_ttf: %s", TTF_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        log_error("Couldn't initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    return true;
}

bool loadFonts(GameMedia *gameMedia) {
    gameMedia->fonts.gameFont =
        TTF_OpenFont("assets/MouseMemoirs-Regular.ttf", 40);
    if (!gameMedia->fonts.gameFont) {
        log_error("Failed to open font: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

void initColors(GameMedia *gameMedia) {
    gameMedia->colors.white = (SDL_Color){255, 255, 255, 255};
}

bool loadTexture(SDL_Texture **dst, GameMedia *gameMedia, char *filename) {
    log_debug("Loading texture: %s", filename);
    SDL_Texture *texture;
    texture = IMG_LoadTexture(gameMedia->renderer, filename);

    *dst = texture;
    if (texture == NULL) {
        log_error("Failed to load texture: %s", IMG_GetError());
        return false;
    }
    return true;
}

bool loadTextures(GameMedia *gameMedia) {
    return (loadTexture(&(gameMedia->textures.tiles), gameMedia,
                        "assets/tiles.png"));
}

void destroyTextures(GameMedia *gameMedia) {
    SDL_DestroyTexture(gameMedia->textures.tiles);
    IMG_Quit();
}

void destroyFonts(GameMedia *gameMedia) {
    TTF_CloseFont(gameMedia->fonts.gameFont);
    TTF_Quit();
}

void destroySDL(GameMedia *gameMedia) {
    SDL_DestroyRenderer(gameMedia->renderer);
    SDL_DestroyWindow(gameMedia->window);
    SDL_Quit();
}

bool initMedia(GameMedia *gameMedia) {
    bool res =
        (initSDL(gameMedia) && loadTextures(gameMedia) && loadFonts(gameMedia));
    initColors(gameMedia);
    return res;
}

void destroyMedia(GameMedia *gameMedia) {
    destroyTextures(gameMedia);
    destroyFonts(gameMedia);
    destroySDL(gameMedia);
}

#include "media.h"

SDL_Texture *loadTexture(Game *game, char *filename);
bool initSDL(Game *game);
bool loadFonts(Game *game);
bool loadTextures(Game *game);
void destroyTextures(Game *game);
void destroyFonts(Game *game);
void destroySDL(Game *game);

bool initSDL(Game *game) {
    uint32_t rendererFlags = SDL_RENDERER_ACCELERATED;
    uint32_t windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    game->window = SDL_CreateWindow("FallBlock!", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                    SCREEN_HEIGHT, windowFlags);

    if (!game->window) {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH,
               SCREEN_HEIGHT, SDL_GetError());
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    game->renderer = SDL_CreateRenderer(game->window, -1, rendererFlags);

    if (!game->renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        printf("Couldn't initialize SDL_ttf: %s\n", TTF_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Couldn't initialize SDL_image: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

bool loadFonts(Game *game) {
    game->stage->gameFont = TTF_OpenFont("assets/MouseMemoirs-Regular.ttf", 40);
    if (!game->stage->gameFont) {
        printf("Failed to open font: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

SDL_Texture *loadTexture(Game *game, char *filename) {
    SDL_Texture *texture;
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                   "Loading %s", filename);
    texture = IMG_LoadTexture(game->renderer, filename);
    return texture;
}

bool loadTextures(Game *game) {
    game->stage->tileTexture = loadTexture(game, "assets/tiles.png");
    if (!game->stage->tileTexture) {
        printf("Failed to load texture: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

void destroyTextures(Game *game) {
    SDL_DestroyTexture(game->stage->tileTexture);
    IMG_Quit();
}

void destroyFonts(Game *game) {
    TTF_CloseFont(game->stage->gameFont);
    TTF_Quit();
}

void destroySDL(Game *game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}

bool initMedia(Game *game) {
    return (initSDL(game) && loadTextures(game) && loadFonts(game));
}

void destroyMedia(Game *game) {
    destroyTextures(game);
    destroyFonts(game);
    destroySDL(game);
}

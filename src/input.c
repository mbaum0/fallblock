#include "input.h"

static void doKeyUp(Game *game, SDL_KeyboardEvent *event) {
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
        game->keyboard[event->keysym.scancode] = 0;
    }
}

static void doKeyDown(Game *game, SDL_KeyboardEvent *event) {
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
        game->keyboard[event->keysym.scancode] = 1;
    }
}

bool processInput(Game *game) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return false;
            break;
        case SDL_KEYDOWN:
            doKeyDown(game, &event.key);
            break;
        case SDL_KEYUP:
            doKeyUp(game, &event.key);
            break;
        default:
            break;
        }
    }
    return true;
}

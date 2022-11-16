#include "input.h"

void doKeyUp(Keyboard *keyboard, SDL_KeyboardEvent *event) {
    SDL_Scancode keyCode = event->keysym.scancode;
    if (event->repeat == 0 && keyCode < MAX_KEYBOARD_KEYS) {
        keyboard->keys[keyCode] = (InputKey){keyCode, false, false, false};
    }
}

void doKeyDown(Keyboard *keyboard, SDL_KeyboardEvent *event) {
    SDL_Scancode keyCode = event->keysym.scancode;
    if (keyCode < MAX_KEYBOARD_KEYS) {
        keyboard->keys[keyCode] =
            (InputKey){keyCode, true, (event->repeat != 0), false};
    }
}

bool processInput(Keyboard *keyboard) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            log_debug("Received SDL_QUIT event");
            return true;
        case SDL_KEYDOWN:
            doKeyDown(keyboard, &event.key);
            break;
        case SDL_KEYUP:
            doKeyUp(keyboard, &event.key);
            break;
        default:
            break;
        }
    }
    return false;
}

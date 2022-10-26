#pragma once
#include "log.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_KEYBOARD_KEYS 350

typedef struct InputKey InputKey;
typedef struct Keyboard Keyboard;
struct InputKey {
    SDL_Scancode key;
    bool pressed;
    bool held;
};

struct Keyboard {
    InputKey keys[MAX_KEYBOARD_KEYS];
};

/**
 * @brief Updates a keyboard instance based on any SDL input events that have
 * occured
 *
 * @param keyboard Keyboard instance to update
 * @return true if the SDL_QUIT event was received
 * @return false if the SDL_QUIT event was NOT received
 */
bool processInput(Keyboard *keyboard);

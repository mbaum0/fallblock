#pragma once
#include "game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>

bool initMedia(Game *game);
void destroyMedia(Game *game);

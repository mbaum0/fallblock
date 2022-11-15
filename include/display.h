#pragma once
#include "board.h"
#include "media.h"
#include "util.h"

/**
 * @brief Update the graphical display with the current game status
 *
 * @param game The game instance
 */
void updateDisplay(GameMedia *media, GameBoard *board);

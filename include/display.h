#pragma once
#include "board.h"
#include "media.h"
#include "util.h"

/**
 * @brief Update the graphical display with the current game status
 *
 * @param media Game media instance containing textures, renderer, resources,
 * etc
 * @param board1 First players board. Must not be NULL
 * @param board2 Second players board. Can be NULL if single player mode
 */
void updateDisplay(GameMedia *media, GameBoard *board1, GameBoard *board2);

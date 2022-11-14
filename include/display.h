#pragma once
#include "game.h"
#include "media.h"

/**
 * @brief Update the graphical display with the current game status
 *
 * @param gameMedia GameMedia instance containing textures, fonts, etc.
 * @param gameBoard GameBoard instance containing the current state of the game
 */
void updateDisplay(GameMedia *gameMedia, GameBoard *gameBoard);

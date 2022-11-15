#include "board.h"
#include "display.h"
#include "log.h"
#include "media.h"
#include <stdint.h>
#include <unistd.h>
#include "input.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    // srand(time(0));
    // Game *theGame = createGame();
    // if (!initMedia(&theGame->media)) {
    //     log_error("Failed to initialize media\n");
    //     return 0;
    // }
    // Game *theGame2 = createGame();
    // while (!stepGame(theGame))
    //     ;
    // // runGame(theGame);
    // destroyGame(theGame);

    srand(time(0));
    GameMedia *media = createGameMedia();
    GameBoard *board = createGameBoard();
    Keyboard keyboard;

    while (!processInput(&keyboard)) {
        stepGameBoard(board);
        updateDisplay(media, board);
        SDL_Delay(100);
    }
}

#include "board.h"
#include "display.h"
#include "input.h"
#include "log.h"
#include "media.h"
#include <stdint.h>
#include <unistd.h>


bool shouldPerformKeyEvent(Keyboard* keyboard, SDL_Scancode sc){
    // return if key has been pressed, but not read yet
    return (keyboard->keys[sc].pressed && !keyboard->keys[sc].read);
}


void handleInput(GameBoard* board, Keyboard* keyboard){
    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_RIGHT)){
        processCommandEvent(board, IE_MOVE_RIGHT);
        keyboard->keys[SDL_SCANCODE_RIGHT].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_LEFT)){
        processCommandEvent(board, IE_MOVE_LEFT);
        keyboard->keys[SDL_SCANCODE_LEFT].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_Z)){
        processCommandEvent(board, IE_ROTATE_COUNTER_CLOCKWISE);
        keyboard->keys[SDL_SCANCODE_Z].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_X)){
        processCommandEvent(board, IE_ROTATE_CLOCKWISE);
        keyboard->keys[SDL_SCANCODE_X].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_SPACE)){
        processCommandEvent(board, IE_HARD_DROP);
        keyboard->keys[SDL_SCANCODE_SPACE].read = true;
    }
}

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
        handleInput(board, &keyboard);
        SDL_Delay(16);
    }
}

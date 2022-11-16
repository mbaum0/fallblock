#include "game.h"

bool shouldPerformKeyEvent(Keyboard *keyboard, SDL_Scancode sc) {
    // return if key has been pressed, but not read yet
    return (keyboard->keys[sc].pressed && !keyboard->keys[sc].read);
}

void handleInput(GameBoard *board, Keyboard *keyboard) {
    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_RIGHT)) {
        processCommandEvent(board, IE_MOVE_RIGHT);
        keyboard->keys[SDL_SCANCODE_RIGHT].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_LEFT)) {
        processCommandEvent(board, IE_MOVE_LEFT);
        keyboard->keys[SDL_SCANCODE_LEFT].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_Z)) {
        processCommandEvent(board, IE_ROTATE_COUNTER_CLOCKWISE);
        keyboard->keys[SDL_SCANCODE_Z].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_X)) {
        processCommandEvent(board, IE_ROTATE_CLOCKWISE);
        keyboard->keys[SDL_SCANCODE_X].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, SDL_SCANCODE_SPACE)) {
        processCommandEvent(board, IE_HARD_DROP);
        keyboard->keys[SDL_SCANCODE_SPACE].read = true;
    }
}

GameRunner *createGame(GameMode gm) {
    GameRunner *gr = calloc(1, sizeof(GameRunner));
    gr->mode = gm;
    gr->media = createGameMedia();

    gr->board1 = createGameBoard();

    if (gm == MULTI_PLAYER) {
        gr->board2 = createGameBoard();
    }

    return gr;
}

void destroyGame(GameRunner *game) {
    destroyGameBoard(game->board1);
    if (game->mode == MULTI_PLAYER) {
        destroyGameBoard(game->board2);
    }
    destroyGameMedia(&game->media);
    safefree(&game);
}

void runGame(GameRunner *game) {
    while (!processInput(&game->keyboard)) {
        stepGameBoard(game->board1);
        if (game->mode == MULTI_PLAYER) {
            stepGameBoard(game->board2);
        }

        handleInput(game->board1, &game->keyboard);
        if (game->mode == MULTI_PLAYER) {
            handleInput(game->board2, &game->keyboard);
        }
        updateDisplay(game->media, game->board1);
        SDL_Delay(16);
    }
}

#include "game.h"

bool shouldPerformKeyEvent(Keyboard *keyboard, SDL_Scancode sc) {
    // return if key has been pressed, but not read yet
    return (keyboard->keys[sc].pressed && !keyboard->keys[sc].read);
}

void handleInput(GameBoard *board, Keyboard *keyboard, PlayerNum player) {
    if (shouldPerformKeyEvent(keyboard, PLAYER_RIGHT(player))) {
        processCommandEvent(board, IE_MOVE_RIGHT);
        keyboard->keys[PLAYER_RIGHT(player)].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, PLAYER_LEFT(player))) {
        processCommandEvent(board, IE_MOVE_LEFT);
        keyboard->keys[PLAYER_LEFT(player)].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, PLAYER_ROTATE_CC(player))) {
        processCommandEvent(board, IE_ROTATE_COUNTER_CLOCKWISE);
        keyboard->keys[PLAYER_ROTATE_CC(player)].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, PLAYER_ROTATE_C(player))) {
        processCommandEvent(board, IE_ROTATE_CLOCKWISE);
        keyboard->keys[PLAYER_ROTATE_C(player)].read = true;
    }

    if (shouldPerformKeyEvent(keyboard, PLAYER_HARD_DROP(player))) {
        processCommandEvent(board, IE_HARD_DROP);
        keyboard->keys[PLAYER_HARD_DROP(player)].read = true;
    }
}

GameRunner *createGame(GameMode gm) {
    GameRunner *gr = calloc(1, sizeof(GameRunner));
    gr->mode = gm;
    gr->media = createGameMedia(gm);

    gr->board1 = createGameBoard();

    if (gm == MULTI_PLAYER) {
        gr->board2 = createGameBoard();
        gr->netkit = initNetworking();
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

        handleInput(game->board1, &game->keyboard, PLAYER_ONE);
        if (game->mode == MULTI_PLAYER) {
            handleInput(game->board2, &game->keyboard, PLAYER_TWO);
        }
        updateDisplay(game->media, game->board1, game->board2);
        // SDL_Delay(16);
        checkForPackets(game->netkit);
    }
}

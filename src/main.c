#include "display.h"
#include "game.h"
#include "input.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

Game game = {0};

int main(void) {
    srand(time(0));
    initGame(&game);
    printf("Starting game!\n");

    bool gameOver = false;
    while (processInput(&game)) {
        uint64_t start = SDL_GetPerformanceCounter();
        gameOver = doLogic(&game);
        updateDisplay(&game);
        uint64_t end = SDL_GetPerformanceCounter();
        float elapsedMS =
            (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        // Cap to 60FPS
        int32_t delay = floor(16.666f - elapsedMS);
        delay = (delay > 0) ? delay : 0;
        SDL_Delay(delay);

        if (gameOver) {
            printf("You lose! Game over!\n");
            break;
        }
    }

    destroyGame(&game);
    return 0;
}

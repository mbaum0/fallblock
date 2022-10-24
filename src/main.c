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
    while (processInput(&game)) {
        uint64_t start = SDL_GetPerformanceCounter();
        doLogic(&game);
        updateDisplay(&game);
        uint64_t end = SDL_GetPerformanceCounter();
        float elapsedMS =
            (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

        // Cap to 60FPS
        int32_t delay = floor(16.666f - elapsedMS);
        delay = (delay > 0) ? delay : 0;
        SDL_Delay(delay);
    }

    destroyGame(&game);
    return 0;
}

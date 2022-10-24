#include "game.h"
#include <math.h>
#include <stdlib.h>

Game game = {0};

int main(void) {
    srand(time(0));
    initGame(&game);
    printf("Starting game!\n");
    runGame(&game);
    destroyGame(&game);
    return 0;
}

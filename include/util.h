#pragma once

#define safefree(x)                                                            \
    if (*x != NULL) {                                                          \
        free(*x);                                                              \
        *x = NULL;                                                             \
    }

typedef enum { SINGLE_PLAYER, MULTI_PLAYER } GameMode;

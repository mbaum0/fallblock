#pragma once

#define safefree(x)                                                            \
    if (*x != NULL) {                                                          \
        free(*x);                                                              \
        *x = NULL;                                                             \
    }

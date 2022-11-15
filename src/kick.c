#include "kick.h"

void getKO_I_CW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy);
void getKO_I_CCW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy);
void getKO_JLSTZ_CW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy);
void getKO_JLSTZ_CCW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy);

void getKickOffsets(PieceType pt, PieceState ps, KickTest kt, bool clockwise,
                    int32_t *dx, int32_t *dy) {
    if (pt == Piece_I) {
        if (clockwise) {
            getKO_I_CW(kt, ps, dx, dy);
        } else {
            getKO_I_CCW(kt, ps, dx, dy);
        }
    } else if (pt == Piece_O) {
        *dx = *dy = 0;
    } else {
        if (clockwise) {
            getKO_JLSTZ_CW(kt, ps, dx, dy);
        } else {
            getKO_JLSTZ_CCW(kt, ps, dx, dy);
        }
    }
}

void getKO_I_CW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy) {
    switch (ps) {
    case PS_A:
        switch (kt) {
        case KT_A:
            *dx = -2;
            *dy = 0;
            break;
        case KT_B:
            *dx = 1;
            *dy = 0;
            break;
        case KT_C:
            *dx = -2;
            *dy = -1;
            break;
        case KT_D:
            *dx = 1;
            *dy = 2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_B:
        switch (kt) {
        case KT_A:
            *dx = -1;
            *dy = 0;
            break;
        case KT_B:
            *dx = 2;
            *dy = 0;
            break;
        case KT_C:
            *dx = -1;
            *dy = 2;
            break;
        case KT_D:
            *dx = 2;
            *dy = -1;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_C:
        switch (kt) {
        case KT_A:
            *dx = 2;
            *dy = 0;
            break;
        case KT_B:
            *dx = -1;
            *dy = 0;
            break;
        case KT_C:
            *dx = 2;
            *dy = 1;
            break;
        case KT_D:
            *dx = -1;
            *dy = -2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_D:
        switch (kt) {
        case KT_A:
            *dx = 1;
            *dy = 0;
            break;
        case KT_B:
            *dx = -2;
            *dy = 0;
            break;
        case KT_C:
            *dx = 1;
            *dy = -2;
            break;
        case KT_D:
            *dx = -2;
            *dy = 1;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    }
}

void getKO_I_CCW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy) {
    switch (ps) {
    case PS_A:
        switch (kt) {
        case KT_A:
            *dx = -1;
            *dy = 0;
            break;
        case KT_B:
            *dx = 2;
            *dy = 0;
            break;
        case KT_C:
            *dx = -1;
            *dy = 2;
            break;
        case KT_D:
            *dx = 2;
            *dy = -1;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_B:
        switch (kt) {
        case KT_A:
            *dx = -2;
            *dy = 0;
            break;
        case KT_B:
            *dx = 1;
            *dy = 0;
            break;
        case KT_C:
            *dx = -2;
            *dy = -1;
            break;
        case KT_D:
            *dx = 1;
            *dy = 2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_C:
        switch (kt) {
        case KT_A:
            *dx = 1;
            *dy = 0;
            break;
        case KT_B:
            *dx = -2;
            *dy = 0;
            break;
        case KT_C:
            *dx = 1;
            *dy = -2;
            break;
        case KT_D:
            *dx = -2;
            *dy = 1;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_D:
        switch (kt) {
        case KT_A:
            *dx = 2;
            *dy = 0;
            break;
        case KT_B:
            *dx = -1;
            *dy = 0;
            break;
        case KT_C:
            *dx = 2;
            *dy = 1;
            break;
        case KT_D:
            *dx = -1;
            *dy = -2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    }
}

void getKO_JLSTZ_CW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy) {
    switch (ps) {
    case PS_A:
        switch (kt) {
        case KT_A:
            *dx = -1;
            *dy = 0;
            break;
        case KT_B:
            *dx = -1;
            *dy = 1;
            break;
        case KT_C:
            *dx = 0;
            *dy = -2;
            break;
        case KT_D:
            *dx = -1;
            *dy = -2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_B:
        switch (kt) {
        case KT_A:
            *dx = 1;
            *dy = 0;
            break;
        case KT_B:
            *dx = 1;
            *dy = -1;
            break;
        case KT_C:
            *dx = 0;
            *dy = 2;
            break;
        case KT_D:
            *dx = 1;
            *dy = 2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_C:
        switch (kt) {
        case KT_A:
            *dx = 1;
            *dy = 0;
            break;
        case KT_B:
            *dx = 1;
            *dy = 1;
            break;
        case KT_C:
            *dx = 0;
            *dy = -2;
            break;
        case KT_D:
            *dx = 1;
            *dy = -2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_D:
        switch (kt) {
        case KT_A:
            *dx = -1;
            *dy = 0;
            break;
        case KT_B:
            *dx = -1;
            *dy = -1;
            break;
        case KT_C:
            *dx = 0;
            *dy = 2;
            break;
        case KT_D:
            *dx = -1;
            *dy = 2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    }
}

void getKO_JLSTZ_CCW(KickTest kt, PieceState ps, int32_t *dx, int32_t *dy) {
    switch (ps) {
    case PS_A:
        switch (kt) {
        case KT_A:
            *dx = 1;
            *dy = 0;
            break;
        case KT_B:
            *dx = 1;
            *dy = 1;
            break;
        case KT_C:
            *dx = 0;
            *dy = -2;
            break;
        case KT_D:
            *dx = 1;
            *dy = -2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_B:
        switch (kt) {
        case KT_A:
            *dx = -1;
            *dy = 0;
            break;
        case KT_B:
            *dx = -1;
            *dy = -1;
            break;
        case KT_C:
            *dx = 0;
            *dy = 2;
            break;
        case KT_D:
            *dx = -1;
            *dy = 2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_C:
        switch (kt) {
        case KT_A:
            *dx = -1;
            *dy = 0;
            break;
        case KT_B:
            *dx = -1;
            *dy = 1;
            break;
        case KT_C:
            *dx = 0;
            *dy = -2;
            break;
        case KT_D:
            *dx = -1;
            *dy = -2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    case PS_D:
        switch (kt) {
        case KT_A:
            *dx = 1;
            *dy = 0;
            break;
        case KT_B:
            *dx = 1;
            *dy = -1;
            break;
        case KT_C:
            *dx = 0;
            *dy = 2;
            break;
        case KT_D:
            *dx = 1;
            *dy = 2;
            break;
        default:
            *dx = *dy = 0;
        }
        break;
    }
}

//
// Created by user on 26/02/2026.
//

#include "Utils.h"

bool rectanglesOverlap(double ax, double ay, int aw, int ah, double bx, double by, int bw, int bh) {
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

int rpsResult(Mode left, Mode right) {
    if (left == right) {
        return 0;
    }
    if ((left == Rock && right == Scissors) ||
        (left == Paper && right == Rock) ||
        (left == Scissors && right == Paper)) {
        return 1;
    }
    return -1;
}

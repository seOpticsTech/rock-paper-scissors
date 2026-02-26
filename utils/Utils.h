//
// Created by user on 26/02/2026.
//

#ifndef GAME_UTILS_H
#define GAME_UTILS_H

enum Mode {
    Rock,
    Paper,
    Scissors
};

bool rectanglesOverlap(double ax, double ay, int aw, int ah, double bx, double by, int bw, int bh);
int rpsResult(Mode left, Mode right);

#endif //GAME_UTILS_H

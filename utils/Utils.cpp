//
// Created by user on 26/02/2026.
//

#include "Utils.h"

bool rectanglesOverlap(double ax, double ay, int aw, int ah, double bx, double by, int bw, int bh) {
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

int rpsResult(Player::Mode left, Player::Mode right) {
    if (left == right) {
        return 0;
    }
    if ((left == Player::Rock && right == Player::Scissors) ||
        (left == Player::Paper && right == Player::Rock) ||
        (left == Player::Scissors && right == Player::Paper)) {
        return 1;
    }
    return -1;
}

int rpsResult(Player::Mode left, MiniMe::Mode right) {
    if (left == Player::Rock && right == MiniMe::Rock) {
        return 0;
    }
    if (left == Player::Paper && right == MiniMe::Paper) {
        return 0;
    }
    if (left == Player::Scissors && right == MiniMe::Scissors) {
        return 0;
    }
    if ((left == Player::Rock && right == MiniMe::Scissors) ||
        (left == Player::Paper && right == MiniMe::Rock) ||
        (left == Player::Scissors && right == MiniMe::Paper)) {
        return 1;
    }
    return -1;
}

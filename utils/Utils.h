//
// Created by user on 26/02/2026.
//

#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "Actor/Actors/MiniMe/MiniMe.h"
#include "Actor/Actors/Player/Player.h"

bool rectanglesOverlap(double ax, double ay, int aw, int ah, double bx, double by, int bw, int bh);
int rpsResult(Player::Mode left, Player::Mode right);
int rpsResult(Player::Mode left, MiniMe::Mode right);

#endif //GAME_UTILS_H

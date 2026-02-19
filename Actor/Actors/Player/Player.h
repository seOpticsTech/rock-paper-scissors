//
// Created by user on 19/02/2026.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H
#include "Actor/Actor.h"
#include "State/State.h"
using namespace std;


class Player {
    public:
    explicit Player(Error& err);
    ~Player();

    Actor::eventAction genOnKeyDown(const Vector& v);

    Actor* actor;
};


#endif //GAME_PLAYER_H

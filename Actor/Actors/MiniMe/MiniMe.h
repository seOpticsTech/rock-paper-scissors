//
// Created by user on 24/02/2026.
//

#ifndef GAME_MINIME_H
#define GAME_MINIME_H
#include <SDL2/SDL_rect.h>
#include "Actor/Actor.h"
#include "Actor/Actors/Player/Player.h"
#include "Error.h"


class MiniMe {
    public:
    enum Mode {
        Rock,
        Paper,
        Scissors
    };
    MiniMe(const SDL_Rect& pos, Player::Mode playerMode, Error& err);
    ~MiniMe();

    Actor* actor;
    Mode mode;
};

#endif //GAME_MINIME_H

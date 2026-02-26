//
// Created by user on 24/02/2026.
//

#ifndef GAME_MINIME_H
#define GAME_MINIME_H
#include <SDL2/SDL_rect.h>
#include "Actor/Actor.h"
#include "utils/Utils.h"
#include "Error.h"


class MiniMe : public Actor {
    public:
    MiniMe(const SDL_Rect& pos, Mode playerMode, Error& err);
    ~MiniMe();

    Mode mode;
};

#endif //GAME_MINIME_H

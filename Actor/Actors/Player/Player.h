//
// Created by user on 19/02/2026.
//

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H
#include "Actor/Actor.h"
#include "State/State.h"
#include "Config/Config.h"
#include "utils/Utils.h"
using namespace std;


class Player : public Actor {
    public:
    Player(const string& name, const Vector& startPosition, Error& err);
    ~Player();

    Actor::eventAction genOnKeyDown(const Vector& v);

    ControlMode controlMode;
    SDL_JoystickID controllerId;
    Mode mode;
    Uint32 lastSpawnMs;
    Uint32 lastModeChangeMs;
    int hp;
};


#endif //GAME_PLAYER_H

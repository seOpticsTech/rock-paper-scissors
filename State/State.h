//
// Created by user on 14/02/2026.
//

#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "SDL_Environment/SDL_Environment.h"
#include <vector>

#include "Actor/Actor.h"


class State {
    public:
    State(const Config& config, Error& err);
    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&& other) noexcept;
    State& operator=(State&& other) noexcept;
    ~State();

    void startEventLoop();

    SDL_Environment* env;
    bool running;
    vector<Actor*> actors;
};


#endif //GAME_STATE_H

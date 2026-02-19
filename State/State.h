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
    // Ctors and Dtors
    State(const Config& config, Error& err);
    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&& other) noexcept;
    State& operator=(State&& other) noexcept;
    ~State();

    // Methods
    void startEventLoop();
    Texture* loadTexture(const string &name, const string &filePath, Error &err);
    Texture* loadTexture(const string &name, const string &filePath, const SDL_Rect& scope, Error &err);
    Actor* addActor(const string& name);

    // Vars
    map<string, Texture*> textures;
    SDL_Environment* env;
    bool running;
    map<string, Actor*> actors;
};


#endif //GAME_STATE_H

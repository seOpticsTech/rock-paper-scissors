//
// Created by user on 14/02/2026.
//

#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "SDL_Environment/SDL_Environment.h"
#include "Actor/Actors/View/View.h"

#include "Actor/Actor.h"

using namespace std;

class View;

class State {
    public:
    //Static
    static State* instance;
    static void make(const Config& config, Error& err);
    static void destroy();
    static State& get();

    // Ctors and Dtors
    State(const State&) = delete;
    State& operator=(const State&) = delete;
    ~State();

    // Methods
    void startEventLoop();
    Texture* loadTexture(const string &name, const string &filePath, Error &err);
    Texture* loadTexture(const string &name, const string &filePath, const SDL_Rect& scope, Error &err);
    Actor* addActor(const string& name, Error& err);

    void cleanupTextures();

    // Vars
    map<string, Texture*> textures;
    SDL_Environment* env;
    bool running;
    map<string, Actor*> actors;
    int textureCleanupEveryXFrames;
    View* view;

    protected:
    State(const Config& config, Error& err);
};


#endif //GAME_STATE_H

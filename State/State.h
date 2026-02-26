//
// Created by user on 14/02/2026.
//

#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "SDL_Environment/SDL_Environment.h"
#include "Actor/Actors/View/View.h"
#include "Animation/Animation.h"
#include <initializer_list>
#include <utility>
#include <vector>
#include <SDL2/SDL.h>
#include <unordered_set>

#include "Actor/Actor.h"

using namespace std;

class View;
class Player;
class MiniMe;

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
    void collisionHandler();
    void handleMiniMesCollisions(const vector<Player*>& players, const vector<MiniMe*>& minimes);
    void handlePlayersCollisions(const vector<Player*>& players);
    Animation loadAnimation(const string& name, std::initializer_list<string> paths, int msPerFrame, Error& err);
    Animation loadAnimation(const string& name, std::initializer_list<pair<string, const SDL_Rect*>> frames, int msPerFrame, Error& err);
    Animation loadAnimationNoDuplicate(const string& name, std::initializer_list<string> paths, int msPerFrame, Error& err);
    Animation loadAnimationNoDuplicate(const string& name, std::initializer_list<pair<string, const SDL_Rect*>> frames, int msPerFrame, Error& err);
    Actor* addActor(const string& name, Error& err);
    bool registerActor(const string& name, Actor* actor, Error& err);

    void removeActor(const string& name);
    void removeActor(Actor* actor);

    void cleanupAnimations();
    void applyDeferredActors();

    // Vars
    map<string, Animation> animations;
    SDL_Environment* env;
    bool running;
    map<string, Actor*> actors;
    int animationCleanupEveryXFrames;
    View* view;
    int fps;
    ControlMode controlMode;
    vector<SDL_GameController*> controllers;
    vector<SDL_JoystickID> controllerIds;

    function<int(SDL_Event *)> pollEvent;

    bool deferActorMutations;
    vector<pair<string, Actor*>> pendingAddActors;
    unordered_set<string> pendingRemoveNames;
    unordered_set<Actor*> pendingRemoveActors;

    protected:
    State(const Config& config, Error& err);
};


#endif //GAME_STATE_H

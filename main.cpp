#include <iostream>
#include <SDL2/SDL.h>
#include "Error.h"
#include "Actor/Actors/Player/Player.h"
#include "SDL_Environment/SDL_Environment.h"
#include "State/State.h"
#include "Vector/Vector.h"

using namespace std;

int customPollEvent(SDL_Event *event) {
    static bool eventSet = false;
    static Uint32 lastEventMs = 0;
    static bool leftNext = true;
    SDL_Event quitEvent;
    if (SDL_PollEvent(&quitEvent) && quitEvent.type == SDL_QUIT) {
        *event = quitEvent;
        return 1;
    }
    if (eventSet) {
        eventSet = false;
        return 0;
    }
    Uint32 nowMs = SDL_GetTicks();
    if (nowMs - lastEventMs >= 1000) {
        event->type = SDL_KEYDOWN;
        event->key.state = SDL_PRESSED;
        event->key.repeat = 0;
        event->key.keysym.sym = leftNext ? SDLK_LEFT : SDLK_RIGHT;
        event->key.keysym.scancode = SDL_GetScancodeFromKey(event->key.keysym.sym);
        event->key.keysym.mod = 0;
        leftNext = !leftNext;
        lastEventMs = nowMs;
        eventSet = true;
        return 1;
    }
    return 0;
}

int main()
{
    Error err;
    auto config = getDefaultConfig();
    config.controlMode = GAMEPAD;
    // config.pollEvent = customPollEvent;

    State::make(config, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
        return 1;
    }
    State& state = State::get();

    Player* p1 = new Player("player_1", Vector(480, 540), err);
    if (err.status == failure) {
        delete p1;
        cerr << "Failed to create player 1: " << err.message << endl;
        return 1;
    }

    Player* p2 = new Player("player_2", Vector(1440, 540), err);
    if (err.status == failure) {
        delete p2; 
        cerr << "Failed to create player 2: " << err.message << endl;
        return 1;
    }

    if (state.controlMode == GAMEPAD) {
        p1->controlMode = GAMEPAD;
        p2->controlMode = GAMEPAD;
        p1->controllerId = state.controllerIds[0];
        p2->controllerId = state.controllerIds[1];
    } else {
        p1->controlMode = KEYBOARD;
        p2->controlMode = GAMEPAD;
        p2->controllerId = state.controllerIds[0];
    }

    state.startEventLoop();
    State::destroy();
    return 0;
}

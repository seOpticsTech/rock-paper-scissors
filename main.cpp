#include <iostream>
#include <SDL2/SDL.h>
#include "Error.h"
#include "Actor/Actors/Player/Player.h"
#include "SDL_Environment/SDL_Environment.h"
#include "State/State.h"

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
    } else {
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

}

int main()
{
    Error err;
    auto config = getDefaultConfig();
    config.pollEvent = customPollEvent;

    State::make(config, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
        return 1;
    }
    State& state = State::get();

    Player p(err);
    if (err.status == failure) {
            cerr << "Failed to create actor: " << err.message << endl;
        return 1;
    }

    state.startEventLoop();
    State::destroy();
    return 0;
}

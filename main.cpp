#include <iostream>
#include "Error.h"
#include "Actor/Actors/Player/Player.h"
#include "SDL_Environment/SDL_Environment.h"
#include "State/State.h"

using namespace std;

int main()
{
    Error err;
    const auto config = getDefaultConfig();

    State state(config, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
        return 1;
    }

    Player(state, err);
    if (err.status == failure) {
            cerr << "Failed to create actor: " << err.message << endl;
        return 1;
    }

    state.startEventLoop();
    return 0;
}

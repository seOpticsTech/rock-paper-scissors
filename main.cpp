#include <iostream>
#include "Error.h"
#include "SDL_Environment/SDL_Environment.h"
#include "State/State.h"

using namespace std;

void setUpPlayer(State& state, Error& err) {
    auto player = state.addActor("player", err);
    if (err.status == failure) {
        return;
    }
    player->textures["main"] = state.loadTexture("player", "./assets/player.png", err);
    if (err.status == failure) {
        return;
    }
    player->currentTexture = "main";
    player->position = Vector(380, 280);
    player->acceleration = Vector(0, 0.1);
}

int main()
{
    Error err;
    const auto config = getDefaultConfig();

    State state(config, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
        return 1;
    }

    setUpPlayer(state, err);
    if (err.status == failure) {
            cerr << "Failed to create actor: " << err.message << endl;
        return 1;
    }

    state.startEventLoop();
    return 0;
}

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "Error.h"
#include "SDL_Environment/SDL_Environment.h"
#include <map>

using namespace std;

typedef Error (*eventHandler)(SDL_Environment&);

Error handleQuitEvent(SDL_Environment& env) {
    env.running = false;
    return Error::Success();
}

int main(int argc, char *argv[])
{
    Error err;
    const auto config = getDefaultConfig();
    auto env = SDL_Environment(config, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
        return 1;
    }

    const auto playerTexture = env.renderer->loadTexture("assets/player.png", err);
    if (err.status == failure) {
        cerr << "SDL_Environment load error: " << err.message << endl;
        return 1;
    }

    int texWidth = 0;
    int texHeight = 0;
    playerTexture.querySize(texWidth, texHeight, err);
    if (err.status == failure) {
        cerr << "Texture size error: " << err.message << endl;
        return 1;
    }

    SDL_Event event;

    map<Uint32, eventHandler> typeToHandler;
    typeToHandler[SDL_QUIT] = handleQuitEvent;

    while (env.running) {
        while (SDL_PollEvent(&event)) {
            eventHandler handle = typeToHandler[event.type];
            if (handle == nullptr) {
                continue;
            }
            err = handle(env);
            if (err.status == failure) {
                cerr << "SDL_Event type error: " << err.message << endl;
            }
        }

        env.renderer->setDrawColor(0, 0, 0, 255);
        env.renderer->clear();

        SDL_Rect dstRect;
        dstRect.w = texWidth;
        dstRect.h = texHeight;
        dstRect.x = (800 - texWidth) / 2;
        dstRect.y = (600 - texHeight) / 2;
        env.renderer->copy(playerTexture, &dstRect);

        env.renderer->present();
    }

    return 0;
}

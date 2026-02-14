#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "Error.h"
#include "SDL_Environment/SDL_Environment.h"

using namespace std;

int main(int argc, char *argv[])
{
    Error err;
    const auto env = SDL_Environment(800, 600, err);
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

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        env.renderer->setDrawColor(0, 0, 0, 255);
        env.renderer->clear();

        SDL_Rect dstRect;
        dstRect.w = texWidth;
        dstRect.h = texHeight;
        dstRect.x = (800 - texWidth) / 2;
        dstRect.y = (600 - texHeight) / 2;
        env.renderer->copy(playerTexture, nullptr, &dstRect);

        env.renderer->present();
    }

    return 0;
}

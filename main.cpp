#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "sdl_tools/Error.h"
#include "sdl_tools/SDL_Environment.h"

using namespace std;

int main(int argc, char *argv[])
{
    Error err;
    const auto env = SDL_Environment(800, 600, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
    }
    const auto renderer = env.renderer->renderer;

    SDL_Texture *playerTexture = IMG_LoadTexture(renderer, "assets/player.png");
    if (!playerTexture) {
        cerr << "IMG_LoadTexture error: " << IMG_GetError() << endl;
        return 1;
    }

    int texWidth = 0;
    int texHeight = 0;
    if (SDL_QueryTexture(playerTexture, nullptr, nullptr, &texWidth, &texHeight) != 0) {
        cerr << "SDL_QueryTexture error: " << SDL_GetError() << endl;
        SDL_DestroyTexture(playerTexture);
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect dstRect;
        dstRect.w = texWidth;
        dstRect.h = texHeight;
        dstRect.x = (800 - texWidth) / 2;
        dstRect.y = (600 - texHeight) / 2;
        SDL_RenderCopy(renderer, playerTexture, nullptr, &dstRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(playerTexture);
    return 0;
}

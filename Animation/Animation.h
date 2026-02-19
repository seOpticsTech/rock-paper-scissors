//
// Created by user on 19/02/2026.
//

#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H

#include "CyclicList/CyclicList.h"

class Texture;

class Animation {
public:
    CyclicList<Texture*>::cyclic_iterator iterator;
    int msPerFrame;

    Animation() : iterator(), msPerFrame(0) {}
    explicit Animation(CyclicList<Texture*>::cyclic_iterator it, int msPerFrame) : iterator(it), msPerFrame(msPerFrame) {}
};

#endif //GAME_ANIMATION_H

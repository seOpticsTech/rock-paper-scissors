//
// Created by user on 19/02/2026.
//

#ifndef GAME_VIEW_H
#define GAME_VIEW_H



#include "Actor/Actor.h"
using namespace std;


class View : public Actor {
public:
    explicit View(int width, int height);
    ~View() = default;

    void onRender();

    int width, height;
};



#endif //GAME_VIEW_H

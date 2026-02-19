//
// Created by user on 19/02/2026.
//

#ifndef GAME_VIEW_H
#define GAME_VIEW_H



#include "Actor/Actor.h"
#include "State/State.h"
using namespace std;


class View {
public:
    explicit View(int width, int height);
    ~View() = default;

    void onRender();

    Actor* actor;
    int width, height;
};



#endif //GAME_VIEW_H
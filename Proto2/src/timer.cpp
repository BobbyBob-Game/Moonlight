#include "timer.h"

Time::Time() {
    start = 0;
    pause = 0;
    isPause = false;
    isStart = false;
}

void Time::startTime() {
    isStart = true;
    isPause = false;
    start = SDL_GetTicks();
}

double Time::getFPS() {
    if(isPause){
        return pause;
    }
    else{
        SDL_GetTicks() - start;
    }
}
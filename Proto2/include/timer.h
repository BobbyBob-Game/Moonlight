#ifndef TIME_H
#define TIME_H
#include "defs.h"

class Time {
public:
    Time();
    void startTime();
    double getFPS();

private:
    long long start;
    long long pause;
    bool isPause;
    bool isStart;
};

#endif
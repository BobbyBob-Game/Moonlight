#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED
#include "object.h"
#include <iostream>
#include <vector>
using namespace std;

class Entity  : public Object {
    public: 
    Entity () {rev = false;}
    void setHealth(int h) {health = h;}
    void setmaxHealth(int h) {maxHealth = h;}
    int getHealth() const {return health;}
    int getmaxHealth() const {return maxHealth;}
    int createCycle(int r, int w, int h, int amount, int speed);
    void setCurrentAnimation(int c) {begin = 0; curAnim = c;}
    int getcurAnimation() const {return curAnim;}
    void updateAnimation(float deltaTime);
    void reverseNeed(bool r, int nA) {rev = r; nAb = 1; nA = newAnim;}
    void reverse(int nA) {nAb = 1 ;nA = newAnim;}

    private:
    int health, maxHealth;
    float xpos, ypos;
    float xspeed, yspeed;
    float groundSpeed;
    struct cycle {
        int row;
        int w;
        int h;
        int amount;
        int speed;
        int tick;
    };
    vector<cycle> animations;
    int curAnim;
    int begin;
    bool rev, nAb;
    int newAnim;
};



#endif
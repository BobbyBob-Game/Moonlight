#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED
#include "object.h"
#include "defs.h"
#include <iostream>
#include <vector>
using namespace std;

class Entity  : public Object {
    public: 
    float xpos, ypos;
    bool onGround = false;
    float velX = 0.0f;
    float velY = 0.0f;
    
    Entity () {}
    void setHealth(int h) {health = h;}
    void setmaxHealth(int h) {maxHealth = h;}
    int getHealth() const {return health;}
    int getmaxHealth() const {return maxHealth;}
    int createCycle(int r, int w, int h, int amount, int speed);
    void setCurrentAnimation(int c) {begin = 0; curAnim = c;}
    int getcurAnimation() const {return curAnim;}
    void updateAnimation(float deltaTime);
    bool isonGround() {return onGround;}
    void StartJump();
    void EndJump();
    void Update();

    private:
    int health, maxHealth;

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
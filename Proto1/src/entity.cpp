#include "entity.h"

int Entity::createCycle(int r, int w, int h, int amount, int speed){
    cycle temp;
    temp.row = r-1;
    temp.w = w;
    temp.h = h;
    temp.amount = amount;
    temp.speed = speed;
    temp.tick = 0;
    animations.push_back(temp);
    return animations.size() - 1; //return the index of the newly added cycle
}

void Entity::updateAnimation(float deltaTime){
    begin += deltaTime * 130;
    if (begin >= animations[curAnim].speed) { 
        begin = 0; 
        if(!rev) {
            animations[curAnim].tick++; 
        }
        if(rev){
            animations[curAnim].tick--;
        }
    }

    if (animations[curAnim].tick >= animations[curAnim].amount) {
        animations[curAnim].tick = 0;
    }

    if (animations[curAnim].tick <= 0){
        if(nAb){
            curAnim = newAnim;
            nAb = 0;
            rev = 0;
        }
        else{
            animations[curAnim].tick = 0;
        }
    }

    
    setSource(animations[curAnim].w * animations[curAnim].tick, 
        animations[curAnim].row * animations[curAnim].h, 
        animations[curAnim].w, animations[curAnim].h);

}

#include "entity.h"

int Entity::createCycle(int r, int w, int h, int amount, int speed){
    cycle temp;
    temp.row    = r-1;
    temp.w      = w;
    temp.h      = h;
    temp.amount = amount;
    temp.speed  = speed;
    temp.tick   = 0;
    animations.push_back(temp);
    return animations.size() - 1; //return the index of the newly added cycle
}

void Entity::updateAnimation(float deltaTime){
    if (begin >= animations[curAnim].speed) { 
        begin = 0; 
        animations[curAnim].tick++; 
    }
    begin++;

    if (animations[curAnim].tick >= animations[curAnim].amount) {
        animations[curAnim].tick = 0;
    }

    if (animations[curAnim].tick <= 0){
        if(nAb){
            curAnim = newAnim;
            nAb = 0;
        }
        else{
            animations[curAnim].tick = 0;
        }
    }

    
    setSource(animations[curAnim].w * animations[curAnim].tick, 
        animations[curAnim].row * 97, 
        animations[curAnim].w, animations[curAnim].h);

}
void Entity::StartJump() {
    if (onGround) {
        velY = -12.0f;  // Jump velocity (negative because up is negative in SDL)
        onGround = false;
    }
}

void Entity::EndJump() {
    if (velY < -6.0f) {  // If still rising fast, reduce upward velocity
        velY = -6.0f;
    }
}

void Entity::checkHorizontalCollisions(float newX){
    if (newX < 0) {
        newX = 0;
        velX = 0;
    }
    else if (newX + getDW() > SCREEN_WIDTH) {
        newX = SCREEN_WIDTH - getDW();
        velX = 0;
    }

    // Assign back to the entity
    xpos = newX;
}

void Entity::checkVerticalCollisions(float newY){
    if (newY < 0) {
        newY = 0;
        velY = 0;
    }
    else if (newY + getDH() > SCREEN_HEIGHT) {
        newY = SCREEN_HEIGHT - getDH();
        velY = 0;
        onGround = true;
    }

    // Assign back to the entity
    ypos = newY;
}

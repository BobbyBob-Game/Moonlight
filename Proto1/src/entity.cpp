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

void Entity::Update() {

    velY += GRAVITY;  // Apply gravity each frame
    ypos += velY;     // Move player vertically
    xpos += velX;     // Move player horizontally

    // Ground Collision
    if (ypos > 175.0f) {  // If below ground level
        ypos = 175.0f;    // Snap to ground
        velY = 0.0f;      // Reset vertical speed
        onGround = true;  // Allow jumping again
    }

    // Wall Collision
    if (xpos < 10 || xpos > 190) {
        velX *= -1;  // Reverse direction on walls
    }
}


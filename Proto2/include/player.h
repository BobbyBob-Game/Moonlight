#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"
#include "object.h"

class Player : public Object {
public:
    
    Player(SDL_Renderer* renderer, float posX, float posY, int normalSpeed, int dashSpeed, float velY, bool isJumping, bool isFalling, bool isWallSliding, Uint32 dashStartTime, Uint32 dashCooldownTime);
    ~Player();
    
    void handleEvent(const SDL_Event& e);
    int createCycle(int r, int w, int h, int amount, int speed);
    void setCurrentAnimation(int c) {begin = 0; curAnim = c;}
    void update(float deltaTime);
    void updateAnimation(float deltaTime);
    void render(SDL_Renderer* renderer);
    float getX() {return posX;}
    bool reachedExit() {return posX >= SCREEN_WIDTH - PLAYER_WIDTH;}
    void reset() {posX = 0;}
    
    SDL_Rect getRect() const;
    
private:
    
    SDL_Texture* texture;
    int lastDirection = 0; // -1 for left, 1 for right
    float posX;
    float posY;
    float velX;
    float velY;
    int SPEED, normalSpeed, dashSpeed;
    bool isJumping, isFalling, isWallSliding;
    Uint32 dashStartTime, dashCooldownTime;
    
    
    int currentFrame;       // Current frame index in the sprite sheet
    float frameTimer;       // Accumulated time for frame switching
    int numFrames;          // Total number of animation frames
    
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string &path);
    struct cycle {
        int row;
        int w;
        int h;
        int amount;
        int speed;
        int tick;
    };
    std::vector<cycle> animations;
    int curAnim = 0;
    int begin   = 0;
    bool nAb    = false;
    int newAnim = 0;
    int walking, idle, dashing;
    float animTimer = 0.0f;
    float frameDelay = 0.1f; 
    bool left, right, dash;
};

#endif // PLAYER_H

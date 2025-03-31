#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"
#include "object.h"
#include "levelman.h"

class Player : public Object {
public:
    int levelStartX, levelStartY;
    Player(SDL_Renderer* renderer, float posX, float posY, int normalSpeed, int dashSpeed, float velY, bool isJumping, bool isFalling, bool isWallSliding, Uint32 dashStartTime, Uint32 dashCooldownTime);
    ~Player();
    
    void handleEvent(const SDL_Event& e, bool pauseGame);
    int createCycle(int r, int w, int h, int amount, int speed);
    void setCurrentAnimation(int c) {begin = 0; curAnim = c;}
    void update(float deltaTime, bool pauseGame);
    void updateAnimation(float deltaTime);
    void render(SDL_Renderer* renderer);
    float getX() {return posX;}
    float getY() {return posY;}
    bool reachedExit() {return posX >= SCREEN_WIDTH - PLAYER_WIDTH;}
    void reset(bool hadActivated, int level);
    bool loadSound();
    void stopMovement();
    void updateRenderBox();
    void setCheckpoint(int posXofCheckpoint, int posYofCheckpoint);

    SDL_Rect getRect() const;
    
private:
    Mix_Chunk* walkSound = nullptr;
    Mix_Chunk* jumpSound = nullptr;
    Mix_Chunk* dashSound = nullptr;
    Mix_Music* music = nullptr;
    Uint32 lastWalkSound;
    SDL_Texture* texture;
    SDL_Rect collisionBox;
    SDL_Rect renderBox;
    int collisionWidth = 40;
    int collisionHeight = 60;
    bool reachedCheckpoint;
    
    float posX;
    float posY;
    float velX;
    float velY;
    int checkpointX, checkpointY;
    
    int SPEED, normalSpeed, dashSpeed;
    bool isJumping, isFalling, isWallSliding;
    Uint32 dashStartTime = 0, dashCooldownTime = 0;
    float momentumTimer = 0.0f;
    int dashDirection = 0;
    
    
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
    int walking, idle, dashing, jumping;
    float animTimer = 0.0f;
    float frameDelay = 0.1f; 
    bool left, right, dash, facingLeft;
};

#endif // PLAYER_H

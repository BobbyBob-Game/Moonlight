#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"
#include "object.h"
#include "levelman.h"

class Player : public Object {
public:
    int levelStartX, levelStartY;
    Player(SDL_Renderer* renderer, float posX, float posY, int normalSpeed, int dashSpeed, float velY, bool isJumping, bool isFalling, Uint32 dashStartTime, Uint32 dashCooldownTime);
    ~Player();
    
    void handleEvent(const SDL_Event& e, bool pauseGame);
    int createCycle(int r, int w, int h, int amount, int speed);
    void setCurrentAnimation(int c) {begin = 0; curAnim = c;}
    void update(float deltaTime, bool pauseGame, bool special, const std::vector<std::vector<Tile>>& levelData, int currentLevel);
    void updateAnimation(float deltaTime);
    void render(SDL_Renderer* renderer);
    float getX() {return posX;}
    float getY() {return posY;}
    bool reachedExit(bool special, int levelNum);
    void reset(int level);
    bool loadSound();
    void stopMovement();
    void updateRenderBox();
    bool checkTileCollision(const SDL_Rect& rect, const std::vector<std::vector<Tile>>& levelData, int levelNumber);
    bool isDead(int levelNum) const;
    void flashLight(SDL_Renderer* renderer, int centerX, int centerY, int radius, int screenW, int screenH, bool isOn);

    SDL_Rect getRect() const;
    
private:
    Mix_Chunk* walkSound = nullptr;
    Mix_Chunk* jumpSound = nullptr;
    Mix_Chunk* dashSound = nullptr;
    Uint32 lastWalkSound;
    SDL_Texture* texture;
    SDL_Rect collisionBox;
    SDL_Rect renderBox;
    int collisionWidth = 40;
    int collisionHeight = 50;
    bool reachedCheckpoint;
    float jumpHoldTIme = 0.0f;
    bool jumpHold = false;

    float posX;
    float posY;
    float velX;
    float velY;
    
    int SPEED, normalSpeed, dashSpeed;
    bool isJumping, isFalling;
    Uint32 dashStartTime = 0, dashCooldownTime = 0;
    float momentumTimer = 0.0f;
    int dashDirection = 0;
    
    int currentFrame;      
    float frameTimer;     
    int numFrames;   
    
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

#endif 

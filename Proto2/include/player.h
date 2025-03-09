#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"
#include "object.h"

class Player : public Object {
public:
    
    static constexpr float SPEED = 200.0f;
    
    Player(SDL_Renderer* renderer);
    ~Player();
    
    void handleEvent(const SDL_Event& e);
    int createCycle(int r, int w, int h, int amount, int speed);
    void setCurrentAnimation(int c) {begin = 0; curAnim = c;}
    void update(float deltaTime);
    void updateAnimation(float deltaTime);
    void render(SDL_Renderer* renderer);
    
    SDL_Rect getRect() const;
    
private:
    
    SDL_Texture* texture;
    
    float posX;
    float posY;
    float velX;
    float velY;
    
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
    int walking, idle;
    float animTimer = 0.0f;
    float frameDelay = 0.1f; 
};

#endif // PLAYER_H

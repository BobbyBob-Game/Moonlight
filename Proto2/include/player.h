#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"

class Player {
public:
    
    static const int PLAYER_WIDTH = 64;
    static const int PLAYER_HEIGHT = 64;
    
    
    static constexpr float SPEED = 200.0f;
    
    Player(SDL_Renderer* renderer);
    ~Player();
    
    
    void handleEvent(const SDL_Event& e);
    
    
    void update(float deltaTime);
    
    
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
    float frameDelay;       // Seconds per frame (duration to display each frame)
    int numFrames;          // Total number of animation frames
    
    
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string &path);
};

#endif // PLAYER_H

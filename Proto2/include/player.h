#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"

class Player {
public:
    // Dimensions of the player sprite (size of each animation frame)
    static const int PLAYER_WIDTH = 64;
    static const int PLAYER_HEIGHT = 64;
    
    // Movement speed in pixels per second
    static constexpr float SPEED = 200.0f;
    
    Player(SDL_Renderer* renderer);
    ~Player();
    
    // Handle input events
    void handleEvent(const SDL_Event& e);
    
    // Update player position and animation frame
    void update(float deltaTime);
    
    // Render the player with current animation frame
    void render(SDL_Renderer* renderer);
    
    // Get current position (for collision or other checks)
    SDL_Rect getRect() const;
    
private:
    // The player's texture (sprite sheet)
    SDL_Texture* texture;
    
    // Position and velocity of the player
    float posX;
    float posY;
    float velX;
    float velY;
    
    // Animation variables
    int currentFrame;       // Current frame index in the sprite sheet
    float frameTimer;       // Accumulated time for frame switching
    float frameDelay;       // Seconds per frame (duration to display each frame)
    int numFrames;          // Total number of animation frames
    
    // Load texture helper function
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string &path);
};

#endif // PLAYER_H

#include "player.h"

Player::Player(SDL_Renderer* renderer)
    : texture(nullptr), posX(368), posY(268), velX(0.0f), velY(0.0f),
      currentFrame(0), frameTimer(0.0f), frameDelay(0.1f), numFrames(4) {
    texture = loadTexture(renderer, "assets/Owlboy.png");
    if (!texture) {
        std::cout << "Failed to load player texture!\n";
    }
}

Player::~Player() {
    SDL_DestroyTexture(texture);
    texture = nullptr;
}

SDL_Texture* Player::loadTexture(SDL_Renderer* renderer, const std::string &path) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cout << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << "\n";
        return nullptr;
    }
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!newTexture) {
        std::cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << "\n";
    }
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

void Player::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:    velY -= SPEED; break;
            case SDLK_DOWN:  velY += SPEED; break;
            case SDLK_LEFT:  velX -= SPEED; break;
            case SDLK_RIGHT: velX += SPEED; break;
        }
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:    velY += SPEED; break;
            case SDLK_DOWN:  velY -= SPEED; break;
            case SDLK_LEFT:  velX += SPEED; break;
            case SDLK_RIGHT: velX -= SPEED; break;
        }
    }
}

void Player::update(float deltaTime) {
    // Update the player's position based on velocity and delta time
    posX += velX * deltaTime;
    posY += velY * deltaTime;
    
    // Ensure the player remains within the screen bounds (assuming an 800x600 viewport)
    if (posX < 0) posX = 0;
    if (posX > 800 - PLAYER_WIDTH) posX = 800 - PLAYER_WIDTH;
    if (posY < 0) posY = 0;
    if (posY > 600 - PLAYER_HEIGHT) posY = 600 - PLAYER_HEIGHT;
    
    // Update animation only when the player is moving
    if (velX != 0 || velY != 0) {
        frameTimer += deltaTime;
        if (frameTimer >= frameDelay) {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % numFrames;
        }
    } else {
        // Optionally reset animation when idle
        currentFrame = 0;
        frameTimer = 0.0f;
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_Rect srcRect = { currentFrame * PLAYER_WIDTH, 0, PLAYER_WIDTH, PLAYER_HEIGHT };
    SDL_Rect renderQuad = { static_cast<int>(posX), static_cast<int>(posY), PLAYER_WIDTH, PLAYER_HEIGHT };
    
    SDL_RenderCopy(renderer, texture, &srcRect, &renderQuad);
}

SDL_Rect Player::getRect() const {
    return { static_cast<int>(posX), static_cast<int>(posY), PLAYER_WIDTH, PLAYER_HEIGHT };
}

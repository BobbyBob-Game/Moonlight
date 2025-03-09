#include "player.h"

Player::Player(SDL_Renderer* renderer)
    : texture(nullptr), posX(368), posY(268), velX(0.0f), velY(0.0f),
      currentFrame(0), frameTimer(0.0f), frameDelay(0.1f), numFrames(4) {
    
    texture = loadTexture(renderer, "assets/Owlboy.png");
    if (!texture) {
        std::cout << "Failed to load player texture!\n";
    }
    idle = createCycle(8, PLAYER_WIDTH, PLAYER_HEIGHT, 10, 40);
    walking = createCycle(2, PLAYER_WIDTH, PLAYER_HEIGHT, 12, 30);

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
            case SDLK_UP:
                velY -= SPEED; break;
            case SDLK_DOWN:
                velY += SPEED; break;
            case SDLK_LEFT:  
                velX -= SPEED;
                setCurrentAnimation(walking);
                break;
            case SDLK_RIGHT: 
                velX += SPEED;
                setCurrentAnimation(walking);
                break;
        }
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:    
                velY += SPEED; break;
            case SDLK_DOWN:  
                velY -= SPEED; break;
            case SDLK_LEFT:  
                velX += SPEED;
                setCurrentAnimation(idle);
                break;
            case SDLK_RIGHT: 
                velX -= SPEED;
                setCurrentAnimation(idle);
                break;
        }
    }
}

void Player::update(float deltaTime) {
    // Update the player's position based on velocity and delta time
    posX += velX * deltaTime;
    posY += velY * deltaTime;

    // Keep the player within bounds
    if (posX < 0) posX = 0;
    if (posX > SCREEN_WIDTH - PLAYER_WIDTH) posX = SCREEN_WIDTH - PLAYER_WIDTH;
    if (posY < 0) posY = 0;
    if (posY > SCREEN_HEIGHT - PLAYER_HEIGHT) posY = SCREEN_HEIGHT - PLAYER_HEIGHT;

}


void Player::render(SDL_Renderer* renderer) {
    SDL_Rect renderQuad = { static_cast<int>(posX), static_cast<int>(posY), PLAYER_WIDTH, PLAYER_HEIGHT };
    // Use the updated source rectangle (src) for rendering
    SDL_RenderCopy(renderer, texture, &src, &renderQuad);
}

SDL_Rect Player::getRect() const {
    return { static_cast<int>(posX), static_cast<int>(posY), PLAYER_WIDTH, PLAYER_HEIGHT };
}

void Player::updateAnimation(float deltaTime) {
    // Accumulate elapsed time
    animTimer += deltaTime*2.5f;

    // Advance the frame if enough time has passed
    if (animTimer >= frameDelay) {
        animTimer -= frameDelay;  // subtract frameDelay to handle extra accumulated time
        animations[curAnim].tick++;
    }

    // Loop the animation if necessary
    if (animations[curAnim].tick >= animations[curAnim].amount) {
        animations[curAnim].tick = 0;
    }

    // Update the source rectangle based on the current frame
    setSource(animations[curAnim].w * animations[curAnim].tick, 
              animations[curAnim].row * 97, 
              animations[curAnim].w, animations[curAnim].h);
}


int Player::createCycle(int r, int w, int h, int amount, int speed) {
    cycle temp;
    temp.row = r - 1;
    temp.w = w;
    temp.h = h;
    temp.amount = amount;
    temp.speed = speed;
    temp.tick = 0;
    animations.push_back(temp);
    return animations.size() - 1; // Return the index of the newly added cycle
}


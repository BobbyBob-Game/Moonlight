#include "player.h"

Player::Player(SDL_Renderer* renderer, float posX, float posY, int normalSpeed, int dashSpeed, float velY, bool isJumping, bool isFalling, bool isWallSliding, Uint32 dashStartTime, Uint32 dashCooldownTime)
    : texture(nullptr), posX(368), posY(268), velX(0.0f), velY(0.0f),
      currentFrame(0), frameTimer(0.0f), frameDelay(0.1f), numFrames(4), dashSpeed(dashSpeed) {
    
    SPEED = normalSpeed;
    texture = loadTexture(renderer, "assets/Owlboy.png");
    if (!texture) {
        std::cout << "Failed to load player texture!\n";
    }
    idle = createCycle(8, PLAYER_WIDTH, PLAYER_HEIGHT, 10, 40);
    walking = createCycle(2, PLAYER_WIDTH, PLAYER_HEIGHT, 12, 30);
    dashing = createCycle(20, PLAYER_WIDTH, PLAYER_HEIGHT, 5, 50);
    jumping = createCycle(7, PLAYER_WIDTH, PLAYER_HEIGHT, 9, 10);
    dashDirection = 0;
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
    bool touchingWallLeft = (posX <= 0);
    bool touchingWallRight = (posX >= SCREEN_WIDTH - PLAYER_WIDTH);

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                left = true;
                
                facingLeft = true;
                velX = -SPEED; 
                setCurrentAnimation(walking);
                break;

            case SDLK_RIGHT:
                right = true;
                
                facingLeft = false;
                velX = SPEED; 
                setCurrentAnimation(walking);
                break;

            case SDLK_LSHIFT:
                if (!isWallSliding && SDL_GetTicks() > dashCooldownTime) {
                    dash = true;
                    dashStartTime = SDL_GetTicks();

                    setCurrentAnimation(dashing);
                }
                break;

            case SDLK_SPACE:
                if (!isJumping && !isFalling) {
                    isJumping = true;
                    velY = jumpStrength;
                } else if (isWallSliding) {
                    if (touchingWallLeft) {
                        posX += wallJumpStrengthX;
                        velY = wallJumpStrengthY;
                    } else if (touchingWallRight) {
                        posX -= wallJumpStrengthX;
                        velY = wallJumpStrengthY;
                    }
                    isWallSliding = false;
                }
                setCurrentAnimation(jumping);
                break;
        }
    } 
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                left = false;
                velX = 0; 
                setCurrentAnimation(idle);
                break;
                
            case SDLK_RIGHT:
                right = false;
                velX = 0; 
                setCurrentAnimation(idle);
                break;
                
            case SDLK_LSHIFT:
                
                break;
        }
    }
}

void Player::update(float deltaTime) {
    bool inputActive = left || right;

    if (dash) {
        if (SDL_GetTicks() <= dashStartTime + 200) {
            if(facingLeft) posX -= dashSpeed;
            else posX += dashSpeed;
        } 
        else {
            dash = false;
            if (!inputActive) {
                // Preserve direction after dash ends
                velX = 0;
                if(!(left || right)){
                    momentumTimer = 0.3f; // Maintain momentum for 0.2 seconds
                }
            }
        }
    }
    else {
        // If there is active input, override velocity.
        if (left && !right) {
            velX = -SPEED;
            momentumTimer = 0.0f; // cancel momentum if user provides input
        } else if (right && !left) {
            velX = SPEED;
            momentumTimer = 0.0f;
        } 
        else if (!inputActive) {
            if (momentumTimer > 0.0f) {
                momentumTimer -= deltaTime;
            } else {
                if (velX > 0) {
                    velX -= FRICTION;
                    if (velX < 0)
                        velX = 0;
                } else if (velX < 0) {
                    velX += FRICTION;
                    if (velX > 0)
                        velX = 0;
                }
            }
        }
    }

    if (velX > MAX_SPEED) velX = MAX_SPEED;
    if (velX < -MAX_SPEED) velX = -MAX_SPEED;

    // Determine if we're touching a wall
    bool touchingWallLeft  = (posX <= 0);
    bool touchingWallRight = (posX >= SCREEN_WIDTH - PLAYER_WIDTH);

    // Apply gravity if not wall sliding.
    if (!isWallSliding) {
        velY += GRAVITY * deltaTime;
        if (velY > maxFallSpeed) {
            velY = maxFallSpeed;
        }
    }

    // Additional gravity adjustments:
    if (velY < 0) {
        velY += GRAVITY_UP * deltaTime;
    } else {
        velY += GRAVITY_DOWN * deltaTime;
    }

    const float horizontalMultiplier = 200.0f; // Adjust for desired horizontal speed
    const float verticalMultiplier = 100.0f;   // Adjust for desired vertical speed
    // Update player position with deltaTime scaling.
    posX += velX * deltaTime * horizontalMultiplier;
    posY += velY * deltaTime * verticalMultiplier;

    // WALL SLIDE: If touching a wall while falling, slow vertical velocity.
    if ((touchingWallLeft || touchingWallRight) && velY > 0) {
        isWallSliding = true;
        velY = 1.8f; // slow vertical speed while sliding.
    } else {
        isWallSliding = false;
    }

    // Constrain the player within screen bounds.
    if (posX < 0) posX = 0;
    if (posX > SCREEN_WIDTH - PLAYER_WIDTH) posX = SCREEN_WIDTH - PLAYER_WIDTH;
    if (posY < 0) {
        posY = 0;
        velY = 0;
    }
    if (posY >= SCREEN_HEIGHT - PLAYER_HEIGHT - 10) {
        posY = SCREEN_HEIGHT - PLAYER_HEIGHT - 10;
        velY = 0;
        isJumping = false;
        isFalling = false;
    } else if (velY > 0) {
        isFalling = true;
    }
    
}


void Player::render(SDL_Renderer* renderer) {
    SDL_Rect renderQuad = { static_cast<int>(posX), static_cast<int>(posY), PLAYER_WIDTH, PLAYER_HEIGHT };
    SDL_RendererFlip flip = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, &src, &renderQuad, 0.0, nullptr, flip);
}


SDL_Rect Player::getRect() const {
    return { static_cast<int>(posX), static_cast<int>(posY), PLAYER_WIDTH, PLAYER_HEIGHT };
}

void Player::updateAnimation(float deltaTime) {
    // Accumulate elapsed time
    animTimer += deltaTime*2.0f;

    if (dash && SDL_GetTicks() <= dashStartTime + 200){
        curAnim = dashing;
    }
    else{
        if((curAnim == jumping) && !isJumping && !isFalling ){
            curAnim = idle;
        }

        if(!isJumping && !isFalling && curAnim != idle && curAnim == dashing){
            curAnim = idle;
        }
    }
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


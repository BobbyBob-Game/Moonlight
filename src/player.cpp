#include "player.h"

Player::Player(SDL_Renderer* renderer, float posX, float posY, int normalSpeed, int dashSpeed, float velY, bool isJumping, bool isFalling, bool isWallSliding, Uint32 dashStartTime, Uint32 dashCooldownTime)
    : texture(nullptr),
      posX(368), posY(268), velX(0.0f), velY(0.0f),
      currentFrame(0), frameTimer(0.0f), frameDelay(0.1f), numFrames(4),
      dashSpeed(dashSpeed), lastWalkSound(0),
      left(false), right(false), dash(false), isJumping(false),
      isFalling(false), isWallSliding(false), facingLeft(false) {
    
    checkpointX = posX;
    checkpointY = posY;
    levelStartX = posX;
    levelStartY = posY;
    SPEED = normalSpeed;
    texture = loadTexture(renderer, "assets/Owlboy.png");
    if (!texture) {
        std::cout << "Failed to load player texture!\n";
    }
    idle = createCycle(8, PLAYER_WIDTH, PLAYER_HEIGHT, 10, 40);
    walking = createCycle(2, PLAYER_WIDTH, PLAYER_HEIGHT, 12, 30);
    dashing = createCycle(20, PLAYER_WIDTH, PLAYER_HEIGHT, 5, 50);
    jumping = createCycle(7, PLAYER_WIDTH, PLAYER_HEIGHT, 9, 10);
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

void Player::handleEvent(const SDL_Event& e, bool pauseGame) {
    if(pauseGame){
        stopMovement();
        return;
    }
    bool isMoving = false;
    Uint32 currentTime = SDL_GetTicks();
    bool touchingWallLeft = (posX <= 0);
    bool touchingWallRight = (posX >= SCREEN_WIDTH - PLAYER_WIDTH);

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                left = true;
                isMoving = true;
                facingLeft = true;
                setCurrentAnimation(walking);
                break;

            case SDLK_RIGHT:
                right = true;
                isMoving = true;
                facingLeft = false;
                setCurrentAnimation(walking);
                break;

            case SDLK_LSHIFT:
                if (!isWallSliding && currentTime > dashCooldownTime) {
                    dash = true;
                    dashStartTime = currentTime;
                    Mix_PlayChannel(-1, dashSound, 0);
                    setCurrentAnimation(dashing);
                }
                break;

            case SDLK_SPACE:
                if (!isJumping && !isFalling) {
                    isJumping = true;
                    velY = jumpStrength;
                    Mix_PlayChannel(-1, jumpSound, 0);
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

void Player::update(float deltaTime, bool pauseGame) {
    if(pauseGame){
        stopMovement();
        return;
    }
    bool inputActive = left || right;
    Uint32 currentTime = SDL_GetTicks();

    if (dash) {
        if (SDL_GetTicks() <= dashStartTime + 200) {
            posX += (facingLeft ? -dashSpeed : dashSpeed);
        } else {
            dash = false;
            velX = 0;
        }
    }
    else {
        if (left) {
            velX = -SPEED;
        } else if (right) {
            velX = SPEED;
        } else {
            velX = 0;
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
        isJumping = false;
    } else {
        isWallSliding = false;
    }

    // Constrain the player within screen bounds.
    if (posX < 0) posX = 0;
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
    
    if (inputActive && !isJumping && !isFalling && (currentTime > lastWalkSound + 700)) {
        Mix_PlayChannel(-1, walkSound, 0);
        lastWalkSound = currentTime;
    }
    updateRenderBox();
}

void Player::render(SDL_Renderer* renderer) {
    SDL_RendererFlip flip = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, &src, &renderBox, 0.0, nullptr, flip);
}

SDL_Rect Player::getRect() const {
    return collisionBox;
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
              112, 80);
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

bool Player::loadSound(){
    bool success = true;
    music = Mix_LoadMUS("assets/sound/vanishinghope.mp3");
    if (!music) {
        std::cerr << "Failed to load music: " << Mix_GetError() << '\n';
        success = false;
    }

    // Load sound effects once
    walkSound = Mix_LoadWAV("assets/sound/Walk.wav");
    if (!walkSound) {
        std::cerr << "Failed to load Walk.wav: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(walkSound, MIX_MAX_VOLUME);  // Set volume for walk sound
    }

    jumpSound = Mix_LoadWAV("assets/sound/Jump.wav");
    if (!jumpSound) {
        std::cerr << "Failed to load Jump.wav: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(jumpSound, MIX_MAX_VOLUME);  // Set volume for jump sound
    }

    dashSound = Mix_LoadWAV("assets/sound/Dash.wav");
    if (!dashSound) {
        std::cerr << "Failed to load Dash.wav: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(dashSound, MIX_MAX_VOLUME);  // Set volume for dash sound
    }

    return success;
}

void Player::stopMovement(){
    velX = 0;
    velY = 0;
    dash = false;
    isFalling = false;
    isJumping = false;
    isWallSliding = false;
    left = false;
    right = false;
    setCurrentAnimation(idle);
}

void Player::updateRenderBox(){
    renderBox = {
        static_cast<int>(posX) - 36,  // Offset to center the sprite
        static_cast<int>(posY) - 20,  // Offset to align feet
        112, // Render sprite width
        80   // Render sprite height
    };
    collisionBox = {
        static_cast<int>(posX),
        static_cast<int>(posY),
        collisionWidth,  
        collisionHeight 
    };
}

void Player::setCheckpoint(int posXofCheckpoint, int posYofCheckpoint){
    checkpointX = posXofCheckpoint;
    checkpointY = posYofCheckpoint;
}

void Player::reset(bool hadActivated, int level) {
    if(!hadActivated && (level == 1 || level == 2)){
        posX = checkpointX;
        posY = checkpointY;
    }
    else{
        posX = 0;
        posY = 0;
        checkpointX = posX;
        checkpointY = posY;
    }
    velX = 0;
    velY = 0;
    isJumping = false;
    isFalling = false;
    isWallSliding = false;
}



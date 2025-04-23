#include "player.h"

Player::Player(SDL_Renderer* renderer, float posX, float posY, int normalSpeed, int dashSpeed, float velY, bool isJumping, bool isFalling, Uint32 dashStartTime, Uint32 dashCooldownTime)
    : texture(nullptr),
      posX(posX), posY(posY), velX(0.0f), velY(0.0f),
      currentFrame(0), frameTimer(0.0f), frameDelay(0.1f), numFrames(4),
      dashSpeed(dashSpeed), lastWalkSound(0),
      left(false), right(false), dash(false), isJumping(false),
      isFalling(false), facingLeft(false){

    levelStartX = posX;
    levelStartY = posY;
    SPEED = normalSpeed;
    texture = loadTexture(renderer, "assets/Owlboy.png");
    if (!texture) {
        std::cout << "Failed to load player texture!\n";
    }
    idle = createCycle(8, PLAYER_WIDTH, PLAYER_HEIGHT, 10, 10);
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

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                left = true;
                isMoving = true;
                facingLeft = true;
                break;

            case SDLK_RIGHT:
                right = true;
                isMoving = true;
                facingLeft = false;
                break;

            case SDLK_LSHIFT:
                if ( currentTime > dashCooldownTime) {
                    dash = true;
                    dashStartTime = currentTime + 200;
                    Mix_PlayChannel(-1, dashSound, 0);
                    setCurrentAnimation(dashing);
                }
                break;

                case SDLK_SPACE:
                    if (coyoteTimer <= coyoteTime) { 
                        velY = jumpStrength;
                        isJumping = true;
                        isFalling = false;
                        jumpHold = true;
                        setCurrentAnimation(jumping);
                        Mix_PlayChannel(-1, jumpSound, 0);
                    }               
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

            case SDLK_SPACE:
                jumpHold = false;
                break;
            
        }
    }
}

void Player::update(float deltaTime, bool pauseGame, bool special, const std::vector<std::vector<Tile>>& levelData, int currentLevel) {
    if(pauseGame){
        stopMovement();
        return;
    }
    float newPosX = posX + velX * deltaTime * 200.0f;
    float newPosY = posY + velY * deltaTime * 100.0f;

    bool inputActive = left || right;
    Uint32 currentTime = SDL_GetTicks();

    if (dash) {
        if (currentTime <= dashStartTime + 300) {
            int dashDir = facingLeft ? -1 : 1;
            float dashStep = dashDir * dashSpeed;
    
            float ifX = posX + dashStep;
            SDL_Rect dashRect = {
                static_cast<int>(ifX),
                static_cast<int>(posY),
                collisionWidth,
                collisionHeight
            };
    
            if (!checkTileCollision(dashRect, levelData, currentLevel)) {
                posX = ifX;
            } else {
                dash = false;
            }
        } else {
            dash = false;
        }
    }
    else{
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

    if (velY < 0) {
        if (!jumpHold) {
            velY += GRAVITY_UP * 2.5f * deltaTime; // stronger gravity if jump released
        } else {
            velY += GRAVITY_UP * deltaTime;
        }
    } else {
        velY += GRAVITY_DOWN * deltaTime;
    }

    if (velY > maxFallSpeed) velY = maxFallSpeed;

    SDL_Rect feet = {
        int(posX), int(posY) + collisionHeight + 1,
        collisionWidth, 1
    };

    bool onGround = checkTileCollision(feet, levelData, currentLevel);
    if (onGround) coyoteTimer = 0.0f;
    else          coyoteTimer += deltaTime;
    std::cout << "coyoteTimer: " << coyoteTimer << "\n";


    // Horizontal movement + collision
    float nextPosX = posX + velX * deltaTime * 200.0f;
    SDL_Rect futureXRect = { static_cast<int>(nextPosX), static_cast<int>(posY), collisionWidth, collisionHeight };
    if (!checkTileCollision(futureXRect, levelData, currentLevel)) {
        posX = nextPosX;
    } else {
        velX = 0; // Stop at wall
    }

    // Vertical movement + collision
    float nextPosY = posY + velY * deltaTime * 100.0f;
    SDL_Rect futureYRect = { static_cast<int>(posX), static_cast<int>(nextPosY), collisionWidth, collisionHeight };
    if (!checkTileCollision(futureYRect, levelData, currentLevel)) {
        posY = nextPosY;
        if (velY > 0) isFalling = true;
    } else {
        if (velY > 0) {
            isFalling = false;
            isJumping = false;
        }
        velY = 0; 
    }

    if (posX < 0) posX = 0;
    if(currentLevel == 5){
        if (posX > SCREEN_WIDTH - collisionWidth) {
            posX = SCREEN_WIDTH - collisionWidth;
        }
    }

    if (special) {
        if (posY < 0) {
            posY = 0;
            velY = 0;
        }
        if (posY >= 1280 - PLAYER_HEIGHT) {
            posY = 1280 - PLAYER_HEIGHT;
            velY = 0;
            isJumping = false;
            isFalling = false;
        } else if (velY > 0) {
            isFalling = true;
        }
    } else {
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

    if (inputActive && !isJumping && !isFalling && (currentTime > lastWalkSound + 700)) {
        Mix_PlayChannel(-1, walkSound, 0);
        lastWalkSound = currentTime;
    }

    updateRenderBox();
    updateAnimation(deltaTime);

}

void Player::render(SDL_Renderer* renderer) {
    SDL_RendererFlip flip = facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, &src, &renderBox, 0.0, nullptr, flip);
}

SDL_Rect Player::getRect() const {
    return collisionBox;
}

void Player::updateAnimation(float deltaTime) { //TODO: fix animation
    animTimer += deltaTime * 2.0f;

    if (dash && SDL_GetTicks() <= dashStartTime + 200) {
        curAnim = dashing;
    } else if (isJumping || isFalling) {
        curAnim = jumping;
    } else if (left || right) {
        curAnim = walking;
    } else {
        curAnim = idle;
    }

    if (animTimer >= frameDelay) {
        animTimer -= frameDelay;
        animations[curAnim].tick++;
    }

    if (animations[curAnim].tick >= animations[curAnim].amount) {
        animations[curAnim].tick = 0;
    }

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
    return animations.size() - 1;
}

bool Player::loadSound(){
    bool success = true;

    walkSound = Mix_LoadWAV("assets/sound/Walk.wav");
    if (!walkSound) {
        std::cerr << "Failed to load Walk.wav: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(walkSound, MIX_MAX_VOLUME);
    }

    jumpSound = Mix_LoadWAV("assets/sound/Jump.wav");
    if (!jumpSound) {
        std::cerr << "Failed to load Jump.wav: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(jumpSound, MIX_MAX_VOLUME);
    }

    dashSound = Mix_LoadWAV("assets/sound/Dash.wav");
    if (!dashSound) {
        std::cerr << "Failed to load Dash.wav: " << Mix_GetError() << '\n';
        success = false;
    } else {
        Mix_VolumeChunk(dashSound, MIX_MAX_VOLUME);
    }

    return success;
}

void Player::stopMovement(){
    velX = 0;
    velY = 0;
    dash = false;
    isFalling = false;
    isJumping = false;
    left = false;
    right = false;
    setCurrentAnimation(idle);
}

void Player::updateRenderBox(){
    renderBox = {
        static_cast<int>(posX) - 36,  
        static_cast<int>(posY) - 20,  
        112,
        80
    };
    collisionBox = {
        static_cast<int>(posX),
        static_cast<int>(posY),
        collisionWidth,  
        collisionHeight 
    };
}

void Player::reset(int level) {
    if(level == 4){
        posX = PORTRAIT_WIDTH / 2;
        posY = 0;
    }
    else{
        posX = levelStartX;
        posY = levelStartY;
    }
    stopMovement();
}

const SDL_Rect VERTICAL_EXIT_RECT = {720 + PLAYER_WIDTH / 2, 1280 - PLAYER_HEIGHT, 32, 32};
const SDL_Rect PRE_VERTICAL_EXIT_RECT = {416, SCREEN_HEIGHT - PLAYER_HEIGHT / 2, 672, 96};

bool Player::reachedExit(bool special, int levelNum){ 
    SDL_Rect playerRect = {
        static_cast<int>(posX),
        static_cast<int>(posY),
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    };
    if(special){
        return SDL_HasIntersection(&playerRect, &VERTICAL_EXIT_RECT);
    }
    else if(levelNum == 3){
        return SDL_HasIntersection(&playerRect, &PRE_VERTICAL_EXIT_RECT);
    }
    else{
        return posX >= LANDSCAPE_WIDTH ;
    }
}

bool Player::checkTileCollision(const SDL_Rect& rect, const std::vector<std::vector<Tile>>& levelData, int levelNumber) {
    int leftTile   = rect.x / TILE_SIZE;
    int rightTile  = (rect.x + rect.w - 1) / TILE_SIZE;
    int topTile    = rect.y / TILE_SIZE;
    int bottomTile = (rect.y + rect.h - 1) / TILE_SIZE;

    for (int y = topTile; y <= bottomTile; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            if (y >= 0 && y < levelData.size() && x >= 0 && x < levelData[y].size()) {
                if (LevelManager::isSolid(levelData[y][x].type, levelNumber)) {
                    return true; 
                }
            }
        }
    }
    return false; 
}

bool Player::isDead(int levelNum) const {
    if(levelNum == 4){ 
        return posY == 1200;
    }
    else if(levelNum == 0 || levelNum == 1 || levelNum == 2 || levelNum == 5 ){
        return posY == 620;
    }
    else if(levelNum == 3){
        return false;
    }
    return false;
}
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x * x + y * y <= radius * radius) { 
                SDL_SetRenderDrawColor(renderer, r, g, b, a);
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void Player::flashLight(SDL_Renderer* renderer, int centerX, int centerY, int radius, int screenW, int screenH, bool isOn){
    SDL_Texture* overlay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screenW, screenH);
    if (!overlay) {
        SDL_Log("Failed to create overlay texture: %s", SDL_GetError());
        return;
    }
    SDL_SetTextureBlendMode(overlay, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, overlay);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE); // Force overwrite
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 230);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    drawCircle(renderer, centerX, centerY, radius, 0, 0, 0, 0);

    SDL_SetRenderTarget(renderer, nullptr);

    SDL_RenderCopy(renderer, overlay, nullptr, nullptr);

    SDL_DestroyTexture(overlay);
}

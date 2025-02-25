#include "game.h"
#include "defs.h"

enum Direction {
    IDLE_RIGHT,
    IDLE_LEFT,
    RUNNING_RIGHT,
    RUNNING_LEFT
};

Direction characterState = IDLE_RIGHT;
GameState gameState = STATE_MENU;

Game::Game() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        running = false;
        return;
    }
    if (TTF_Init() == -1) {
        std::cerr << "True type font could not be initialized! Error: " << TTF_GetError() << "\n";
        running = false;
        return;
    }

    window = SDL_CreateWindow("Moonlight", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        std::cerr << "Failed to create window! SDL_Error: " << SDL_GetError() << "\n";
        running = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Failed to create renderer! SDL_Error: " << SDL_GetError() << "\n";
        running = false;
        return;
    }

    font = TTF_OpenFont("assets/FONTS/8bitOperatorPlus8-Regular.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font! Error: " << TTF_GetError() << "\n";
        running = false;
        return;
    }

    running = true;
    count = 0;

    // Load background
    background.setImage("assets/MAP/Clouds 3/1.png", renderer); //ingame
    menu.setBackground("assets/BUTTONS/0.png", renderer);

    // Load player
    player.setImage("assets/PLAYER/player.png", renderer);


    player.setDest(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100, 63, 63);

    // Ensure animations are valid
    idle_right = player.createCycle(2, 63, 63, 10, 10);
    run_right = player.createCycle(1, 63, 63, 16, 10);
    idle_left = player.createCycle(6, 63, 63, 10, 10);
    run_left = player.createCycle(5, 63, 63, 16, 10);

    start_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 69, 288, 64, "Start a new game");
    exit_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2, 288, 64, "Exit ...");

    menu.setButtonTexture(start_button, "assets/BUTTONS/buttons1.png", renderer);
    menu.setButtonTexture(exit_button, "assets/BUTTONS/buttons1.png", renderer);
    menu.loadArrow("assets/BUTTONS/select_button.png", renderer);

    loadMap("assets/MAP/Ground.level");

    mapX = mapY = 0;
    scrollingSpeed = 1;
    loop();
}

Game::~Game(){
    TTF_CloseFont(font);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::loop() {
    const int FRAME_DELAY = 1000 / 60; // Limit to 60 FPS
    Uint32 frameStart;
    int frameTime;

    while (running) {
        frameStart = SDL_GetTicks();

        render(); // Draw everything
        input();  // Handle input
        update(); // Update game state

        // Frame rate control
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}


void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (background.getTex()) {
        SDL_Rect bgRectDest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, background.getTex(), NULL, &bgRectDest);
    } else {
        std::cerr << "Warning: Background texture is NULL!\n";
    }

    if (gameState == STATE_MENU) {
        menu.renderBackground(renderer);
        menu.button_render(renderer, font);
        drawLetter("Moonlight", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 4, 255, 255, 255, 40);
    } 
    else if (gameState == STATE_GAME) {
        drawMap();
        draw(player);
    }

    SDL_RenderPresent(renderer);
}



void Game::draw(Object o) {
    SDL_Rect dest = o.getDest();
    SDL_Rect src = o.getSource();

    if (!o.getTex()) {
        std::cerr << "Error: Object texture is NULL for " <<"\n";
        return;
    }

    SDL_RenderCopyEx(renderer, o.getTex(), &src, &dest, 0, NULL, SDL_FLIP_NONE);
}


void Game::input() { //TODO: make the player jump
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            running = false;
        }
        if (gameState == STATE_MENU) {
            menu.updateWidget(event, gameState);
            if (gameState == STATE_END) {
                running = false;
            }
        }
        

        else if(gameState == STATE_GAME){
                
            // Handle key press (only change state once)
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_RIGHT && characterState != RUNNING_RIGHT && event.key.repeat == 0){
                    left = 0;
                    right = 1;
                    player.setCurrentAnimation(run_right);
                    characterState = RUNNING_RIGHT;
                }
                if(event.key.keysym.sym == SDLK_LEFT && characterState != RUNNING_LEFT && event.key.repeat == 0){
                    left = 1;
                    right = 0;

                    player.setCurrentAnimation(run_left);
                    characterState = RUNNING_LEFT;
                }
                if(event.key.keysym.sym == SDLK_UP && !isJumping && !fall){
                    jumpBuffer = JUMP_BUFFER_FRAME;
                }

            }
            // Handle key release (only change state once)
            if(event.type == SDL_KEYUP) {
                if(event.key.keysym.sym == SDLK_RIGHT && characterState == RUNNING_RIGHT){
                    right = 0; // Stop movement
                    left = 0;
                    player.setCurrentAnimation(idle_right);
                    characterState = IDLE_RIGHT;
                }
                if(event.key.keysym.sym == SDLK_LEFT && characterState == RUNNING_LEFT){
                    left = 0; // Stop movement
                    right = 0;
                    player.setCurrentAnimation(idle_left);
                    characterState = IDLE_LEFT;
                }
            }
        }        
    }
}


void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    float dt = (currentTime - lastFrameTime) / 1000.0f; // Delta time in seconds
    lastFrameTime = currentTime;

    if(gameState == STATE_MENU){
        menu.updateBackground(dt);
    }
    
    if(gameState == STATE_GAME){
        if(isJumping || fall){
            gravity_timer ++;
            float gravityEffect = (gravity_timer < 5) ? (GRAVITY*0.5f) : GRAVITY;
            UpVelocity += gravityEffect;
            if(UpVelocity > MAX_FALL_SPEED){
                UpVelocity = MAX_FALL_SPEED;
            }
        }
        if(!isJumping && fall){
            flyingTimer = (flyingTimer > 0) ? flyingTimer - 1 : 0;
        }

        if(jumpBuffer > 0) jumpBuffer--; //decrease jump buffer timer

        if(jumpBuffer > 0 && (flyingTimer > 0 || !fall)){
            isJumping = true;
            UpVelocity = jumpForce;
            gravity_timer = 0;
            jumpBuffer = 0;
            jumpHoldTimer = JUMP_HOLD_TIMER;
        }
        if(jumpHoldTimer > 0 && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP]){
            UpVelocity += JUMP_HOLD_FORCE;
            jumpHoldTimer--;
        }

        player.setDest(player.getDX(), player.getDY() + UpVelocity);
        // Apply horizontal movement
        if(left) {
            player.setDest(player.getDX() - runningSpeedNormal, player.getDY());
        }
        else if(left && isJumping){
            player.setDest(player.getDX() - runningSpeedNormal, player.getDY() + UpVelocity);
        }
        if(right) {
            player.setDest(player.getDX() + runningSpeedNormal, player.getDY());
        }
        else if(right && isJumping){
            player.setDest(player.getDX() + runningSpeedNormal, player.getDY() + UpVelocity);
        }
        
        // Collision handling
        fall = true; 
        for (int i = 0; i < map.size(); i++) {
            if (collision(player, map[i]) && map[i].getSolid()) {
                if (UpVelocity > 0) { //falling
                    isJumping = false;
                    fall = false;
                    UpVelocity = 0;

                    // Align player's feet exactly on top of the platform
                    player.setDest(player.getDX(), map[i].getDY() - player.getDH());
                    flyingTimer = FLYING_TIME_FRAME;
                }
                // If moving up (jumping) and hitting ceiling, stop upward movement
                else if (UpVelocity < 0) {
                    UpVelocity = 0;
                    player.setDest(player.getDX(), map[i].getDY() + map[i].getDH());
                }
            }
        }

        if(water && left){
            player.setDest(player.getDX() + (runningSpeedNormal)- (runningSpeedWater), player.getDY());
        }
        if(water && right){
            player.setDest(player.getDX() - (runningSpeedNormal) + (runningSpeedWater), player.getDY());
        }
        //Need to know the position of player:
        //cout<<player.getDX()<<":" <<player.getDY()<<"\n";

        menu.setDest(0, 0, 128, 64);

        player.updateAnimation();
    }
}

void Game::loadMap(const char *filename){
    Object temp; //creating a rect for each tile
    temp.setImage("assets/MAP/Untitled.png", renderer);
    int current, mx, my, mw,mh;
    ifstream in(filename);
    if(!in.is_open()){
        cout<<"Failed to open map file"<<"\n";
        return;
    }
    in >> mw;
    in >> mh;
    in >> mx;
    in >> my;
    for(int i = 0; i < mh; i++){
        for(int j = 0; j < mw; j++){
            if(in.eof()){
                cout<<"Reached end of file too soon\n";
                return;
            }
            in >> current;
            if(current != 0){ // not sky | the tileset must be in order
                temp.setSource((current - 1)*TILE_SIZE, 0, TILE_SIZE, TILE_SIZE);
                temp.setDest((j*TILE_SIZE) + mx, (i*TILE_SIZE) + my, TILE_SIZE, TILE_SIZE);
                temp.setID(current);
                if(current == 1){temp.setSolid(1);}
                if(current == 2){temp.setSolid(0);}
                map.push_back(temp);
            }
        }
    }
    in.close();
}

void Game::drawMap(){ //TODO: make map parallax scrolling
    for(int i = 0; i < map.size(); i++){
        if(map[i].getDX() >= mapX - TILE_SIZE& map[i].getDY() >= mapY & map[i].getDX() <= mapX + SCREEN_WIDTH + TILE_SIZE){
            draw(map[i]);
        }
    }
}

bool Game::collision(Object a, Object b){
    if((a.getDX() < b.getDX() + b.getDW()) && 
    (a.getDX() + a.getDW() > b.getDX()) && 
    (a.getDY() < b.getDY() + b.getDH()) && 
    (a.getDY() + a.getDH() > b.getDY())) {  
        return true;
    }

    else{
        return false;
    }
}

void Game::drawLetter(const char* msg, int x, int y, int r, int g, int b, int size) {

    TTF_Font* font = TTF_OpenFont("assets/FONTS/8bitOperatorPlus8-Regular.ttf", size);
    if (font == nullptr) {
        std::cerr << "Error: Font failed to load! " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 255;

    SDL_Surface *surfText = TTF_RenderText_Solid(font, msg, color);
    if (surfText == nullptr) {
        std::cerr << "Error: Failed to create text surface! " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfText);
    if (textureText == nullptr) {
        std::cerr << "Error: Failed to create texture from text! " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surfText);
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect textholder;
    textholder.x = x;
    textholder.y = y;
    textholder.w = surfText->w;
    textholder.h = surfText->h;

    SDL_RenderCopy(renderer, textureText, NULL, &textholder);

    SDL_FreeSurface(surfText);
    SDL_DestroyTexture(textureText);
    TTF_CloseFont(font);
}
#include "game.h"
#include "defs.h"
Uint32 lastFrameTime = SDL_GetTicks();
float dt = 0.0f;
Direction characterState = IDLE_RIGHT;
GameState gameState = STATE_MENU;

Game::Game() : dialogueBox(renderer,font), currentMap(1, nullptr){
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
    background.setImage("assets/MAP/background_cave.png", renderer); //ingame
    dia.setImage("assets/BUTTONS/dialogue.png", renderer);
    gameTitle.setImage("assets/BUTTONS/game_title.png", renderer);
    menu.setBackground("assets/BUTTONS/0.png", renderer);

    // Load player
    player.setImage("assets/PLAYER/Owlboy.png", renderer);
    player.setDest(0, 0, 72, 72);

    // Ensure animations are valid
    idle_right = player.createCycle(8, 112, 80, 9, 10);
    run_right = player.createCycle(2, 112, 80, 10, 10);

    //buttons on menu
    start_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 69, 288, 64, "Start a new game");
    exit_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2, 288, 64, "Exit ...");

    menu.setButtonTexture(start_button, "assets/BUTTONS/buttons1.png", renderer);
    menu.setButtonTexture(exit_button, "assets/BUTTONS/buttons1.png", renderer);
    menu.loadArrow("assets/BUTTONS/select_button.png", renderer);

    //Dialogue
    dialogueBox.font = font;
    dialogueBox.renderer = renderer;

    //Map:
    dimensionID = 1; 
    currentMap = Map(dimensionID, renderer);
    currentMap.loadTileTexture(); 
    currentMap.loadMap("assets/MAP/first_level.csv", 40, 30);
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

    if (gameState == STATE_MENU) {
        menu.renderBackground(renderer);
        SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 4 - 100, 300, 200};
        SDL_RenderCopy(renderer, gameTitle.getTex(), NULL, &titleRect);
        menu.button_render(renderer, font);
        drawLetter("Moonlight", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 4, 255, 255, 255, 40);
    } 
    else if (gameState == STATE_GAME) {
        if (background.getTex()) {
            SDL_Rect bgRectDest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, background.getTex(), NULL, &bgRectDest);
        } else {
            std::cerr << "Warning: Background texture is NULL!\n";
        }
        //test dialogue:

        currentMap.drawMap();
        SDL_Rect diaRect = {SCREEN_WIDTH / 2 - 350, 10, 700, 160};
        SDL_RenderCopy(renderer, dia.getTex(), NULL, &diaRect);
        dialogueBox.render();
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

    SDL_RendererFlip flipType = SDL_FLIP_NONE;
    if(o.facingLeft){
        flipType = SDL_FLIP_HORIZONTAL;
    }

    SDL_RenderCopyEx(renderer, o.getTex(), &src, &dest, 0, NULL, flipType);
}

void Game::input() {
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

            if(event.type == SDL_MOUSEBUTTONDOWN){
                dimensionID = (dimensionID == 1) ? 2 : 1;
                currentMap = Map(dimensionID, renderer);
                currentMap.loadTileTexture();
                currentMap.loadMap(dimensionID == 1 ? "assets/MAP/first_level.csv" : "assets/MAP/dimension2", 40, 30);
            }
            // Handle key press (only change state once)
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_RIGHT && characterState != RUNNING_RIGHT && event.key.repeat == 0){
                    left = 0;
                    right = 1;
                    player.setCurrentAnimation(run_right);
                    player.facingLeft = false;
                    characterState = RUNNING_RIGHT;
                }
                if(event.key.keysym.sym == SDLK_LEFT && characterState != RUNNING_LEFT && event.key.repeat == 0){
                    left = 1;
                    right = 0;
                    player.setCurrentAnimation(run_right);
                    player.facingLeft = true;
                    characterState = RUNNING_LEFT;
                }
                if(event.key.keysym.sym == SDLK_SPACE) {
                    jump = true;
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
                    player.setCurrentAnimation(idle_right);
                    characterState = IDLE_LEFT;
                }
            }
        }        
    }
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    dt = (currentTime - lastFrameTime) / 1000.0f; 
    lastFrameTime = currentTime;

    if(gameState == STATE_MENU) {
        menu.updateBackground(dt);
    }
    
    if(gameState == STATE_GAME) {
        dialogueBox.update(SDL_GetTicks());
        // --- Horizontal Movement ---
        if (left) {
            velX -= ACCELERATION;  // Accelerate left
        } 
        if (right) {
            velX += ACCELERATION;  // Accelerate right
        }

        if (velX > MAX_SPEED) velX = MAX_SPEED;
        if (velX < -MAX_SPEED) velX = -MAX_SPEED;

        if (!left && !right) {
            if (velX > 0) {
                velX -= FRICTION;
                velX -= DECELERATION;
                if (velX < 0) velX = 0;
            } else if (velX < 0) {
                velX += FRICTION;
                velX += DECELERATION;
                if (velX > 0) velX = 0;
            }
        }

        if (jump && onGround) {
            velY = -JUMP_FORCE;  // Launch upward
            onGround = false;
            jump = false;
        }

        velY += GRAVITY * dt;
        if (velY < 0) {
            velY += GRAVITY_UP * dt;
        } else {
            velY += GRAVITY_DOWN * dt;
        }
        
        // Update player position
        player.setDest(player.getDX() + velX, player.getDY() + velY);
        currentMap.tileCollision(player, velX, velY, onGround);
        currentMap.clampObjectPosition(player, velX, velY, onGround);
        
        menu.setDest(0, 0, 128, 64);
        player.updateAnimation(dt);
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

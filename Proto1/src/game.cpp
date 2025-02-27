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
    background.setImage("assets/MAP/background_cave.png", renderer); //ingame
    dia.setImage("assets/BUTTONS/dialogue.png", renderer);
    gameTitle.setImage("assets/BUTTONS/game_title.png", renderer);
    menu.setBackground("assets/BUTTONS/0.png", renderer);

    // Load player
    player.setImage("assets/PLAYER/player.png", renderer);
    player.setDest(0, 0, 70, 70);

    // Ensure animations are valid
    idle_right = player.createCycle(2, 63, 63, 10, 10);
    run_right = player.createCycle(1, 63, 63, 16, 10);
    idle_left = player.createCycle(6, 63, 63, 10, 10);
    run_left = player.createCycle(5, 63, 63, 16, 10);

    //buttons on menu
    start_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2 - 69, 288, 64, "Start a new game");
    exit_button = menu.createWidget(SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 2, 288, 64, "Exit ...");

    menu.setButtonTexture(start_button, "assets/BUTTONS/buttons1.png", renderer);
    menu.setButtonTexture(exit_button, "assets/BUTTONS/buttons1.png", renderer);
    menu.loadArrow("assets/BUTTONS/select_button.png", renderer);

    //Dialogue
    dialogueBox.font = font;
    dialogueBox.renderer = renderer;

    //Map
    loadTileTexture();
    loadMap("assets/MAP/first_level.csv", 40,30);

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

        drawMap();
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

    SDL_RenderCopyEx(renderer, o.getTex(), &src, &dest, 0, NULL, SDL_FLIP_NONE);
}


void Game::input() { //TODO: make the player jump
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            running = false;
        }
        if(event.type == SDL_MOUSEBUTTONDOWN){
            cout<<"mouse clicked\n";
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
        dialogueBox.update(currentTime);
        // Apply horizontal movement
        if(left) {
            player.setDest(player.getDX() - runningSpeedNormal, player.getDY());
        }
        if(right) {
            player.setDest(player.getDX() + runningSpeedNormal, player.getDY());
        }
        
        // Collision handling
        //outside if map
        if(player.getDX() < 0){
            player.setDest(0, player.getDY());
        }
        else if(player.getDX() + player.getDW() > SCREEN_WIDTH){
            player.setDest(SCREEN_WIDTH - player.getDW() + 1, player.getDY());
        }
        else if(player.getDY() < 0){
            player.setDest(player.getDX(),0);
        }
        else if(player.getDY() + player.getDH() > SCREEN_HEIGHT){
            player.setDest(player.getDX(),SCREEN_HEIGHT - player.getDH());
        }




        //falling
        fall = true; 
        int tileX = player.getDX() / TILE_SIZE;
        int tileY = (player.getDY() + player.getDH()) / TILE_SIZE; //the left corner
        if(tileMap[tileY][tileX] == 6 || tileMap[tileY][tileX] == 7){
            fall = false;
            player.setDest(player.getDX(), tileY * TILE_SIZE - player.getDH() - 12);
        }
        else{
            fall = true;
        }

        menu.setDest(0, 0, 128, 64);

        player.updateAnimation();
    }
}

void Game::loadMap(const char *filename, int sizeX, int sizeY){
    fstream mapFile(filename);
    if (!mapFile.is_open()) {
        cerr << "Error: Failed to open map file: " << filename << endl;
        return;
    }

    tileMap.clear();
    string line;

    for(int y = 0; y < sizeY; y++){
        vector<int> row;
        if (!getline(mapFile, line)) {
            cerr << "Error: Unexpected end of file in map!" << endl;
            break;
        }
        stringstream ss(line); // to get doiuble digits number

        for(int x = 0; x < sizeX; x++){
            int tileID;
            if (!(ss >> tileID)) {  // Read full number instead of a single char
                cerr << "Error: Invalid tile at (" << x << "," << y << ")" <<endl;
                tileID = 0; // Default to empty
            }
            row.push_back(tileID);
            if (ss.peek() == ',') ss.ignore(); // Skip comma
        }
        tileMap.push_back(row);
    }

    mapFile.close();
}


void Game::drawMap() {
    for (int y = 0; y < tileMap.size(); y++) {
        for (int x = 0; x < tileMap[y].size(); x++) {
            int tileID = tileMap[y][x]; // Get tile ID from tileMap
            cout << "Rendering tile ID: " << tileID << " at (" << x << "," << y << ")\n";
            if (tileTextures.find(tileID) != tileTextures.end()) {
                SDL_Texture* texture = tileTextures[tileID];

                SDL_Rect destRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, texture, NULL, &destRect);
            }
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

void Game::AddTile(int id, int x, int y){
    tileMap[y][x] = id;
}

void Game::loadTileTexture(){
    tileTextures[1] = IMG_LoadTexture(renderer, "assets/MAP/tile1.png");
    tileTextures[3] = IMG_LoadTexture(renderer, "assets/MAP/tile3.png");
    tileTextures[5] = IMG_LoadTexture(renderer, "assets/MAP/tile5.png");
    tileTextures[6] = IMG_LoadTexture(renderer, "assets/MAP/tile6.png");
    tileTextures[7] = IMG_LoadTexture(renderer, "assets/MAP/tile7.png");
    tileTextures[8] = IMG_LoadTexture(renderer, "assets/MAP/tile8.png");
    tileTextures[21] = IMG_LoadTexture(renderer, "assets/MAP/tile21.png");
    tileTextures[22] = IMG_LoadTexture(renderer, "assets/MAP/tile22.png");
    tileTextures[24] = IMG_LoadTexture(renderer, "assets/MAP/tile24.png");
    tileTextures[42] = IMG_LoadTexture(renderer, "assets/MAP/tile42.png");
    tileTextures[63] = IMG_LoadTexture(renderer, "assets/MAP/tile63.png");
    tileTextures[64] = IMG_LoadTexture(renderer, "assets/MAP/tile64.png");
    tileTextures[65] = IMG_LoadTexture(renderer, "assets/MAP/tile65.png");
    tileTextures[66] = IMG_LoadTexture(renderer, "assets/MAP/tile66.png");

    for (const auto& pair : tileTextures) {
        if (!pair.second) {
            std::cerr << "Error: Failed to load texture for tile ID " << pair.first 
                      << "! SDL_Error: " << IMG_GetError() << "\n";
        }
    }
}
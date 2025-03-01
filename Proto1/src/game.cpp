#include "game.h"
#include "defs.h"

enum Direction {
    IDLE_RIGHT,
    IDLE_LEFT,
    RUNNING_RIGHT,
    RUNNING_LEFT,
    DASH_LEFT,
    DASH_RIGHT
};

Uint32 lastFrameTime = SDL_GetTicks();
float dt = 0.0f;
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
    player.setDest(0, 0, 72, 72);

    // Ensure animations are valid
    idle_right = player.createCycle(2, 63, 63, 10, 10);
    run_right = player.createCycle(1, 63, 63, 16, 10);
    idle_left = player.createCycle(6, 63, 63, 10, 10);
    run_left = player.createCycle(5, 63, 63, 16, 10);
    dash = player.createCycle(9, 63, 63, 5, 10);

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
    loadTileTexture(dimensionID); 
    loadMap("assets/MAP/first_level.csv", 40, 30);


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
        if (gameState == STATE_MENU) {
            menu.updateWidget(event, gameState);
            if (gameState == STATE_END) {
                running = false;
            }
        }
        

        else if(gameState == STATE_GAME){

            if(event.type == SDL_MOUSEBUTTONDOWN){
                cout<<"mouse clicked\n";
                if(dimensionID == 1){
                    dimensionID = 2;
                }                
                else {
                    dimensionID = 1;
                }
                cout<<"Current dimension id is: "<<dimensionID<<"\n";
                loadTileTexture(dimensionID);
                loadMap(dimensionID == 1 ? "assets/MAP/first_level.csv" : "assets/MAP/dimension2", 40, 30);
            }
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
                if(event.key.keysym.sym == SDLK_LSHIFT && event.key.repeat == 0){
                    player.setCurrentAnimation(dash);
                    characterState = DASH_RIGHT;
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
    dt = (currentTime - lastFrameTime) / 1000.0f; 
    lastFrameTime = currentTime;

    if(gameState == STATE_MENU){
        menu.updateBackground(dt);
    }
    
    if(gameState == STATE_GAME){
        dialogueBox.update(currentTime);

        if (left) {
            velX -= ACCELERATION;  // Accelerate left
        } 
        if (right) {
            velX += ACCELERATION;  // Accelerate right
        }
        
        // Apply speed limit
        if (velX > MAX_SPEED) velX = MAX_SPEED;
        if (velX < -MAX_SPEED) velX = -MAX_SPEED;
        
        // Apply friction if no movement keys are pressed
        if (!left && !right) {
            if (velX > 0) {
                velX -= FRICTION;
                if (velX < 0) velX = 0;  // Stop completely
            }
            else if (velX < 0) {
                velX += FRICTION;
                if (velX > 0) velX = 0;  // Stop completely
            }
        }
        
        // Update player position
        player.setDest(player.getDX() + velX, player.getDY());
        
        // Collision handling
        //outside if map
        if(player.getDX() < 0){
            player.setDest(0, player.getDY());
        }
        else if(player.getDX() + player.getDW() > SCREEN_WIDTH){
            player.setDest(SCREEN_WIDTH - player.getDW(), player.getDY());
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
        if(fall){
            player.setDest(player.getDX(), player.getDY() + GRAVITY);
            if (tileY >= 0 && tileY < tileMap1.size() && 
                tileX >= 0 && tileX < tileMap1[tileY].size()) {
                    
                if(tileMap1[tileY][tileX] == 6 || tileMap1[tileY][tileX] == 7){
                    fall = false;
                    player.setDest(player.getDX(), tileY*TILE_SIZE - player.getDH());
                }
            }
        }
        

        menu.setDest(0, 0, 128, 64);

        player.updateAnimation(dt);
    }
}

void Game::loadMap(const char *filename, int sizeX, int sizeY) {
    fstream mapFile(filename);
    if (!mapFile.is_open()) {
        cerr << "Error: Failed to open map file: " << filename << endl;
        return;
    }

    // Initialize maps with proper dimensions
    vector<vector<int>> newMap(sizeY, vector<int>(sizeX, 0));
    
    string line;
    for(int y = 0; y < sizeY; y++) {
        if (!getline(mapFile, line)) {
            cerr << "Error: Unexpected end of file at line " << y << endl;
            break;
        }

        stringstream ss(line);
        for(int x = 0; x < sizeX; x++) {
            int tileID = 0;
            if (ss >> tileID) {
                newMap[y][x] = tileID;
            }
            if (ss.peek() == ',') ss.ignore();
        }
    }

    // Safely assign to correct dimension
    if (dimensionID == 1) {
        tileMap1 = newMap;
    } else {
        tileMap2 = newMap;
    }

    mapFile.close();
}


void Game::drawMap() {
    if (dimensionID == 1) {
        if (tileMap1.empty()) return; // Check if tileMap1 is empty
        for (int y = 0; y < tileMap1.size(); y++) {
            for (int x = 0; x < tileMap1[y].size(); x++) {
                int tileID = tileMap1[y][x];
                if (tileTextures1.find(tileID) != tileTextures1.end()) {
                    SDL_Texture* texture = tileTextures1[tileID];
                    if (texture) { // Check if texture is not null
                        SDL_Rect destRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                    }
                }
            }
        }
    } else {
        if (tileMap2.empty()) return; // Check if tileMap2 is empty
        for (int y = 0; y < tileMap2.size(); y++) {
            for (int x = 0; x < tileMap2[y].size(); x++) {
                int tileID = tileMap2[y][x];
                if (tileTextures2.find(tileID) != tileTextures2.end()) {
                    SDL_Texture* texture = tileTextures2[tileID];
                    if (texture) { // Check if texture is not null
                        SDL_Rect destRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                    }
                }
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

void Game::loadTileTexture(int dimensionID) {
    std::unordered_map<int, std::string> tileTexturePaths;

    if (dimensionID == 1) {
        tileTexturePaths = {
            {1, "assets/MAP/Dimension1/tile1.png"}, {3, "assets/MAP/Dimension1/tile3.png"},
            {5, "assets/MAP/Dimension1/tile5.png"}, {6, "assets/MAP/Dimension1/tile6.png"},
            {7, "assets/MAP/Dimension1/tile7.png"}, {8, "assets/MAP/Dimension1/tile8.png"},
            {21, "assets/MAP/Dimension1/tile21.png"}, {22, "assets/MAP/Dimension1/tile22.png"}, {24, "assets/MAP/Dimension1/tile24.png"},
            {42, "assets/MAP/Dimension1/tile42.png"}, {63, "assets/MAP/Dimension1/tile63.png"}, {64, "assets/MAP/Dimension1/tile64.png"},
            {65, "assets/MAP/Dimension1/tile65.png"}, {66, "assets/MAP/Dimension1/tile66.png"}
        };
    } else {
        tileTexturePaths = {
            {1, "assets/MAP/Dimension2/di2_tile1.png"}, {3, "assets/MAP/Dimension2/di2_tile3.png"},
            {4, "assets/MAP/Dimension2/di2_tile4.png"}, {5, "assets/MAP/Dimension2/di2_tile5.png"},
            {6, "assets/MAP/Dimension2/di2_tile6.png"}, {7, "assets/MAP/Dimension2/di2_tile7.png"},
            {9, "assets/MAP/Dimension2/di2_tile9.png"}, {11, "assets/MAP/Dimension2/di2_tile11.png"},
            {16, "assets/MAP/Dimension2/di2_tile16.png"},{18, "assets/MAP/Dimension2/di2_tile18.png"},
            {19, "assets/MAP/Dimension2/di2_tile19.png"}, {48, "assets/MAP/Dimension2/di2_tile48.png"},
            {49, "assets/MAP/Dimension2/di2_tile49.png"}, {50, "assets/MAP/Dimension2/di2_tile50.png"} 
        };
    }

    for (auto& pair : tileTextures1) {
        SDL_DestroyTexture(pair.second);
    }
    tileTextures1.clear();

    for (auto& pair : tileTextures2) {
        SDL_DestroyTexture(pair.second);
    }
    tileTextures2.clear();

    // Load new textures
    for (auto& pair : tileTexturePaths) {
        SDL_Surface* surface = IMG_Load(pair.second.c_str());
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (dimensionID == 1) {
            tileTextures1[pair.first] = texture;
        } else {
            tileTextures2[pair.first] = texture;
        }
    }
}

#include "game.h"
#include "defs.h"

enum Direction {
    IDLE_RIGHT,
    IDLE_LEFT,
    RUNNING_RIGHT,
    RUNNING_LEFT
};

Direction characterState = IDLE_RIGHT;

Game::Game() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    window = SDL_CreateWindow("Moonlight", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    running = true;
    count = 0;

    background.setImage("assets/MAP/Clouds 3/1.png", renderer);
    
    player.setImage("assets/PLAYER/player.png",renderer);
    player.setDest(SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 100, 63,63);
    
    idle_right = player.createCycle(2,64,64,10,10);
    run_right = player.createCycle(1,64,64,16,10);
    idle_left = player.createCycle(6,64,64,10,10);
    run_left = player.createCycle(5,64,64,16,10);
    loadMap("assets/MAP/Ground.level");

    mapX = mapY = 0;
    scrollingSpeed = 1;
    loop();
}

Game::~Game(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
}

void Game::loop() {
    static int lastTime = 0;
    while(running){

        lastFrame = SDL_GetTicks();
        static int lastTime;
        if(lastFrame >= (lastTime+1000)){
            lastTime = lastFrame;
            frameCount = 0;
        }

        render();
        input();
        update();

    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw Background directly
    SDL_Rect bgRectDest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; 
    SDL_RenderCopy(renderer, background.getTex(), NULL, &bgRectDest);

    drawMap();  // Draws the level
    draw(player);

    SDL_RenderPresent(renderer);
}



void Game::draw(Object o){
    SDL_Rect dest = o.getDest();
    SDL_Rect src = o.getSource();
    if (!o.getTex()) {
        std::cerr << "Error: Object texture is NULL\n";
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
        // Handle key press (only change state once)
        if(event.type == SDL_KEYDOWN && event.key.repeat == 0) {
            if(event.key.keysym.sym == SDLK_RIGHT && characterState != RUNNING_RIGHT){
                left = 0;
                right = 1;
                player.setCurrentAnimation(run_right);
                characterState = RUNNING_RIGHT;
            }
            if(event.key.keysym.sym == SDLK_LEFT && characterState != RUNNING_LEFT){
                left = 1;
                right = 0;

                player.setCurrentAnimation(run_left);
                characterState = RUNNING_LEFT;
            }
        }
        // Handle key release (only change state once)
        if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_RIGHT && characterState == RUNNING_RIGHT){
                right = 0;
                player.setCurrentAnimation(idle_right);
                characterState = IDLE_RIGHT;
            }
            if(event.key.keysym.sym == SDLK_LEFT && characterState == RUNNING_LEFT){
                left = 0;
                player.setCurrentAnimation(idle_left);
                characterState = IDLE_LEFT;
            }
        }
    }
}


void Game::update(){
    //make the player decelarate in water:
    if(left){
        player.setDest(player.getDX() - (runningSpeedNormal), player.getDY());
    }
    if(right){
        player.setDest(player.getDX() + (runningSpeedNormal), player.getDY());
    }
    fall = 1, water = 0; 
    for(int i = 0; i < map.size(); i++){
        if(collision(player, map[i])){
            if(map[i].getSolid()){
                fall = 0;
            }
            else{
                water = 1;
            }
        }
    }
    if(fall){
        player.setDest(player.getDX(), player.getDY() + GRAVITY);
    }
    if(water && left){
        player.setDest(player.getDX() + (runningSpeedNormal)- (runningSpeedWater), player.getDY());
    }
    if(water && right){
        player.setDest(player.getDX() - (runningSpeedNormal) + (runningSpeedWater), player.getDY());
    }
    //Need to know the position of player:
    cout<<player.getDX()<<":" <<player.getDY()<<"\n";
    

    player.updateAnimation();
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
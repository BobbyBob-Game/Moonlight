#include "levelman.h"
#include "player.h"

// Constructor initializes levels and loads textures
LevelManager::LevelManager(SDL_Renderer* renderer) : currentLevel(0) {
    levels = {
        "assets/TileMap/chunk1.csv",
        "assets/TileMap/chunk2.csv",
        "assets/TileMap/chunk3_pre.csv", "assets/TileMap/chunk3.csv",
        "assets/TileMap/chunk4_ice.csv"
    };

    // Load textures for each tile type
    textures[6] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile6.png"); 
    textures[7] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile7.png"); 
    textures[8] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile8.png"); 
    textures[12] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile12.png"); 
    textures[13] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile13.png"); 
    textures[14] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile14.png"); 
    textures[18] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile18.png"); 
    textures[19] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile19.png"); 
    textures[20] = IMG_LoadTexture(renderer, "assets/TileMap/tileset/structTile20.png"); 

    for (const auto& [key, texture] : textures) {
        if (!texture) {
            std::cerr << "Failed to load texture for tile " << key << ": " << IMG_GetError() << "\n";
        }
    }

    checkpointTexture = IMG_LoadTexture(renderer, "assets/doge.png");
    if (!checkpointTexture) {
        std::cerr << "Failed to load checkpoint texture: " << IMG_GetError() << "\n";
    }
}

// Destructor to free textures
LevelManager::~LevelManager() {
    for (auto& [key, texture] : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
}

// Load current level from CSV
void LevelManager::LoadLevel() {
    if (currentLevel >= levels.size()) {
        std::cout << "Game Over! Restarting...\n";
        currentLevel = 0;
    }

    std::cout << "Loading: " << levels[currentLevel] << "...\n";
    std::cout << "The player is in: " << currentLevel << "...\n";

    std::ifstream file(levels[currentLevel]);
    if (!file.is_open()) {
        std::cerr << "Error loading level file: " << levels[currentLevel] << "\n";
        return;
    }

    levelData.clear();
    std::string line;

    while (std::getline(file, line)) {
        std::vector<Tile> row;
        std::stringstream ss(line);
        std::string tileValue;

        while (std::getline(ss, tileValue, ',')) {
            Tile tile;
            tile.type = std::stoi(tileValue);
            row.push_back(tile);
        }

        levelData.push_back(row);
    }

    file.close();
    loadCheckPoint(currentLevel);
}

// Move to the next level
void LevelManager::NextLevel() {
    if (currentLevel < levels.size() - 1) {
        currentLevel++;
        LoadLevel();
    }
}

// Get the level data grid
std::vector<std::vector<Tile>> LevelManager::GetLevelData() const {
    return levelData;
}

// Get the texture for a given tile type
SDL_Texture* LevelManager::GetTexture(int tileType) const {
    auto it = textures.find(tileType);
    return (it != textures.end()) ? it->second : nullptr;
}

bool LevelManager::isSpecialLevel(){
    return levels[currentLevel] == "assets/TileMap/chunk3.csv";
}

void LevelManager::fade(SDL_Renderer *renderer, int duration, bool fadeIn) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Enable alpha blending

    int startAlpha = fadeIn ? 255 : 0;
    int endAlpha = fadeIn ? 0 : 255;
    int step = (endAlpha - startAlpha) / (duration / 10); // Auto-adjust step size

    SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    for (int alpha = startAlpha; (fadeIn ? alpha >= endAlpha : alpha <= endAlpha); alpha += step) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &screen);
        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Small delay for smooth effect
    }
}

void LevelManager::loadCheckPoint(int levelNumber){
    checkpoints.clear();
    if(levelNumber == 1){
        checkpoints.push_back({SCREEN_WIDTH/2,SCREEN_HEIGHT - 64,false});
    }
    else if(levelNumber == 2){
        checkpoints.push_back({SCREEN_WIDTH/2,SCREEN_HEIGHT - 64,false});
    }
}

void LevelManager::updateCheckpoint(float deltaTime, Player& player){
    for(auto& checkpoint : checkpoints){
        if(checkpoint.activated == false && player.getX() > checkpoint.x - 10){
                checkpoint.activated = true;
                player.setCheckpoint(checkpoint.x, checkpoint.y);
                std::cout << "Checkpoint activated at: " << checkpoint.x << ", " << checkpoint.y << "\n";
        }
    }
}

void LevelManager::renderCheckpoint(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    
    for (auto& checkpoint : checkpoints) {
        SDL_Rect destRect = {checkpoint.x, checkpoint.y, 32, 32};
        SDL_RenderFillRect(renderer, &destRect);  // Draw the green rectangle

        if (checkpointTexture) {  // Check if the texture is valid
            SDL_RenderCopy(renderer, checkpointTexture, nullptr, &destRect);
        } else {
            std::cerr << "Checkpoint texture is NULL\n";
        }
    }
}

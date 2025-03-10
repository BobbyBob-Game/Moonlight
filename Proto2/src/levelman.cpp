#include "levelman.h"

// Constructor initializes levels and loads textures
LevelManager::LevelManager(SDL_Renderer* renderer) : currentLevel(0) {
    levels = {
        "assets/TileMap/chunk1.csv",
        "assets/TileMap/chunk2.csv",
        "assets/TileMap/chunk3_pre.csv", "assets/TileMap/chunk3.csv",
        "assets/TileMap/chunk4.csv"
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
}

// Move to the next level
void LevelManager::NextLevel() {
    if (currentLevel < levels.size() - 1) {
        currentLevel++;
        LoadLevel();
    } else {
        std::cout << "Game Completed!\n";
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

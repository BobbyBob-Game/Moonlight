#include "levelman.h"
#include "player.h"

LevelManager::LevelManager(SDL_Renderer* renderer, SDL_Window* window) : currentLevel(0) {
    this->renderer = renderer; //same name so use this
    this->window = window;
    levels = {
        {"assets/TileMap/tutorial.csv", "assets/TileMap/tileset/Village_prop/Ground"},
        {"assets/TileMap/level1.csv", "assets/TileMap/tileset/Level1"},
        {"assets/TileMap/level2.csv", "assets/TileMap/tileset/Level2"},
        {"assets/TileMap/level3_pre.csv", "assets/TileMap/tileset/Level3"}, {"assets/TileMap/level3.csv", "assets/TileMap/tileset/Level3"},
        {"assets/TileMap/ending_layer1.csv", "assets/TileMap/tileset/Village_prop/Ground"}
    };

}

LevelManager::~LevelManager() {
    for (auto& [key, texture] : textures) {
        if (texture) SDL_DestroyTexture(texture);
    }
    textures.clear();
}

void LevelManager::LoadLevel() {
    levelData.clear();
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();

    const LevelInfo& levelInfo = levels[currentLevel];

    if (levelInfo.layoutFile == "assets/TileMap/level3.csv") {
        SDL_SetWindowSize(window, PORTRAIT_WIDTH, PORTRAIT_HEIGHT);
        SDL_RenderSetLogicalSize(renderer, LANDSCAPE_HEIGHT, LANDSCAPE_WIDTH); 
    } else {
        SDL_SetWindowSize(window, LANDSCAPE_WIDTH, LANDSCAPE_HEIGHT);
        SDL_RenderSetLogicalSize(renderer, LANDSCAPE_WIDTH, LANDSCAPE_HEIGHT);
    }    

    std::ifstream file(levelInfo.layoutFile);
    if (!file.is_open()) {
        std::cerr << "Error loading level file: " << levelInfo.layoutFile << "\n";
        return;
    }

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

    std::set<int> uniqueTileTypes;
    for (const auto& row : levelData) {
        for (const auto& tile : row) {
            uniqueTileTypes.insert(tile.type);
        }
    }

    for (int tileType : uniqueTileTypes) {
        if(tileType == -1) continue;
        std::string path = levelInfo.tilesetFolder + "/tile" + std::to_string(tileType) + ".png";
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (!tex) {
            std::cerr << "Failed to load tile texture: " << path << " - " << IMG_GetError() << "\n";
        } else {
            textures[tileType] = tex;
        }
    }

}

void LevelManager::NextLevel() {
    if (currentLevel < levels.size() - 1) {
        currentLevel++;
        LoadLevel();
    }
}

std::vector<std::vector<Tile>> LevelManager::GetLevelData() const {
    return levelData;
}

SDL_Texture* LevelManager::GetTexture(int tileType) const {
    auto it = textures.find(tileType);
    return (it != textures.end()) ? it->second : nullptr;
}

bool LevelManager::isSpecialLevel(){
    return levels[currentLevel].layoutFile == "assets/TileMap/level3.csv";
}

bool LevelManager::isSolid(int tileType, int levelNumber){
    if(levelNumber == 0 || levelNumber == 5){
        return (tileType == 0 || tileType == 1 || tileType == 2 || tileType == 224 || tileType == 225 || tileType == 226);
    }
    else if(levelNumber == 1){
        return (tileType == 0 || tileType == 1 || tileType == 2 || tileType == 6 || tileType == 7 || tileType == 8 || tileType == 12 || tileType == 14 || tileType == 18 || tileType == 19 ||tileType == 20);
    }
    else if(levelNumber == 2){
        return (tileType == 6 || tileType == 7 || tileType == 8);
    }
    else{
        return (tileType == 0 || tileType == 1 || tileType == 2 || tileType == 3);
    }
}

std::string LevelManager::getLevelCSV(){
    return levels[currentLevel].layoutFile;
}

void LevelManager::renderText(const std::string& text, TTF_Font* font, SDL_Rect position, SDL_Color color) {
    if (text.empty() || !font || !renderer) return;

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    int textW = textSurface->w;
    int textH = textSurface->h;
    SDL_FreeSurface(textSurface);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); 
    SDL_RenderFillRect(renderer, &position);

    SDL_Rect textRect = {
        position.x + (position.w - textW) / 2,
        position.y + (position.h - textH) / 2,
        textW,
        textH
    };

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}

void LevelManager::reset(){
    currentLevel = 0;
    LoadLevel();
}

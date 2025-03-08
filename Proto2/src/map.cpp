#include "map.h"

Map::Map(SDL_Renderer* renderer) : renderer(renderer){

}

void Map::loadMap(const char* filename, int sizeX, int sizeY){
    std::fstream mapFile(filename);
    if (!mapFile.is_open()) {
        std::cerr << "Error: Failed to open map file: " << filename << std::endl;
        return;
    }
    std::vector<std::vector<int>> newMap(sizeY, std::vector<int>(sizeX, 0));

    std::string line;
    for (int y = 0; y < sizeY; y++) {
        if (!getline(mapFile, line)) {
            std::cerr << "Error: Unexpected end of file at line " << y << std::endl;
            break;
        }

        std::stringstream ss(line);
        for (int x = 0; x < sizeX; x++) {
            int tileID = 0;
            if (ss >> tileID) {
                newMap[y][x] = tileID;
            }
            if (ss.peek() == ',') ss.ignore();
        }
    }
    tileMap = newMap;
    mapFile.close();
}

void Map::drawMap(){
    if (!renderer) {
        std::cerr << "Error: Renderer is not initialized!\n";
        return;
    }
    
    if(tileMap.empty()) return;
    for(int y = 0; y < tileMap.size(); y++){
        for(int x = 0; x < tileMap[y].size(); x++){
            int tileID = tileMap[y][x];
            if(tileTexture.find(tileID) != tileTexture.end()){
                SDL_Texture* texture = tileTexture[tileID];
                if(texture){
                    SDL_Rect destRect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(renderer, texture, NULL, &destRect);
                }
            }
        }
    }
}

void Map::loadTileTexture(){
    std::unordered_map<int, std::string> tileTexturePaths;
    tileTexturePaths = {
        {6, "assets/TileMap/tileset/structTile6.png"},
        {7, "assets/TileMap/tileset/structTile7.png"},
        {8, "assets/TileMap/tileset/structTile8.png"},
        {12, "assets/TileMap/tileset/structTile12.png"},
        {13, "assets/TileMap/tileset/structTile13.png"},
        {14, "assets/TileMap/tileset/structTile14.png"},
        {18, "assets/TileMap/tileset/structTile18.png"},
        {19, "assets/TileMap/tileset/structTile19.png"},
        {20, "assets/TileMap/tileset/structTile20.png"}
    };
    for(const auto& pair : tileTexture){
        SDL_DestroyTexture(pair.second);
    }
    tileTexture.clear();

    for(auto& pair : tileTexturePaths){
        SDL_Surface* surface = IMG_Load(pair.second.c_str());
        if (!surface) {
            std::cerr << "Error: Failed to load surface for " << pair.second << '\n';
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            std::cerr << "Error: Failed to create texture from " << pair.second << '\n';
            continue;
        }
        tileTexture[pair.first] =  texture;
    }
}
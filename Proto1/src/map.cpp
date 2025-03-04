#include "map.h"

Map::Map(int dimensionID, SDL_Renderer* renderer)
    : dimensionID(dimensionID), renderer(renderer) {
}

Map::~Map() {
    for (auto& pair : tileTextures1) {
        SDL_DestroyTexture(pair.second);
    }
    for (auto& pair : tileTextures2) {
        SDL_DestroyTexture(pair.second);
    }
}

void Map::loadMap(const char* filename, int sizeX, int sizeY) {
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

    if (dimensionID == 1) {
        tileMap1 = newMap;
    } else {
        tileMap2 = newMap;
    }

    mapFile.close();
}

void Map::drawMap() {
    if (dimensionID == 1) {
        if (tileMap1.empty()) return;
        for (int y = 0; y < tileMap1.size(); y++) {
            for (int x = 0; x < tileMap1[y].size(); x++) {
                int tileID = tileMap1[y][x];
                if (tileTextures1.find(tileID) != tileTextures1.end()) {
                    SDL_Texture* texture = tileTextures1[tileID];
                    if (texture) {
                        SDL_Rect destRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                    }
                }
            }
        }
    } else {
        if (tileMap2.empty()) return;
        for (int y = 0; y < tileMap2.size(); y++) {
            for (int x = 0; x < tileMap2[y].size(); x++) {
                int tileID = tileMap2[y][x];
                if (tileTextures2.find(tileID) != tileTextures2.end()) {
                    SDL_Texture* texture = tileTextures2[tileID];
                    if (texture) {
                        SDL_Rect destRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                        SDL_RenderCopy(renderer, texture, NULL, &destRect);
                    }
                }
            }
        }
    }
}

void Map::loadTileTexture() {
    std::unordered_map<int, std::string> tileTexturePaths;
    if (dimensionID == 1) {
        tileTexturePaths = {
            {1, "assets/MAP/Dimension1/tile1.png"},
            {3, "assets/MAP/Dimension1/tile3.png"},
            {5, "assets/MAP/Dimension1/tile5.png"},
            {6, "assets/MAP/Dimension1/tile6.png"},
            {7, "assets/MAP/Dimension1/tile7.png"},
            {8, "assets/MAP/Dimension1/tile8.png"},
            {21, "assets/MAP/Dimension1/tile21.png"},
            {22, "assets/MAP/Dimension1/tile22.png"},
            {24, "assets/MAP/Dimension1/tile24.png"},
            {42, "assets/MAP/Dimension1/tile42.png"},
            {63, "assets/MAP/Dimension1/tile63.png"},
            {64, "assets/MAP/Dimension1/tile64.png"},
            {65, "assets/MAP/Dimension1/tile65.png"},
            {66, "assets/MAP/Dimension1/tile66.png"}
        };
    } else {
        tileTexturePaths = {
            {1, "assets/MAP/Dimension2/di2_tile1.png"},
            {3, "assets/MAP/Dimension2/di2_tile3.png"},
            {4, "assets/MAP/Dimension2/di2_tile4.png"},
            {5, "assets/MAP/Dimension2/di2_tile5.png"},
            {6, "assets/MAP/Dimension2/di2_tile6.png"},
            {7, "assets/MAP/Dimension2/di2_tile7.png"},
            {9, "assets/MAP/Dimension2/di2_tile9.png"},
            {11, "assets/MAP/Dimension2/di2_tile11.png"},
            {16, "assets/MAP/Dimension2/di2_tile16.png"},
            {18, "assets/MAP/Dimension2/di2_tile18.png"},
            {19, "assets/MAP/Dimension2/di2_tile19.png"},
            {48, "assets/MAP/Dimension2/di2_tile48.png"},
            {49, "assets/MAP/Dimension2/di2_tile49.png"},
            {50, "assets/MAP/Dimension2/di2_tile50.png"}
        };
    }

    // Clear any existing textures
    for (auto& pair : tileTextures1) {
        SDL_DestroyTexture(pair.second);
    }
    tileTextures1.clear();
    for (auto& pair : tileTextures2) {
        SDL_DestroyTexture(pair.second);
    }
    tileTextures2.clear();

    // Load new textures from file paths
    for (auto& pair : tileTexturePaths) {
        SDL_Surface* surface = IMG_Load(pair.second.c_str());
        if (!surface) {
            std::cerr << "Error: Failed to load surface for " << pair.second << std::endl;
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "Error: Failed to create texture from " << pair.second << std::endl;
            continue;
        }

        if (dimensionID == 1) {
            tileTextures1[pair.first] = texture;
        } else {
            tileTextures2[pair.first] = texture;
        }
    }
}

bool Map::isSolidTile(int tileID, int dimensionID) {
    if (dimensionID == 1) {
        return (tileID == 63 || tileID == 64 || tileID == 66 ||
                tileID == 6  || tileID == 7  || tileID == 3  ||
                tileID == 8  || tileID == 1  || tileID == 5);
    } else if (dimensionID == 2) {
        return (tileID == 0 || tileID == 1 || tileID == 3 || tileID == 4 ||
                tileID == 5 || tileID == 6 || tileID == 7);
    }
    return false; 
}

std::vector<std::vector<int>>& Map::getCurrentTileMap() {
    return (dimensionID == 1) ? tileMap1 : tileMap2;
}

void Map::tileCollision(Object &obj, float &velX, float &velY, bool &onGround) {
    std::vector<std::vector<int>>& current = getCurrentTileMap();

    // Bottom collision: tile beneath the object.
    int tileX = static_cast<int>(obj.getDX() / TILE_SIZE);
    int tileY = static_cast<int>((obj.getDY() + obj.getDH()) / TILE_SIZE);
    if (tileY >= 0 && tileY < current.size() &&
        tileX >= 0 && tileX < current[tileY].size()) {
        if (isSolidTile(current[tileY][tileX], dimensionID)) {
            obj.setDest(obj.getDX(), tileY * TILE_SIZE - obj.getDH());
            velY = 0;
            onGround = true;
        }
    }
    
    // Top collision: check when moving upward.
    if (velY < 0) {
        int topTile  = obj.getDY() / TILE_SIZE;
        int leftTile = obj.getDX() / TILE_SIZE;
        int rightTile = (obj.getDX() + obj.getDW() - 1) / TILE_SIZE;
        if (topTile >= 0 && topTile < current.size() &&
            leftTile >= 0 && leftTile < current[topTile].size() &&
            rightTile >= 0 && rightTile < current[topTile].size()) {
            if (isSolidTile(current[topTile][leftTile], dimensionID) ||
                isSolidTile(current[topTile][rightTile], dimensionID)) {
                obj.setDest(obj.getDX(), (topTile + 1) * TILE_SIZE);
                velY = 0;
            }
        }
    }
    
    // Left collision: check when moving left.
    if (velX < 0) {
        int leftTile = obj.getDX() / TILE_SIZE;
        int topTile = obj.getDY() / TILE_SIZE;
        int bottomTile = (obj.getDY() + obj.getDH() - 1) / TILE_SIZE;
        if (leftTile >= 0 &&
            topTile >= 0 && topTile < current.size() &&
            bottomTile >= 0 && bottomTile < current.size() &&
            leftTile < current[topTile].size() && leftTile < current[bottomTile].size()) {
            if (isSolidTile(current[topTile][leftTile], dimensionID) ||
                isSolidTile(current[bottomTile][leftTile], dimensionID)) {
                obj.setDest((leftTile + 1) * TILE_SIZE, obj.getDY());
                velX = 0;
            }
        }
    }
    
    // Right collision: check when moving right.
    if (velX > 0) {
        int rightTile = (obj.getDX() + obj.getDW() - 1) / TILE_SIZE;
        int topTile = obj.getDY() / TILE_SIZE;
        int bottomTile = (obj.getDY() + obj.getDH() - 1) / TILE_SIZE;
        if (!current.empty() && !current[0].empty() &&
            rightTile >= 0 && rightTile < current[0].size() &&
            topTile >= 0 && topTile < current.size() &&
            bottomTile >= 0 && bottomTile < current.size() &&
            rightTile < current[topTile].size() && rightTile < current[bottomTile].size()) {
            if (isSolidTile(current[topTile][rightTile], dimensionID) ||
                isSolidTile(current[bottomTile][rightTile], dimensionID)) {
                obj.setDest(rightTile * TILE_SIZE - obj.getDW(), obj.getDY());
                velX = 0;
            }
        }
    }
}

void Map::clampObjectPosition(Object &obj, float &velX, float &velY, bool &onGround) {
    if (obj.getDX() < 0) {
        obj.setDest(0, obj.getDY());
        velX = 0;
    } else if (obj.getDX() + obj.getDW() > SCREEN_WIDTH) {
        obj.setDest(SCREEN_WIDTH - obj.getDW(), obj.getDY());
        velX = 0;
    }

    if (obj.getDY() < 0) {
        obj.setDest(obj.getDX(), 0);
        velY = 0;
    } else if (obj.getDY() + obj.getDH() > SCREEN_HEIGHT) {
        obj.setDest(obj.getDX(), SCREEN_HEIGHT - obj.getDH());
        velY = 0;
        onGround = true;
    }
}
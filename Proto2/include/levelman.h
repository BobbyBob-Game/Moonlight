#ifndef LEVELMAN_H
#define LEVELMAN_H
#include "defs.h"

struct Tile {
    int type;    // Tile type from CSV (0 = empty, 1 = wall, 2 = player, 3 = exit)
};

class LevelManager {
private:
    int currentLevel;
    std::vector<std::string> levels;
    std::vector<std::vector<Tile>> levelData;  // 2D grid storing tile types
    std::unordered_map<int, SDL_Texture*> textures;  // Map tile types to textures

public:
    LevelManager(SDL_Renderer* renderer);
    ~LevelManager();

    void LoadLevel();
    void NextLevel();
    std::vector<std::vector<Tile>> GetLevelData() const;
    SDL_Texture* GetTexture(int tileType) const;
};

#endif // LEVELMAN_H

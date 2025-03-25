#ifndef LEVELMAN_H
#define LEVELMAN_H
#include "defs.h"

struct Tile {
    int type;    
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
    bool isSpecialLevel();
    std::vector<std::vector<Tile>> GetLevelData() const;
    SDL_Texture* GetTexture(int tileType) const;
};

#endif // LEVELMAN_H

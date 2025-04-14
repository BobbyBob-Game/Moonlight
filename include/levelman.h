#ifndef LEVELMAN_H
#define LEVELMAN_H
#include "defs.h"

class Player;

struct Tile {
    int type;    
};

struct LevelInfo {
    std::string layoutFile;
    std::string tilesetFolder;
};

class LevelManager {
private:
    SDL_Renderer* renderer;
    SDL_Window* window; 
    int currentLevel;
    std::vector<LevelInfo> levels;
    std::vector<std::vector<Tile>> levelData;  
    std::unordered_map<int, SDL_Texture*> textures; 
    
public:
    LevelManager(SDL_Renderer* renderer, SDL_Window* window);
    ~LevelManager();

    void LoadLevel();
    void NextLevel();
    void reset();
    bool isSpecialLevel();
    int getCurrentLevel() {return currentLevel;}

    static bool isSolid(int tileType, int levelNumber);

    void renderText(const std::string& text, TTF_Font* font, SDL_Rect position, SDL_Color color);

    std::vector<std::vector<Tile>> GetLevelData() const;
    std::string getLevelCSV();
    SDL_Texture* GetTexture(int tileType) const;
};

#endif 

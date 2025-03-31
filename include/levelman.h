#ifndef LEVELMAN_H
#define LEVELMAN_H
#include "defs.h"

class Player;

struct Tile {
    int type;    
};

struct Checkpoint {
    int x, y; 
    bool activated = false;
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
    void fade(SDL_Renderer* renderer, int duration, bool fadeIn);
    int getCurrentLevel() {return currentLevel;}

    std::vector<Checkpoint> checkpoints;
    void loadCheckPoint(int levelNumber);
    void renderCheckpoint(SDL_Renderer* renderer);
    void updateCheckpoint(float deltaTime, Player& player);

    std::vector<Checkpoint>& getCheckPoints() {return checkpoints;}
    std::vector<std::vector<Tile>> GetLevelData() const;
    SDL_Texture* GetTexture(int tileType) const;

    SDL_Texture* checkpointTexture = nullptr;
    int checkpointFrame = 0;
    float checkpointAnimTimer = 0.0f;
    float checkpointFrameDelay = 0.2f;
};

#endif // LEVELMAN_H

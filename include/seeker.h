#ifndef SEEKER_H
#define SEEKER_H
#include "defs.h"
#include "levelman.h"

struct Node {
    int x, y;
    int gCost, hCost;
    Node* parent;

    Node(int x, int y, int g, int h, Node* parent = nullptr) : x(x), y(y), gCost(g), hCost(h), parent(parent) {}
    int fCost() const {return gCost + hCost;}
    bool operator>(const Node& other) const {
        return fCost() > other.fCost();
    }
};

struct Coins {
    float x,y; 
    float animTimer = 0.0f;
    int frame = 0;
    bool collected = false;
};

class Seeker {
    private:
    SDL_Renderer* renderer;
    LevelManager* levelMan;
    SDL_Texture* spriteSheet = nullptr;
    Mix_Chunk* coinSound = nullptr;
    int frame = 0;
    int maxFrames = 15;
    int frameWidth = 32;
    int frameHeight = 32;
    float animationSpeed = 0.10f;
    float animationTimer = 0.0f;

    std::deque<std::pair<float,float>> trail;
    int maxTrail = 50;
    std::vector<Coins> coins;
    SDL_Texture* coinTexture = nullptr;
    int coinFrameWidth = 32, coinFrameHeight = 32;
    int coinMaxFrames = 11;
    float coinAnimSpeed = 0.3f;
    Mix_Chunk* seekerSound = nullptr;
    Uint32 lastSeekerSoundTime = 0;
    
    int score = 0;

    int heuristic(int x1, int y1, int x2, int y2) {
        return abs(x2 - x1) + abs(y2 - y1);
    }
    
    bool isValid(int x, int y, int rows, int cols, const std::vector<std::vector<int>>& grid, int level) {
        if (x < 0 || y < 0 || x >= cols || y >= rows)
            return false;
    
        int tileType = grid[y][x];
        return !levelMan->isSolid(tileType, level);
    }
    
    bool canMoveTo(float currentX, float currentY, float targetX, float targetY, const std::vector<std::vector<int>>& grid);
    
    public:
    Seeker(SDL_Renderer* renderer, LevelManager* levelMan);
    ~Seeker();
    float x, y;
    float speed = 350.0f;
    std::vector<std::pair<int, int>> path;
    int pathIndex = 0;
    int tick = 0;
    bool seekerAndPlayer;

    void updateLevel(LevelManager& levelMan);
    void render(SDL_Renderer* renderer, SDL_Rect playerCollisionBox, int levelNum);
    void update(float deltaTime, const std::vector<std::vector<int>>& grid, SDL_Rect playerCollision, int levelNum);
    std::vector<std::pair<int,int>> aStarSearch(const std::vector<std::vector<int>>& grid, int sx, int sy, int gx, int gy, int levelNum);
    std::vector<std::vector<int>> loadCSV(const std::string& filename);
    void loadTexture(const std::string& path);
    bool seekerCollidePlayer(SDL_Rect player);
    void respawn();
    std::string getScore();

    void loadCoinTexture(const std::string& path);
    void spawnCoin(int count);
};

#endif
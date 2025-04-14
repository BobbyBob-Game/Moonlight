#include "seeker.h"

Seeker::Seeker(SDL_Renderer* renderer, LevelManager* levelMan) {
    this->renderer = renderer; //learning what the pointer (this) is ***
    this->levelMan = levelMan;
    x = 100;
    y = 100;
    coinSound = Mix_LoadWAV("assets/sound/coin.wav");
    seekerSound = Mix_LoadWAV("assets/sound/seekerMoving.wav");
}

void Seeker::loadTexture(const std::string& path) {
    SDL_Surface* temp = IMG_Load(path.c_str());
    if (!temp) {
        std::cerr << "Failed to load sprite sheet: " << IMG_GetError() << "\n";
        return;
    }
    spriteSheet = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
}

std::vector<std::pair<int,int>> Seeker::aStarSearch(const std::vector<std::vector<int>>& grid, int sx, int sy, int gx, int gy, int levelNum){
    int rows = grid.size(), cols = grid[0].size();
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::vector<std::vector<char>> visited(rows, std::vector<char>(cols, 0));

    openList.emplace(sx, sy, 0, heuristic(sx, sy, gx, gy));

    while(!openList.empty()){
        Node current = openList.top();
        openList.pop();

        if (current.x < 0 || current.x >= cols || current.y < 0 || current.y >= rows) {
            std::cerr << "Out-of-bounds current node: (" << current.x << "," << current.y << ")\n";
            continue;
        }

        if(visited[current.y][current.x]) continue;
        visited[current.y][current.x] = 1;

        /*
        emplace_back: get directly
        push_back: make a copy 
        */

        if(current.x == gx && current.y == gy){
            std::vector<std::pair<int,int>> path;
            for(Node* node = new Node(current); node != nullptr; node = node->parent){
                path.emplace_back(node->x, node->y);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        const int dx[] = {-1,1,0,0};
        const int dy[] = {0,0,-1,1};

        for(int dir = 0; dir < 4; ++dir){
            int nx = current.x + dx[dir];
            int ny = current.y + dy[dir];

            if(isValid(nx, ny, rows, cols, grid, levelNum) && !visited[ny][nx]){
                int g = current.gCost + 1;
                int h = heuristic(nx, ny, gx, gy);
                openList.emplace(nx, ny, g, h, new Node(current));
            }
        }
    }
    return {}; 
}

std::vector<std::vector<int>> Seeker::loadCSV(const std::string& filename) {
    std::vector<std::vector<int>> grid;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<int> row;
        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        grid.push_back(row);
    }
    return grid;
}

bool Seeker::canMoveTo(float currentX, float currentY, float targetX, float targetY, const std::vector<std::vector<int>>& grid) {
    int targetGridX = static_cast<int>(targetX / TILE_SIZE);
    int targetGridY = static_cast<int>(targetY / TILE_SIZE);

    if (grid[targetGridY][targetGridX] != -1) {
        return false; 
    }

    return true; 
}


void Seeker::update(float deltaTime, const std::vector<std::vector<int>>& grid, SDL_Rect playerRect, int levelNum) {
    if(levelNum == 0) return;
    tick++;

    int sx = static_cast<int>(x + TILE_SIZE / 2) / TILE_SIZE;
    int sy = static_cast<int>(y + TILE_SIZE / 2) / TILE_SIZE;
    int gx = playerRect.x / TILE_SIZE;
    int gy = playerRect.y / TILE_SIZE;

    if ((tick % 13 == 0 || path.empty())) {
        path = aStarSearch(grid, sx, sy, gx, gy, levelNum);
        
        if (path.size() > 1)
            pathIndex = 1;
        else
            pathIndex = 0;
    }
    
    if (pathIndex < path.size()) {
        float targetX = path[pathIndex].first * TILE_SIZE + TILE_SIZE / 2.0f;
        float targetY = path[pathIndex].second * TILE_SIZE + TILE_SIZE / 2.0f;

        float centerX = x + frameWidth / 2.0f;
        float centerY = y + frameHeight / 2.0f;

        float dx = targetX - centerX;
        float dy = targetY - centerY;
        float dist = std::sqrt(dx * dx + dy * dy);
        float step = speed * deltaTime;

        if (dist > step) {
            x += step * dx / dist;
            y += step * dy / dist;
        } else {
            x = targetX - frameWidth / 2.0f;
            y = targetY - frameHeight / 2.0f;
            pathIndex++;
        }

        trail.push_back({x,y}); //store old position of seeker
        if(trail.size() > maxTrail){
            trail.pop_front();
        }

        int space = 8; //pixels

        for (int i = 0; i < coins.size(); ++i) {
            int index = std::max(0, (int)trail.size() - 1 - i * space); //get the latest and make it the furthest and keep adding space between
            if (index < trail.size()) {
                float tx = trail[index].first;
                float ty = trail[index].second;

                coins[i].x += (tx - coins[i].x) * 5.0f * deltaTime;
                coins[i].y += (ty - coins[i].y) * 5.0f * deltaTime; 
            }

            coins[i].animTimer += deltaTime;
            if (coins[i].animTimer >= coinAnimSpeed) {
                coins[i].animTimer = 0;
                coins[i].frame = (coins[i].frame + 1) % coinMaxFrames;
            }
        }

        SDL_Rect playerBoxRect = playerRect;
        for (auto& coin : coins) {
            if (coin.collected) continue;

            SDL_Rect coinRect = {
                static_cast<int>(coin.x),
                static_cast<int>(coin.y),
                coinFrameWidth,
                coinFrameHeight
            };

            if (SDL_HasIntersection(&playerBoxRect, &coinRect)) {
                coin.collected = true;
                score++;
                Mix_PlayChannel(-1, coinSound, 0);
            }
        }
    }

    animationTimer += deltaTime;
    if (animationTimer >= animationSpeed) {
        animationTimer = 0;
        frame = (frame + 1) % maxFrames;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastSeekerSoundTime >= 250) {
        Mix_PlayChannel(-1, seekerSound, 0);
        lastSeekerSoundTime = currentTime;
    }
}


void Seeker::render(SDL_Renderer* renderer, SDL_Rect playerBox,int levelNum) {
    if(levelNum == 0) return;
    // Draw path
    if (path.size() > 1) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 139, 120); 

        const int rectSize = 10;  

        for (const auto& [px, py] : path) {
            int centerX = px * TILE_SIZE + TILE_SIZE / 2;
            int centerY = py * TILE_SIZE + TILE_SIZE / 2;

            SDL_Rect marker = {
                centerX - rectSize / 2,
                centerY - rectSize / 2,
                rectSize,
                rectSize
            };

            SDL_RenderFillRect(renderer, &marker);
        }
    }

    for (const auto& coin : coins) {
        if (coin.collected || !coinTexture) continue;
    
        SDL_Rect src = { coin.frame * coinFrameWidth, 0, coinFrameWidth, coinFrameHeight };
        SDL_Rect dst = { static_cast<int>(coin.x), static_cast<int>(coin.y), coinFrameWidth, coinFrameHeight };
        SDL_RenderCopy(renderer, coinTexture, &src, &dst);
    }    

    if (spriteSheet) {
        SDL_Rect src = { frame * frameWidth, 0, frameWidth, frameHeight };
        int seekerSize = 48;
        SDL_Rect dst = { static_cast<int>(x), static_cast<int>(y), seekerSize, seekerSize };

        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (playerBox.x < x) {
            flip = SDL_FLIP_HORIZONTAL; 
        }

        SDL_RenderCopyEx(renderer, spriteSheet, &src, &dst, 0.0, nullptr, flip);
    }
}

void Seeker::loadCoinTexture(const std::string& path) {
    SDL_Surface* temp = IMG_Load(path.c_str());
    if (!temp) {
        std::cerr << "Failed to load coin sprite: " << IMG_GetError() << "\n";
        return;
    }
    coinTexture = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
}

void Seeker::spawnCoin(int count) {
    coins.clear();
    for (int i = 0; i < count; ++i) {
        Coins c;
        c.x = x;
        c.y = y;
        c.frame = rand() % coinMaxFrames;
        coins.push_back(c);
    }
}

void Seeker::respawn(){
    x = 50;
    y = 50;
}

std::string Seeker::getScore(){
    return std::to_string(score);
}

Seeker::~Seeker(){
    if (coinSound) Mix_FreeChunk(coinSound);
    if (seekerSound) Mix_FreeChunk(seekerSound);
}

bool Seeker::seekerCollidePlayer(SDL_Rect playerRect){
    SDL_Rect seekerRect = {
        static_cast<int>(x),
        static_cast<int>(y),
        48,
        48
    };

    return SDL_HasIntersection(&seekerRect, &playerRect);
}
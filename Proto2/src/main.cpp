#include "game.h"

int main(int argc, char* args[]) {
    Game game;
    if (!game.init()) {
        return -1;
    }
    if (!game.loadMedia()) {
        return -1;
    }
    
    game.run();
    game.close();
    return 0;
}

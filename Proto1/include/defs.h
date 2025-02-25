#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define TILE_SIZE 64

#define MAX_NAME_LENGTH 100
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_END
};
extern GameState gameState; //or use pragma once, prevent multiple definition

#endif
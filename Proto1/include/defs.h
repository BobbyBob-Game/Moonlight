#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define SCREEN_WIDTH 960 //40x30 tiles of 24x24
#define SCREEN_HEIGHT 720

#define TILE_SIZE 24

#define MAX_NAME_LENGTH 100
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_END
};
extern GameState gameState; //or use pragma once, prevent multiple definition

#endif
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

const int SUBPIXELS_PER_PIXEL = 8;
const float ACCELERATION = 0.05f * SUBPIXELS_PER_PIXEL; // 12 subpixels
const float DECELERATION = 0.5f * SUBPIXELS_PER_PIXEL;      // 128 subpixels
const float MAX_SPEED = 1.25f * SUBPIXELS_PER_PIXEL;         // 96 subpixels
const float FRICTION = 0.15f * SUBPIXELS_PER_PIXEL;     // 12 subpixels
const float GRAVITY = 0.5f*SUBPIXELS_PER_PIXEL;

const float JUMP_FORCE   = 15.0f;    // Increase this for a strong jump
const float GRAVITY_UP   = 30.0f;   // Gravity while ascending (lower for a slight float)
const float GRAVITY_DOWN = 50.0f;

extern GameState gameState; //or use pragma once, prevent multiple definition

#endif
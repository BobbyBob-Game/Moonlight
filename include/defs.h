#pragma once
#ifndef DEFS_H
#define DEFS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <queue>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <set>
#include <deque>

#define PORTRAIT_WIDTH 540
#define PORTRAIT_HEIGHT 960
#define LANDSCAPE_WIDTH 1280
#define LANDSCAPE_HEIGHT 720

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 710;
static const int TILE_SIZE = 32; //25x18
static const int PLAYER_WIDTH = 112;
static const int PLAYER_HEIGHT = 80;
static const int jumpStrength = -11;
static const int wallJumpStrengthX = 10; //horizontal boost
static const int wallJumpStrengthY = -12;
static const float maxFallSpeed = 8.0f;
static const int TOTAL_LEVELS = 5;

const int DIALOGUE_TRIGGER = 279;
const int DIALOGUE_TRIGGER_ANOTHER = 278;
const int SUBPIXELS_PER_PIXEL = 8;
const float MAX_SPEED = 0.35f * SUBPIXELS_PER_PIXEL;         // 96 subpixels
const float GRAVITY = 15.0f;
const float INITIAL_JUMP_VEL = -12.0f;
const float MAX_JUMP_HOLD_TIME = 0.25f;
const float JUMP_HOLD_GRAVITY = 0.3f;
const float FALL_GRAVITY = 0.7f;

const float GRAVITY_UP   = 20.0f;   // Gravity while ascending (lower for a slight float)
const float GRAVITY_DOWN = 80.0f;

enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_END,
    STATE_SETTINGS, 
    STATE_DEAD,
    STATE_PAUSE, 
    STATE_WIN 
};

enum MusicType {
    NOEH,
    THEME,
    GAME_MUSIC
};

extern GameState gameState;
extern GameState prevState;
extern MusicType currentMusic;

#endif
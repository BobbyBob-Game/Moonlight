#pragma once
#ifndef DEFS_H
#define DEFS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>


static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;
static const int TILE_SIZE = 32; //25x18
static const int PLAYER_WIDTH = 112;
static const int PLAYER_HEIGHT = 80;
static const int jumpStrength = -13;
static const int wallJumpStrengthX = 10; //horizontal boost
static const int wallJumpStrengthY = -12;
static const float maxFallSpeed = 6.0f;
static const int TOTAL_LEVELS = 5;

const int SUBPIXELS_PER_PIXEL = 8;
const float ACCELERATION = 0.05f * SUBPIXELS_PER_PIXEL; // 12 subpixels
const float DECELERATION = 0.9f * SUBPIXELS_PER_PIXEL;      // 128 subpixels
const float MAX_SPEED = 0.5f * SUBPIXELS_PER_PIXEL;         // 96 subpixels
const float FRICTION = 0.3f * SUBPIXELS_PER_PIXEL;     // 12 subpixels
const float GRAVITY = 0.5f*SUBPIXELS_PER_PIXEL;

const float GRAVITY_UP   = 30.0f;   // Gravity while ascending (lower for a slight float)
const float GRAVITY_DOWN = 50.0f;


#endif
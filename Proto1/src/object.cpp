#include "object.h"
#include <iostream>
using namespace std;

void Object::setDest(int x, int y, int w, int h){
    dest.x = x;
    dest.y = y;
    dest.w = w;
    dest.h = h;
}

void Object::setDest(int x, int y) {
    dest.x = x;
    dest.y = y;
}

void Object::setSource(int x, int y, int w, int h){
    src.x = x;
    src.y = y;
    src.w = w;
    src.h = h;
}
void Object::setImage(const std::string& path, SDL_Renderer* renderer) {

    tex = IMG_LoadTexture(renderer,path.c_str());

    if (!tex) {
        cerr << "Failed to create texture from " << path << ": " << SDL_GetError() << "\n";
    } else {
        cout << "Successfully loaded texture: " << path << "\n";
    }
}

#include "animate.h"
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "resourceman.h"

/*Animate::Animate(SDL_Renderer* renderer, std::string filepath){
    SDL_Surface* retrieveSurface = ResourceManagement::GetInstance().GetSurface(filepath);
    m_texture = SDL_CreateTextureFromSurface(renderer, retrieveSurface);
    SDL_FreeSurface(retrieveSurface);
} --> the problem is the resource management --> TODO*/ 
Animate::Animate(SDL_Renderer* renderer, std::string filepath) {
    SDL_Surface* retrieveSurface = IMG_Load(filepath.c_str());
    if (!retrieveSurface) {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
        return;
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, retrieveSurface);
    SDL_FreeSurface(retrieveSurface);

    if (!m_texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
    }
}

//Deconstructor
Animate::~Animate(){
    SDL_DestroyTexture(m_texture);
}

void Animate::Draw(int x, int y, int w, int h){
    //Create a rectangle
    m_dest.x = x;
    m_dest.y = y;
    m_dest.w = w;
    m_dest.h = h;
}

void Animate::PlayFrame(int x, int y, int w, int h, int frame){
    m_src.x = x + w*frame;
    m_src.y = y;
    m_src.w = w;
    m_src.h = h;
}

void Animate::Update(){

}

void Animate::Render(SDL_Renderer* renderer){
    if (!m_texture) {
        std::cerr << "Error: m_texture is nullptr\n";
        return;
    }
    
    std::cout << "Rendering: Source (" << m_src.x << "," << m_src.y << "," << m_src.w << "," << m_src.h
              << ") -> Destination (" << m_dest.x << "," << m_dest.y << "," << m_dest.w << "," << m_dest.h << ")\n";
    
    SDL_RenderCopy(renderer, m_texture, &m_src, &m_dest);
}
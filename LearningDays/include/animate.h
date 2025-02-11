#ifndef ANIMATE_H_INCLUDED
#define ANIMATE_H_INCLUDED
#include <SDL2/SDL.h>
#include <string>

class Animate{
    public:
    Animate(SDL_Renderer* renderer, std::string filepath);

    //Deconstructor:
    ~Animate();

    void Draw(int x, int y, int w, int h);
    
    //select and play a frame
    void PlayFrame(int x, int y, int w, int h, int frame); //of the source

    //Update
    void Update();

    //Render
    void Render(SDL_Renderer* renderer);
    //pointer *: change will be discarded once functions returns
    //pointer *&: change will persist when function returns

    private:
    SDL_Rect m_src; //select from
    SDL_Rect m_dest; //where we are rendering
    SDL_Texture* m_texture; //what we are rendering
};



#endif
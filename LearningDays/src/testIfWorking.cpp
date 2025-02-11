#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> //pixel based
#include <string>
#include "preDef.h"
#include "graphic.h"
#include "animate.h"
#include "resourceman.h"
using namespace std;

int main(int argc, char* argv[])
{
    // Create a window and a renderer
    Graphics graphic;
    graphic.init(); // check initialization

    // Make background
    /*SDL_Texture* background = graphic.loadTexture("IMAGES/parallax-mountain-bg_newratio.png");
    graphic.prepareScene(background);  // Render background first
    graphic.presentScene();  // Present the background scene to the screen*/

    //Make a platform:
    //Overlay:
    //SDL_Surface* surface = SDL_LoadBMP("IMAGES/high-skies-pixel-art-4k.bmp");
    /*SDL_Surface* retrieveSurfaces = ResourceManagement::GetInstace().GetSurface("IMAGES/high-skies-pixel-art-4k.bmp");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(graphic.getRender(), retrieveSurfaces);
    SDL_FreeSurface(retrieveSurfaces);*/

    SDL_Surface* retrieveSurface = IMG_Load("IMAGES/characters.png");
    SDL_Texture* m_texture = SDL_CreateTextureFromSurface(graphic.getRender(), retrieveSurface);

    //Create a rect:
    SDL_Rect rect;
    rect.x = 50;
    rect.y = 100;
    rect.w = 100;
    rect.h = 100;

    bool quit = false;
    Animate anime(graphic.getRender(), "IMAGES/characters.png");
    static int frame = 0; //23 frame

    while(!quit){ //the problem is that, when creating a platform, the screen blinks -> update every seconds
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){ // Check for window close (X button)
                quit = true;
                cout << "Owari-mashou" << "\n";
            }
        }
        //Make a clear canvas:
        SDL_SetRenderDrawColor(graphic.getRender(), 255, 255, 255, SDL_ALPHA_OPAQUE); //background
        SDL_RenderClear(graphic.getRender());

        //Do the drawing
        SDL_SetRenderDrawColor(graphic.getRender(), 0,0,0 , SDL_ALPHA_OPAQUE); //choose a color
        /*SDL_RenderDrawRect(graphic.getRender(), &rect);
        SDL_RenderCopy(graphic.getRender(), texture, NULL, &rect);*/
        anime.Draw(50,100,100,100);
        anime.PlayFrame(0,0,32,32, frame);
        anime.Render(graphic.getRender());
        frame++;
        if(frame > 23){
            frame = 0;
        }
        //Make it appear:
        SDL_RenderPresent(graphic.getRender());
        SDL_Delay(150);
    }

    // Cleanup
    /*SDL_DestroyTexture(background);
    background = NULL;*/
    //SDL_DestroyTexture(texture);
    graphic.quit();
    return 0;
}

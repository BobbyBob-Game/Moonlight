#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "preDef.h"

using namespace std;

SDL_Window* window;
SDL_Surface* screen;

int main(int argc, char* argv[])
{
    window = SDL_CreateWindow(WINDOW_TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //SDL surface: jpg, bmp, tga, png, gif
    //pitch: how many bytes in a row
    //Graph window surface
    screen = SDL_GetWindowSurface(window);
    SDL_Surface* img;
    img = SDL_LoadBMP("D:\\Bach\\Experimental Tech\\CodeBlocks\\Codeblock\\olympicTinhocKhongchuyen\\DemoSDL\\IMAGES\\high-skies-pixel-art-4k.bmp"); //does exist but too small to see
    SDL_BlitSurface(img,NULL,screen,NULL);
    SDL_FreeSurface(img);
    SDL_UpdateWindowSurface(window);

    bool gameRun = true;
    while(gameRun){
        SDL_Event event;
        //Start our event loop
        while(SDL_PollEvent(&event)){ //Detect of obj named event
            //Handle each specific event
            if(event.type == SDL_QUIT){ //click the X button
                gameRun = false;
            }
            if(event.type == SDL_MOUSEMOTION){ //this can be helpful for music puzzle
                cout<<"Mouse has been moved"<<"\n";
            }
            if(event.type == SDL_KEYDOWN){
                cout<<"A key has been pressed"<<"\n";
                if(event.key.keysym.sym == SDLK_0){ //virtual key: people might remap so use this, scankey: hardware keyboard
                    cout<<"0 was pressed"<<"\n";
                }
            }
            const Uint8* state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_RIGHT]){
                cout<<"Right arrow key was pressed"<<"\n";
            }
        }
    }


    //clean up
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

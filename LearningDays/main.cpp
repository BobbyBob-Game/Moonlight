#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "preDef.h"
#include "graphic.h"
// g++ -o testIfWorking testIfWorking.cpp `pkg-config --cflags --libs sdl2` -lSDL2_image
using namespace std;

int main(int argc, char* argv[])
{
    // Create a window and a renderer
    Graphics graphic;
    graphic.init(); // check initialization

    // Make background
    SDL_Texture* background = graphic.loadTexture("IMAGES/parallax-mountain-bg_newratio.png");
    graphic.prepareScene(background);  // Render background first
    graphic.presentScene();  // Present the final scene to the screen

    bool quit = false;
    while(!quit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){ // Check for window close (X button)
                quit = true;
                cout << "Owari-mashou" << "\n";
            }
        }
    }

    // Cleanup
    SDL_DestroyTexture(background);
    background = NULL;

    graphic.quit();
    return 0;
}

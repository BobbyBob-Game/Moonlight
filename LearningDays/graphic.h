#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

#include <SDL.h>
#include <SDL_image.h>
#include "preDef.h"
//This is for window and renderer creation
class Graphics{
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
public:
    void logErrorAndExit(const char* msg, const char* error){
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
    }

    void init(){
        if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
            logErrorAndExit("SDL_INIT", SDL_GetError());
        }
        else{
            window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            if(window == nullptr){
                //window was not created
                logErrorAndExit("CreateWindow", SDL_GetError());
            }
            if(!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)){
                logErrorAndExit("SDL_image", IMG_GetError());
            }

            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            if(renderer == nullptr){
                logErrorAndExit("CreateRenderer", SDL_GetError());
            }

            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
            SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }

    void prepareScene(SDL_Texture* background){
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
    }

    void presentScene(){
        SDL_RenderPresent(renderer);
    }


};

#endif // GRAPHIC_H_INCLUDED

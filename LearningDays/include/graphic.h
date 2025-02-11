#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "preDef.h"
//This is for window and renderer creation
/*
Use class: because has initializers
*/


class Graphics{
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
public:
    /*##################################################################

                        FUNCTION TO TELL ERROR  

    ##################################################################*/
    void logErrorAndExit(const char* msg, const char* error){
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
        SDL_Quit();
    }


    /*##################################################################

                        CHECK INITIALIZATION

    ##################################################################*/

    void init(){
        if(SDL_Init(SDL_INIT_EVERYTHING) != 0){ //if anything from audio, file, video,... is incorrectly initialized then dont create a window
            logErrorAndExit("SDL_INIT", SDL_GetError());
        }
        else{
            window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            if(window == nullptr){
                //window was not created
                logErrorAndExit("CreateWindow", SDL_GetError());
            }

            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                                    //   (SDL_Window* window, int index, Uint32 flags)
                                    //    index: pick a driver to render | -1 is to pick the first one that support
                                    //    flag: how we want the renderer to work
            if(renderer == nullptr){
                logErrorAndExit("CreateRenderer", SDL_GetError());
            }
            
            //DEBUGGING AREA
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); //set a hint for better debugging
            SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT); //Still allow logical resolution for rendering regardless of window size
        }
    }

    
    /*##################################################################

                        CREATE BACKGROUND (STATIC)

    ##################################################################*/

    void prepareScene(SDL_Texture* background){
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
    }

    void presentScene(){
        SDL_RenderPresent(renderer);
    }

    SDL_Renderer* getRender() {return renderer;} //get renderer, as we using class

    SDL_Texture* loadTexture(const char* filename){ //another texture | NOT THE BACKGROUND
        //if error happens
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,"Loading %s", filename);
        
        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);

        if(texture == nullptr){
            logErrorAndExit("LoadTexture", SDL_GetError());
        }

        return texture;
    }

    void renderTexture(SDL_Texture* texture, int x, int y){ //Overlay the rectangle with texture
        SDL_Rect dest; //create a rectangle
        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h); //get the width and height of the texture
        SDL_RenderCopy(renderer, texture, NULL, &dest); //copy the texture to the renderer
    }

    void quit(){
        IMG_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

};


#endif // GRAPHIC_H_INCLUDED



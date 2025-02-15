#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "preDef.h"
#include "entity.h"
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

class Dot
{
    public:
    int DOT_WIDTH = 20;
    int DOT_HEIGHT = 20;


    //Velocity:
    static const int DOT_VEL = 10;

    //Initialize the variables:
    Dot();

    //SetEntity for Dot:
    void setEntityAttr(const string& name, long long health, const string& power){
        player.setName(name);
        player.setHealth(health);
        player.setPower(power);
    }
    

    //Take key presses and adjust the dot's velocity
    void handleEvent(SDL_Event& event){
        if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
            //Adjust the velocity
            switch(event.key.keysym.sym){
                case SDLK_UP: mVelY -= DOT_VEL; std::cout<<"Up was pressed\n";break;
                case SDLK_DOWN: mVelY += DOT_VEL; std::cout<<"Down was pressed\n";break;
                case SDLK_LEFT: mVelX -= DOT_VEL; std::cout<<"Left was pressed\n";break;
                case SDLK_RIGHT: mVelX += DOT_VEL; std::cout<<"Right was pressed\n";break;
            }
        }
        //If a key was released
        else if(event.type == SDL_KEYUP && event.key.repeat == 0){
            //Adjust the velocity again to negate the previous velocity
            switch(event.key.keysym.sym){
                case SDLK_UP: mVelY += DOT_VEL; break;
                case SDLK_DOWN: mVelY -= DOT_VEL; break;
                case SDLK_LEFT: mVelX += DOT_VEL; break;
                case SDLK_RIGHT: mVelX -= DOT_VEL; break;
            }
        }
    }

    //Move the dot:
    void move(){
        mPosX += mVelX; //left and right
        //if out of screen
        if((mPosX < 0) || mPosX + DOT_WIDTH > SCREEN_WIDTH){
            //Move back
            mPosX -= mVelX;
        }

        mPosY += mVelY; //move up and down
        if((mPosY < 0) || mPosY + DOT_HEIGHT > SCREEN_HEIGHT){
            //Move back
            mPosY -= mVelY;
        }
        
    }

    //Shows on the screen
    void render(SDL_Texture* texture, SDL_Renderer* renderer) { 
        SDL_Rect renderQuad = { mPosX, mPosY, DOT_WIDTH, DOT_HEIGHT };  // Position and size
        if (SDL_RenderCopy(renderer, texture, NULL, &renderQuad) != 0) {
            std::cerr << "Failed to render texture: " << SDL_GetError() << std::endl;
        }
    }
    
    void isColliding(const SDL_Rect& wall){
        SDL_Rect renderQuad = { mPosX, mPosY, DOT_WIDTH, DOT_HEIGHT };
        if (SDL_HasIntersection(&wall, &renderQuad)) {
            // Undo movement
            mPosX -= mVelX;
            mPosY -= mVelY;
            //std::cout<<"Hitting a wall\n"<<std::endl;

            //now make it lose health if collision happens, if health == 0, says it dies
            player.setHealth(player.getHealth() - 10);
            if(player.getHealth() <= 0){
                cout<<player.getName()<<" has died\n";
            }
            else{
                cout<<player.getName()<<" took damage! Health remaining: "<<player.getHealth()<<"\n";
            }
        }
    }

    private:
    //X,Y offsets of the dot
    int mPosX, mPosY;
    //Velocity of the dot
    int mVelX, mVelY;

    Dot_Entity player;
};

Dot::Dot(){
    mPosX = 0;
    mPosY = 0;

    mVelX = 0;
    mVelY = 0;

    player.setName("Just a Dot");
    player.setHealth(1000);
    player.setPower("Basic Attack");
}







#endif // GRAPHIC_H_INCLUDED



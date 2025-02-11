#ifndef PREDEF_H_INCLUDED
#define PREDEF_H_INCLUDED
const int SCREEN_HEIGHT = 360;
const int SCREEN_WIDTH = 640; 
const char* WINDOW_TITLE = "Moonlight";


#endif // PREDEF_H_INCLUDED
//THE SCREEEN IS 16:9 RATIO
/*
SDL_Window: hold info about the window itself

SDL_Renderer: handle all rendering. Tied to SDL_Window so it can render within that window. Draw a hidden target
    SDL_RenderClear(renderer): clear redering target
    SDL_RenderCopy():
        SDL_Renderer* renderer: renderer you want to use for rendering
        SDL_Texture* texture: text you want to render
    SDL_RenderPresent(renderer): make the hidden target that SDL_Render*() drew visible

Another important difference is that SDL_Surface uses software rendering (via CPU) while SDL_Texture uses hardware rendering (via GPU).
If further modification of image is needed use Surface else use Texture. Best performance: use Texture
*/
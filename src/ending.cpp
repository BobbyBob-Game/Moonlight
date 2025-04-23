#include "ending.h"

CreditRenderer::CreditRenderer(SDL_Renderer* ren, TTF_Font* f, int w, int h)
 : renderer(ren), font(f), screenW(w), screenH(h), offsetY(h) {}

CreditRenderer::~CreditRenderer() {
    for (auto t : textures) SDL_DestroyTexture(t);
}

void CreditRenderer::start(const std::vector<std::string>& lines) {
    for (auto t : textures) SDL_DestroyTexture(t);
    textures.clear();
    rects.clear();

    int y = 0;
    for (auto& line : lines) {
        SDL_Surface* surf = TTF_RenderText_Solid(font, line.c_str(), {255, 255, 0, 255});
        if (!surf) { std::cerr << "TTF_RenderText: " << TTF_GetError() << "\n"; continue; }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = { (screenW - surf->w) / 2, y, surf->w, surf->h };
        SDL_FreeSurface(surf);
        textures.push_back(tex);
        rects.push_back(dst);
        y += dst.h + 10;  
    }
    offsetY = screenH;
}

bool CreditRenderer::updateAndRender(float deltaTime) {
    offsetY -= speed * deltaTime;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    bool anyOnScreen = false;
    for (int i = 0; i < textures.size(); ++i) {
        SDL_Rect dst = rects[i];
        dst.y = int(offsetY) + rects[i].y;
        if (dst.y + dst.h >= 0 && dst.y <= screenH) {
            SDL_RenderCopy(renderer, textures[i], nullptr, &dst);
            anyOnScreen = true;
        }
    }
    SDL_RenderPresent(renderer);
    return anyOnScreen;  
}

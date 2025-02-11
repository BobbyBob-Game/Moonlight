#ifndef RESOURCEMAN_H_INCLUDED
#define RESOURCEMAN_H_INCLUDED
#include <string>
#include <unordered_map>
#include <iostream>
#include <SDL2/SDL.h>

class ResourceManagement {
    private:
    ResourceManagement();
    ResourceManagement(ResourceManagement const&); //no copy of resource management
    ResourceManagement operator=(ResourceManagement const&);
    std::unordered_map<std::string, SDL_Surface*> m_surfaces;

    public:
    static ResourceManagement& GetInstance();
    SDL_Surface* GetSurface(std::string filepath);

};

#endif //RESOURCEMAN_H_INCLUDED
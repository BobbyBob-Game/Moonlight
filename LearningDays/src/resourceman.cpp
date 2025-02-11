#include "resourceman.h"

ResourceManagement::ResourceManagement(){

}

ResourceManagement::ResourceManagement(ResourceManagement const&){

}

ResourceManagement ResourceManagement::operator=(ResourceManagement const& rhs){
    return *this;
}

//instantiate exactly 1 instance of out class
ResourceManagement& ResourceManagement::GetInstance(){
    static ResourceManagement* s_instance = new ResourceManagement;
    return *s_instance;
}

SDL_Surface* ResourceManagement::GetSurface(std::string filepath){
    //Look throuh m_surfaces in unordered_map see if a file exists, if yes -> return associated surface, no -> load image
    auto search = m_surfaces.find(filepath);
    if(search != m_surfaces.end()){ //found
        return m_surfaces[filepath];
    }
    else{ //not found - add - return that just found to load for further operation
        SDL_Surface* surface = SDL_LoadBMP(filepath.c_str());
        m_surfaces.insert(std::make_pair(filepath, surface)); 
        return m_surfaces[filepath];
    }
    return nullptr;
    
}
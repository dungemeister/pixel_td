#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <SDL3_ttf/SDL_ttf.h>

struct HudComponent{

};

struct HudRenderSystem{

    void render(){

    }

    std::unordered_map<std::string, SDL_Texture*>   m_text_textures;
    std::unordered_map<std::string, SDL_Texture*>   m_textures;
    std::unordered_map<int, TTF_Font*>              m_fonts;
};
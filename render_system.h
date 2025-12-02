#pragma once
#include "components.h"
#include <SDL3/SDL.h>

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

class RenderSystem{
public:    
    RenderSystem(SDL_Window* window)
    {
        m_sprites.reserve(10);
        m_window = window;
        m_renderer = SDL_CreateRenderer(window, NULL);
        if(!m_renderer){
            SDL_Log("SDL_CreateRenderer: %s", SDL_GetError());
        }
        m_background_color.r = 0x18;
        m_background_color.g = 0x18;
        m_background_color.b = 0x18;
        m_background_color.a = 0xFF;

        load_texture("assets/rocket_tower.png");
        load_texture("assets/jotaro-kujo-josuke.png");
        load_texture("assets/quot-stickers.png");
        load_texture("assets/grass_tile_01.png");
    }

    void add_to_frame(const sprite_data_t&& data);
    void add_sprite_to_batch(const sprite_data_t& sprite);
    void render();
    void render_batch();
    void clean_batch_frame();
    void clean_frame();

    SDL_Window* m_window;
    std::vector<sprite_data_t> m_sprites;
    SDL_Renderer* m_renderer;
    SDL_Color m_background_color;
    std::unordered_map<std::string, SDL_Texture*> m_textures;
    std::unordered_map<SDL_Texture*, std::vector<SDL_Vertex>> m_vertexBatches;
    std::unordered_map<SDL_Texture*, std::vector<int>> m_indexBatches;
private:
    void init_custom_shaders();
    SDL_Texture* load_texture(std::string);
    SDL_Texture* get_texture(std::string);
    bool render_sprite(const sprite_data_t& sprite);
    void add_sprite_vertices(SDL_Texture* texture, float x, float y, float w, float h);
};
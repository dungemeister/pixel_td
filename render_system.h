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
        m_background_sprites.reserve(10);
        m_decoration_sprites.reserve(10);
        m_entity_sprites.reserve(10);

        m_window = window;
        m_renderer = SDL_CreateRenderer(window, NULL);
        if(!m_renderer){
            SDL_Log("SDL_CreateRenderer: %s", SDL_GetError());
        }
        m_background_color.r = 0x18;
        m_background_color.g = 0x18;
        m_background_color.b = 0x18;
        m_background_color.a = 0xFF;

        load_bush_texture();
    }

    void add_sprite_to_batch(const SpriteComponent& sprite);
    void render(const Entities& objects);
    void render_batch();
    void clean_batch_frame();
    void clean_frame();
    void load_to_layer(const SpriteComponent&, const HealthComponent& health);
    void unload_to_layer(const SpriteComponent&);
    bool is_type_valid(int type);

    void register_type_sprite(SpriteType type, const std::vector<std::string>& texturepath);
    void unregister_type_sprite(SpriteType type);
    
    std::vector<SDL_Texture*> get_registered_type_textures(SpriteType type);
    std::vector<std::string>  get_registered_type_textures_pathes(SpriteType type);

public:
    SDL_Window* m_window;
    std::vector<SpriteComponent> m_background_sprites;
    std::vector<SpriteComponent> m_decoration_sprites;
    std::vector<SpriteComponent> m_entity_sprites;
    std::vector<SpriteComponent> m_proj_sprites;
    std::vector<SpriteComponent> m_hud_sprites;

    std::vector<HealthComponent> m_entity_hps;

    SDL_Renderer* m_renderer;
    SDL_Color m_background_color;

    std::unordered_map<std::string, SDL_Texture*>             m_textures;
    std::unordered_map<SpriteType, std::vector<SDL_Texture*>> m_registered_textures;
    std::unordered_map<SpriteType, std::vector<std::string>>  m_registered_types;
    std::unordered_map<SDL_Texture*, std::vector<SDL_Vertex>> m_vertexBatches;
    std::unordered_map<SDL_Texture*, std::vector<int>>        m_indexBatches;

private:
    SDL_Texture* load_texture(std::string);
    void init_custom_shaders();
    SDL_Texture* load_bush_texture();
    SDL_Texture* get_texture(std::string);
    bool render_sprite(const SpriteComponent& sprite);
    void add_sprite_vertices(SDL_Texture* texture, float x, float y, float w, float h);
    bool render_sprite_texture(const SpriteComponent& sprite, SDL_Texture* text);
    size_t get_type_sprites_size(SpriteType type);
    void render_rectangle(SDL_FRect rect, float width, float angle, bool filled, const SDL_Color& color);
    bool render_bar(SDL_FRect rect, float angle, float percentage, const SDL_Color& color, float height_scale);
    bool render_health_bar(const SpriteComponent& sprite, const HealthComponent& health);
    bool render_cooldown_bar(const SpriteComponent& sprite);

private:
    std::vector<std::pair<std::string, SDL_FRect>> m_brushes_src;
};
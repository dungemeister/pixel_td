#include "render_system.h"
#include "wrapper.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_gpu.h>
#include "figures.h"

void RenderSystem::add_to_frame(const sprite_data_t&& data){
    m_sprites.emplace_back(data);
}

void RenderSystem::render(){

    for(auto& sprite_data: m_sprites){
        if(!render_sprite(sprite_data)){
            SDL_Log("render sprite: %s", SDL_GetError());
        }
    }
    m_sprites.clear();
    
}

void RenderSystem::init_custom_shaders(){
        const char* vertex_shader = 
        "#version 130\n"
        "in vec2 position;"
        "in vec2 texcoord;"
        "out vec2 Texcoord;"
        "void main() {"
        "    gl_Position = vec4(position, 0.0, 1.0);"
        "    Texcoord = texcoord;"
        "}";
        
    const char* fragment_shader =
        "#version 130\n"
        "in vec2 Texcoord;"
        "out vec4 outColor;"
        "uniform sampler2D tex;"
        "void main() {"
        "    outColor = texture(tex, Texcoord);"
        "}";
    
    // auto m_textureOnlyShader = SDL_CreateGPUShader(vertex_shader, fragment_shader, NULL);
    // SDL_SetRenderShader(m_renderer, m_textureOnlyShader);
}
SDL_Texture* RenderSystem::load_texture(std::string filepath){
    SDL_Surface* surface = IMG_Load(filepath.c_str());
    if(!surface){
        SDL_Log("IMG_Load: %s", SDL_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if(!texture){
        SDL_Log("SDL_CreateTextureFromSurface: %s", SDL_GetError());
        return nullptr;
    }
    m_textures.insert(std::pair{filepath, texture});
    return texture;
}

SDL_Texture* RenderSystem::get_texture(std::string filepath){
    SDL_Texture* text;
    auto iter = m_textures.find(filepath);
    if(iter != m_textures.end()){
        text = iter->second;
    }
    else{
        text = load_texture(filepath);
    }
    return text;
}

bool RenderSystem::render_sprite(const sprite_data_t& sprite){
    bool res;
    std::string textpath;
    switch(sprite.sprite){
        case 0:
            textpath = "assets/rocket_tower.png";
        break;
        case 1:
            textpath = "assets/jotaro-kujo-josuke.png";
        break;
        case 2:
            textpath = "assets/quot-stickers.png";
        break;
        case 4:
            textpath = "assets/rocket_tower.png";
            // textpath = "assets/grass_tile_01.png";
        break;
        default:
            textpath = "assets/quot-stickers.png";
        break;
    }
    SDL_Texture* text = get_texture(textpath);

    float w, h;
    SDL_GetTextureSize(text, &w, &h);

    SDL_FRect dest_rect = {sprite.posX, sprite.posY, 64, 64};
    if(sprite.width){
        dest_rect.w = sprite.width;
    }
    if(sprite.height){
        dest_rect.h = sprite.height;
    }
    res = SDL_RenderTexture(m_renderer, text, nullptr, &dest_rect);
    // SDL_FPoint center = {sprite.posX, sprite.posY};
    // SDL_RenderTextureRotated(m_renderer, text, nullptr, &dest_rect, 90.f, &center, SDL_FLIP_VERTICAL);

    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x00, 0x00, 0xFF);
    res = SDL_RenderRect(m_renderer, &dest_rect);
    // SDL_Color circle_color = {0x80, 0x00, 0x80, 0xFF};
    // res = Circle::render_circle(m_renderer,
    //                       dest_rect.x + dest_rect.w / 2,
    //                       dest_rect.y + dest_rect.h / 2,
    //                       SDL_sqrtf(dest_rect.w * dest_rect.w + dest_rect.h * dest_rect.h),
    //                       circle_color);
    // res = SDL_RenderTextureTiled(m_renderer, text, &r, scale, NULL);
    return res;
}

void RenderSystem::add_sprite_to_batch(const sprite_data_t& sprite){
    std::string textpath;
    switch(sprite.sprite){
        case 0:
            textpath = "assets/rocket_tower.png";
        break;
        case 1:
            textpath = "assets/jotaro-kujo-josuke.png";
        break;
        case 2:
            textpath = "assets/quot-stickers.png";
        break;
        case 4:
            textpath = "assets/grass_tile_01.png";
        break;
        default:
            textpath = "assets/quot-stickers.png";
        break;
    }
    add_sprite_vertices(get_texture(textpath), sprite.posX, sprite.posY, sprite.width, sprite.height);
}

void RenderSystem::add_sprite_vertices(SDL_Texture* texture, float x, float y, float w, float h) {
    int baseIndex = m_vertexBatches[texture].size();
    
    // Получаем размер текстуры для правильных текстурных координат
    float tex_w, tex_h;
    SDL_GetTextureSize(texture, &tex_w, &tex_h);
    
    // Текстурные координаты должны быть в диапазоне [0, 1]
    float u1 = 0.0f, v1 = 0.0f;
    float u2 = 1.0f, v2 = 1.0f;
    
    // Добавляем 4 вершины для спрайта с правильными текстурными координатами
    m_vertexBatches[texture].push_back({
        .position = {x, y},
        .color = {1., 1., 1., 1.}, // Белый цвет (без tint)
        .tex_coord = {u1, v1}
    });
    
    m_vertexBatches[texture].push_back({
        .position = {x + w, y},
        .color = {1., 1., 1., 1.},
        .tex_coord = {u2, v1}
    });
    
    m_vertexBatches[texture].push_back({
        .position = {x + w, y + h},
        .color = {1., 1., 1., 1.},
        .tex_coord = {u2, v2}
    });
    
    m_vertexBatches[texture].push_back({
        .position = {x, y + h},
        .color = {1., 1., 1., 1.},
        .tex_coord = {u1, v2}
    });
    
    // Добавляем индексы для двух треугольников (квадрат из 2 треугольников)
    auto& indices = m_indexBatches[texture];
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

void RenderSystem::begin_frame() {
    m_vertexBatches.clear();
    m_indexBatches.clear();
}

void RenderSystem::render_batch() {
    for (auto& [texture, vertices] : m_vertexBatches) {
        auto& indices = m_indexBatches[texture];
        if (!vertices.empty()) {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
            auto res = SDL_RenderGeometry(m_renderer, texture, 
                                vertices.data(), vertices.size(),
                                indices.data(), indices.size());
            if(!res) SDL_Log("ERROR: SDL_RenderGeometry %s", SDL_GetError());
        }
    }
    begin_frame();
}

#include "render_system.h"
#include "wrapper.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_gpu.h>
#include "figures.h"
#include "level.h"

void RenderSystem::render(const Entities& objects){
    clean_frame();
    for(auto& sprite: objects.m_sprites){
        load_to_layer(sprite);
    }

    for(auto& sprite_data: m_background_sprites){
        if(!render_sprite(sprite_data)){
            SDL_Log("render background sprite: %s", SDL_GetError());
        }
    }
    for(auto& sprite_data: m_decoration_sprites){
        if(!render_sprite(sprite_data)){
            SDL_Log("render decoration sprite: %s", SDL_GetError());
        }
    }
    for(auto& sprite_data: m_entity_sprites){
        if(!render_sprite(sprite_data)){
            SDL_Log("render entity sprite: %s", SDL_GetError());
        }
    }
    for(auto& sprite_data: m_proj_sprites){
        if(!render_sprite(sprite_data)){
            SDL_Log("render entity sprite: %s", SDL_GetError());
        }
    }
    for(auto& sprite_data: m_hud_sprites){
        if(!render_sprite(sprite_data)){
            SDL_Log("render entity sprite: %s", SDL_GetError());
        }
    }
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
    auto pos = filepath.find(".bmp");
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;
    if(pos == std::string::npos){

        surface = IMG_Load(filepath.c_str());
        if(!surface){
            SDL_Log("IMG_Load: %s", SDL_GetError());
            return nullptr;
        }
        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        if(!texture){
            SDL_Log("SDL_CreateTextureFromSurface: %s", SDL_GetError());
            return nullptr;
        }
        m_textures.insert(std::pair{filepath, texture});
    }
    else{
        surface = SDL_LoadBMP(filepath.c_str());
        if(!surface){
            SDL_Log("IMG_Load: %s", SDL_GetError());
            return nullptr;
        }
        auto format_details = SDL_GetPixelFormatDetails(surface->format);
        auto palette = SDL_GetSurfacePalette(surface);
        auto color_key = SDL_MapRGB(format_details, palette, 0xFF, 0, 0xFF);
        auto res = SDL_SetSurfaceColorKey(surface, true, color_key);

        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        if(!texture){
            SDL_Log("SDL_CreateTextureFromSurface: %s", SDL_GetError());
            return nullptr;
        }
        m_textures.insert(std::pair{filepath, texture});
    }
    SDL_DestroySurface(surface);
    return texture;
}

SDL_Texture* RenderSystem::load_bush_texture(){
    std::string filepath = "assets/bush_tiles.bmp";
    // SDL_Surface* surface = IMG_Load(filepath.c_str());
    SDL_Surface* surface = SDL_LoadBMP(filepath.c_str());
    if(!surface){
        SDL_Log("IMG_Load: %s", SDL_GetError());
        return nullptr;
    }
    auto format_details = SDL_GetPixelFormatDetails(surface->format);
    auto palette = SDL_GetSurfacePalette(surface);
    auto color_key = SDL_MapRGB(format_details, palette, 0xFF, 0, 0xFF);
    auto res = SDL_SetSurfaceColorKey(surface, true, color_key);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if(!texture){
        SDL_Log("SDL_CreateTextureFromSurface: %s", SDL_GetError());
        return nullptr;
    }
    m_textures.insert(std::pair{filepath, texture});
    
    size_t columns = 8;
    size_t rows = 5;
    size_t bushes = rows * columns;
    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    m_brushes_src.reserve(bushes);
    for(int i = 0; i < bushes; i++){
        SDL_FRect src = {.x = (w / columns) * (i % columns),
                         .y = (h / rows) * (i / columns),
                         .w = (w / columns) ,
                         .h = (h / rows) ,};
        
        m_brushes_src.emplace_back("bush" + std::to_string(i), src);
    }
    SDL_DestroySurface(surface);
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

bool RenderSystem::render_sprite(const SpriteComponent& sprite){
    bool res = true;
    auto textures = get_registered_type_textures(sprite.type);
    auto textures_pathes = get_registered_type_textures_pathes(sprite.type);
    if(sprite.anim_index != -1 && textures.size()){
        res &= render_sprite_texture(sprite, textures[sprite.anim_index]);
    }
    else if(textures.size() > 0){
        for(const auto& text: textures){
            res &= render_sprite_texture(sprite, text);
        }
    }
    else{
        render_rectangle({sprite.posX, sprite.posY, sprite.width, sprite.height},
                        2.f,
                        0.f);
    }
    return res;
}

void RenderSystem::add_sprite_to_batch(const SpriteComponent& sprite){
    auto textures = get_registered_type_textures(sprite.type);
    for(const auto& text: textures){
        add_sprite_vertices(text, sprite.posX, sprite.posY, sprite.width, sprite.height);
    }
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

void RenderSystem::clean_batch_frame() {
    m_vertexBatches.clear();
    m_indexBatches.clear();
}

void RenderSystem::render_batch() {
    for (auto& [texture, vertices] : m_vertexBatches) {
        auto& indices = m_indexBatches[texture];
        if (!vertices.empty()) {
            // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            // SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
            auto res = SDL_RenderGeometry(m_renderer, texture, 
                                vertices.data(), vertices.size(),
                                indices.data(), indices.size());
            if(!res) SDL_Log("ERROR: SDL_RenderGeometry %s", SDL_GetError());
            
        }
    }
}

void RenderSystem::clean_frame(){
    m_background_sprites.clear();
    m_decoration_sprites.clear();
    m_entity_sprites.clear();
    m_proj_sprites.clear();
    m_hud_sprites.clear();
}

void RenderSystem::load_to_layer(const SpriteComponent& sprite){

    switch(sprite.layer){
        case SpriteLayer::BACKGROUND:
            m_background_sprites.emplace_back(sprite);
        break;
        case SpriteLayer::DECORATION:
            m_decoration_sprites.emplace_back(sprite);
        break;
        case SpriteLayer::ENTITY:
            m_entity_sprites.emplace_back(sprite);
        break;
        case SpriteLayer::PROJECTILES:
            m_proj_sprites.emplace_back(sprite);
        break;
        case SpriteLayer::HUD:
            m_hud_sprites.emplace_back(sprite);
        break;
    }
}

void RenderSystem::register_type_sprite(SpriteType type, const std::vector<std::string>& pathes){
    std::vector<SDL_Texture*> type_textures;
    for(const auto& texturepath: pathes){
        auto text = get_texture(texturepath);
        if(!text){
            SDL_Log("ERROR: fail to register sprite for type %d", type);
            continue;
        }
        type_textures.emplace_back(text);
    }
    m_registered_types.insert({type, pathes});
    m_registered_textures.insert({type, type_textures});

}

std::vector<SDL_Texture*> RenderSystem::get_registered_type_textures(SpriteType type){
    auto it = m_registered_textures.find(type);
    if(it != m_registered_textures.end()){
        return it->second;
    }
    SDL_Log("WARNING: fail to get registered type %d textures", type);
    return {};
}

std::vector<std::string> RenderSystem::get_registered_type_textures_pathes(SpriteType type){
    auto it = m_registered_types.find(type);
    if(it != m_registered_types.end()){
        return it->second;
    }
    SDL_Log("WARNING: fail to get registered type %d textures pathes", type);
    return {};
}

bool RenderSystem::render_sprite_texture(const SpriteComponent& sprite, SDL_Texture* text){
    bool res = false;
    SDL_FRect dest_rect = {sprite.posX,
                           sprite.posY,
                           sprite.width * sprite.scale,
                           sprite.height * sprite.scale};
    if(sprite.flag == fCenterSprite){

        //Shift sprite to the center of position
        dest_rect.x -= dest_rect.w / 2;
        dest_rect.y -= dest_rect.h / 2;
    }
    SDL_FPoint center = {dest_rect.w / 2, dest_rect.h / 2};
    float angle = sprite.angle;
    SDL_FlipMode mode = SDL_FLIP_NONE;
    res = SDL_RenderTextureRotated(m_renderer, text, nullptr, &dest_rect, angle, &center, mode);

    if(sprite.flag & fSpriteBorder){
        SDL_Surface* surface = SDL_CreateSurface(dest_rect.w, dest_rect.h, SDL_PIXELFORMAT_RGBA32);
        if (!surface) {
            printf("Failed to create surface: %s\n", SDL_GetError());
            return false;
        }
        
        // Заполняем поверхность прозрачным цветом
        SDL_FillSurfaceRect(surface, NULL, 0x0);
    
        // Рисуем контур прямоугольника
        int border_thickness = 1.f;
        SDL_Rect outer_rect = {0, 0, static_cast<int>(dest_rect.w), static_cast<int>(dest_rect.h)};
        SDL_Rect inner_rect = {
            border_thickness, 
            border_thickness, 
            static_cast<int>(dest_rect.w) - 2 * border_thickness, 
            static_cast<int>(dest_rect.h) - 2 * border_thickness
        };


        // Заполняем весь прямоугольник цветом
        SDL_FillSurfaceRect(surface, &outer_rect, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 0xFF, 0, 0, 0xFF));
        // Вырезаем внутреннюю часть чтобы получить контур
        SDL_FillSurfaceRect(surface, &inner_rect, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 0, 0, 0, 0));
        SDL_Texture* border_text = SDL_CreateTextureFromSurface(m_renderer, surface);
        res = SDL_RenderTextureRotated(m_renderer, border_text, nullptr, &dest_rect, angle, &center, SDL_FLIP_HORIZONTAL);
        
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(border_text);

        // SDL_Surface* border_surface = SDL_CreateSurface(dest_rect.w, dest_rect.h, SDL_PIXELFORMAT_RGBA32);
        // SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x00, 0x00, 0xFF);
        // res = SDL_RenderRect(m_renderer, &dest_rect);
        // SDL_Color circle_color = {0x80, 0x00, 0x80, 0xFF};
        // res = Circle::render_circle(m_renderer,
        //                       dest_rect.x + dest_rect.w / 2,
        //                       dest_rect.y + dest_rect.h / 2,
        //                       SDL_sqrtf(dest_rect.w * dest_rect.w + dest_rect.h * dest_rect.h),
        //                       circle_color);
        // res = SDL_RenderTextureTiled(m_renderer, text, &r, scale, NULL);
    }
    return res;
}

size_t RenderSystem::get_type_sprites_size(SpriteType type){
    size_t size = 0;
    auto it = m_registered_textures.find(type);
    if(it != m_registered_textures.end()){
        return it->second.size();
    }
    SDL_Log("WARNING: fail to get registered type %d textures", type);
    return size;
}

void RenderSystem::render_rectangle(SDL_FRect dest_rect, float width, float angle){
    SDL_Surface* surface = SDL_CreateSurface(dest_rect.w, dest_rect.h, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return;
    }
    
    // Заполняем поверхность прозрачным цветом
    SDL_FillSurfaceRect(surface, NULL, 0x0);

    // Рисуем контур прямоугольника
    int border_thickness = 1.f;
    SDL_Rect outer_rect = {0, 0, static_cast<int>(dest_rect.w), static_cast<int>(dest_rect.h)};
    SDL_Rect inner_rect = {
        border_thickness, 
        border_thickness, 
        static_cast<int>(dest_rect.w) - 2 * border_thickness, 
        static_cast<int>(dest_rect.h) - 2 * border_thickness
    };

    SDL_FPoint center = {dest_rect.w / 2, dest_rect.h / 2};
    // Заполняем весь прямоугольник цветом
    SDL_FillSurfaceRect(surface, &outer_rect, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 0xFF, 0, 0, 0xFF));
    // Вырезаем внутреннюю часть чтобы получить контур
    SDL_FillSurfaceRect(surface, &inner_rect, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 0, 0, 0, 0));
    SDL_Texture* border_text = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_RenderTextureRotated(m_renderer, border_text, nullptr, &dest_rect, angle, &center, SDL_FLIP_HORIZONTAL);
    
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(border_text);
}
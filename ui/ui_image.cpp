#include "ui_image.h"
#include "ui_layout.h"
#include "SDL3_image/SDL_image.h"

UIImage::UIImage(const std::string& id)
:UIWidget(id)
{
    
}

UIImage::UIImage(const std::string& sprite, const std::string& id)
:UIWidget(id)
,m_cur_sprite(sprite)

{
    load_texture(sprite);
}

void UIImage::Update(float deltatime){
    load_texture(m_cur_sprite);
}

void UIImage::Draw(SDL_Renderer* renderer){
    auto texture = m_sprites.find(m_cur_sprite);
    if(texture != m_sprites.end()){
        SDL_RenderTexture(renderer, texture->second, NULL, &m_rect);
    }
    else{
        load_texture(m_cur_sprite);
        texture = m_sprites.find(m_cur_sprite);
        SDL_RenderTexture(renderer, texture->second, NULL, &m_rect);
    }
}

void UIImage::AddSprite(const std::string& sprite){
    if(m_sprites.count(sprite) > 0){
        return;
    }
    load_texture(sprite);
}

void UIImage::load_texture(const std::string& sprite){
    if(!m_layout) return;

    auto surface = IMG_Load(sprite.c_str());
    if(!surface){
        SDL_Log("IMG_Load: %s", SDL_GetError());
        return;
    }
    auto texture = SDL_CreateTextureFromSurface(m_layout->GetRenderer(), surface);
    if(!texture){
        SDL_Log("SDL_CreateTextureFromSurface: %s", SDL_GetError());
    }
    m_sprites.insert(std::pair{sprite, texture});
    if(!m_rect.w)
        m_rect.w = surface->w;
    if(!m_rect.h)
        m_rect.h = surface->h;
    SDL_DestroySurface(surface);
}
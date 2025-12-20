#include "ui_image.h"
#include "ui_layout.h"
#include "SDL3_image/SDL_image.h"

UIImage::UIImage(UILayout* layout)
:UIWidget(layout)
{
    
}

UIImage::UIImage(const std::string& sprite, UILayout* layout)
:UIWidget(layout)
,m_cur_sprite(sprite)

{
    load_texture(sprite);
}

void UIImage::Update(float deltatime){

}

void UIImage::Draw(){
    auto render = m_layout->GetRenderer();
    auto texture = m_sprites.find(m_cur_sprite);
    if(!m_cur_sprite.empty() && (texture != m_sprites.end())){
        SDL_RenderTexture(render, texture->second, NULL, &m_rect);
    }
}

void UIImage::AddSprite(const std::string& sprite){
    if(m_sprites.count(sprite) > 0){
        return;
    }
    load_texture(sprite);
}

void UIImage::load_texture(const std::string& sprite){
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
    m_rect.w = surface->w;
    m_rect.h = surface->h;
    SDL_DestroySurface(surface);
}
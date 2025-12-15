#include "ui_text.h"
#include "ui_layout.h"
#include "game.h"
#include "palette.h"

UIText::UIText(class UILayout* layout)
    :UIWidget(layout)
    ,m_texture(nullptr)
    ,m_font(nullptr)
    ,m_font_size(16)
    ,m_text()
{
    m_color = Colors::OceanSunset::bittersweet;
}

UIText::UIText(const std::string& text, class UILayout* layout)
    :UIText(layout)
{
    SetText(text);
}

UIText::~UIText(){
    SDL_DestroyTexture(m_texture);
    TTF_CloseFont(m_font);
}

void UIText::UpdateTexture(){
    if(!m_layout) return;

    if(m_texture){
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
    if(m_text.empty()) return;

    auto surface = CreateSurface(m_text);
    if(surface){
        m_texture = SDL_CreateTextureFromSurface(m_layout->GetRenderer(), surface);
        m_rect.w = surface->w;
        m_rect.h = surface->h;
        SDL_DestroySurface(surface);
    }
}

void UIText::SetText(const std::string& text){
    if(m_text == text) return;
    
    m_text = text;
    UpdateTexture();

}

void UIText::SetFontSize(int size){
    if(size < 0) return;

    m_font_size = size;
    UpdateTexture();
}

void UIText::Update(float deltatime){

}

void UIText::Draw(){
    if(m_hiden) return;

    DrawBackground();
    DrawBorder();
    DrawText();

}

void UIText::DrawText(){
    if(!m_layout) return;

    auto render = m_layout->GetRenderer();
    if(m_texture){
        SDL_RenderTexture(render, m_texture, NULL, &m_rect);
    }
}

void UIText::DrawBorder(){
    if(!m_layout) return;

    auto renderer = m_layout->GetRenderer();
    SDL_Color color = Colors::OceanSunset::midnight_green;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderRect(renderer, &m_rect);
}

void UIText::DrawBackground(){
    if(!m_layout) return;

    auto renderer = m_layout->GetRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, m_color.r, m_color.g, m_color.b, 33); // Белый цвет

    SDL_RenderFillRect(renderer, &m_rect);
}

SDL_Surface* UIText::CreateSurface(const std::string& text){
    if(m_font){
        TTF_CloseFont(m_font);
    }
    m_font = TTF_OpenFont(m_font_path.c_str(), m_font_size);
    auto res = TTF_SetFontDirection(m_font, TTF_Direction::TTF_DIRECTION_LTR);
    if(!res){
        SDL_Log("%s", SDL_GetError());
    }
    return TTF_RenderText_Blended(m_font, text.c_str(), text.size(), m_color);
}


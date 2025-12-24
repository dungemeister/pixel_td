#include "ui_label.h"
#include "ui_layout.h"
#include "game.h"
#include "palette.h"

UILabel::UILabel(const std::string& id)
    :UIWidget(id)
    ,m_texture(nullptr)
    ,m_font(nullptr)
    ,m_font_size(16)
    ,m_text()
    ,m_background(false)
    ,m_border(false)
{
    m_color = Colors::OceanSunset::bittersweet;
}

UILabel::UILabel(const std::string& text, const std::string& id)
    :UILabel(id)
{
    SetText(text);
}

UILabel::~UILabel(){
    SDL_DestroyTexture(m_texture);
}

void UILabel::UpdateTexture(){
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

void UILabel::SetText(const std::string& text){
    m_text = text;
    UpdateTexture();

}

void UILabel::SetFontSize(int size){
    if(size < 0) return;

    m_font_size = size;
    UpdateTexture();
}

void UILabel::Update(float deltatime){
    SetText(m_text);
}

void UILabel::Draw(SDL_Renderer* renderer){
    if(m_hiden) return;

    DrawBackground(renderer);
    DrawBorder(renderer);
    DrawText(renderer);

}

void UILabel::DrawText(SDL_Renderer* renderer){
    if(!m_layout) return;

    if(!m_texture){
        UpdateTexture();
    }
    SDL_RenderTexture(renderer, m_texture, NULL, &m_rect);
}

void UILabel::DrawBorder(SDL_Renderer* renderer){
    if(!m_layout) return;
    if(!m_border) return;

    SDL_Color color = Colors::OceanSunset::midnight_green;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderRect(renderer, &m_rect);
}

void UILabel::DrawBackground(SDL_Renderer* renderer){
    if(!m_layout) return;
    if(!m_background) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, m_color.r, m_color.g, m_color.b, 33); // Белый цвет

    SDL_RenderFillRect(renderer, &m_rect);
}

SDL_Surface* UILabel::CreateSurface(const std::string& text){
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


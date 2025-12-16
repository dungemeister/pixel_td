#include "ui_slider.h"
#include "ui_layout.h"

UISlider::UISlider(UILayout* layout)
:UIWidget(layout)
,m_max_value(0.f)
,m_min_value(0.f)
,m_value(0.f)
{

}
void UISlider::Update(float deltatime){

}

void UISlider::Draw(){
    SDL_FRect border_rect = {m_rect.x, m_rect.y, m_rect.w, m_rect.h};
    SDL_FRect filled_rect = {m_rect.x, m_rect.y, m_rect.w * get_factor(), m_rect.h};
    render_rectangle(border_rect, 2.f, false);
    render_rectangle(filled_rect, 2.f, true);
}

void UISlider::render_rectangle(SDL_FRect dest_rect, float width, bool filled){
    if(!m_layout) return;

    auto render = m_layout->GetRenderer();
    SDL_Surface* surface = SDL_CreateSurface(dest_rect.w, dest_rect.h, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        SDL_Log("Failed to create surface: %s\n", SDL_GetError());
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
    if(!filled){
        // Вырезаем внутреннюю часть чтобы получить контур
        SDL_FillSurfaceRect(surface, &inner_rect, SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 0, 0, 0, 0));
    }

    SDL_Texture* border_text = SDL_CreateTextureFromSurface(render, surface);
    SDL_RenderTextureRotated(render, border_text, nullptr, &dest_rect, 0, &center, SDL_FLIP_HORIZONTAL);
    
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(border_text);
}
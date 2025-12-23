#include "ui_slider.h"
#include "ui_layout.h"

UISlider::UISlider(const std::string& id)
:UIWidget(id)
,m_max_value(0.f)
,m_min_value(0.f)
,m_value(0.f)
{

}
void UISlider::Update(float deltatime){

}

void UISlider::Draw(SDL_Renderer* renderer){
    SDL_FRect border_rect = GetSize();

    SDL_FRect filled_rect = GetSize();
    filled_rect.w *= get_factor();

    render_rectangle(renderer, border_rect, 2.f, false);
    render_rectangle(renderer, filled_rect, 2.f, true);
}

void UISlider::render_rectangle(SDL_Renderer* renderer, SDL_FRect dest_rect, float width, bool filled){
    if(!m_layout) return;

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

    SDL_Texture* border_text = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderTextureRotated(renderer, border_text, nullptr, &dest_rect, 0, &center, SDL_FLIP_HORIZONTAL);
    
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(border_text);
}
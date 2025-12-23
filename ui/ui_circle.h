#pragma once
#include "ui_widget.h"
#include <SDL3/SDL.h>

class UICircle: public UIWidget{
public:
    UICircle(const std::string& id);
    ~UICircle();
    void Draw(SDL_Renderer* renderer) override;
    void set_params(SDL_FPoint center, float radius, SDL_FColor color);
private:
    float       m_radius;
    SDL_FPoint  m_center;
    SDL_FColor  m_color;
    const int   m_segments = 36; 
    SDL_Vertex* m_vertices;
    int*        m_indices;
};
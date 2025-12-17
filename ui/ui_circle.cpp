#include "ui_circle.h"
#include "ui_layout.h"
#include <stdlib.h>

UICircle::UICircle(UILayout* layout)
:UIWidget(layout)
,m_radius(0.f)
,m_center()
,m_color()
{
    m_vertices  = reinterpret_cast<SDL_Vertex*>(malloc(sizeof(SDL_Vertex) * (m_segments + 1)));
    m_indices   = reinterpret_cast<int*>(malloc(sizeof(int) * m_segments * 3));
}

UICircle::~UICircle(){
    free(m_vertices);
    free(m_indices);
}

void UICircle::Draw(){
    SDL_FColor& col = m_color;
    auto render = m_layout->GetRenderer();
    
    // Central vertice
    m_vertices[0] = (SDL_Vertex){
        .position = { m_center.x, m_center.y },
        .color = col
    };
    
    //Other vertices
    for (int i = 0; i < m_segments; i++) {
        float angle = 2.0f * SDL_PI_F * i / m_segments;
        m_vertices[i + 1] = (SDL_Vertex){
            .position = { 
                m_center.x + m_radius * SDL_cosf(angle),
                m_center.y + m_radius * SDL_sinf(angle)
            },
            .color = col
        };
    }
    //Push indices to draw triangle segments
    for (int i = 0; i < m_segments; i++) {
        m_indices[i * 3] = 0; // Центр
        m_indices[i * 3 + 1] = i + 1;
        m_indices[i * 3 + 2] = (i + 1) % m_segments + 1;
    }
    
    SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
    auto res = SDL_RenderGeometry(render, NULL, m_vertices, m_segments + 1, m_indices, m_segments * 3);
    
}

void UICircle::set_params(SDL_FPoint center, float radius, SDL_FColor color){
    m_center = center;
    m_radius = radius;
    m_color = color;
}
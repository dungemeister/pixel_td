#pragma once
#include <SDL3/SDL.h>
#include <stdlib.h>
class Circle{
public:
    static bool render_circle(SDL_Renderer* r, float centerX, float centerY, float radius, const SDL_Color& color){
        int segments_size = 36;
        SDL_FPoint points[segments_size + 1];

        for(int i = 0; i < segments_size; i++){
            float angle_rad = (2 * SDL_PI_F / segments_size) * i;
            points[i] = {.x = centerX + radius * SDL_cosf(angle_rad),
                         .y = centerY + radius * SDL_sinf(angle_rad)};
        }
        points[segments_size] = points[0];

        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
        return SDL_RenderLines(r, points, segments_size + 1);
    }

    static bool render_circle_filled(SDL_Renderer* r, float centerX, float centerY, float radius, const SDL_Color& color){
        SDL_FColor col = {  static_cast<float>(color.r) / 255,
                            static_cast<float>(color.g) / 255,
                            static_cast<float>(color.b) / 255,
                            static_cast<float>(color.a) / 255,
        };
        const int segments = 36; // Количество сегментов для аппроксимации круга
        SDL_Vertex* vertices = reinterpret_cast<SDL_Vertex*>(malloc(sizeof(SDL_Vertex) * (segments + 1)));
        
        // Центральная вершина
        vertices[0] = (SDL_Vertex){
            .position = { centerX, centerY },
            .color = col
        };
        
        // Вершины по окружности
        for (int i = 0; i < segments; i++) {
            float angle = 2.0f * SDL_PI_F * i / segments;
            vertices[i + 1] = (SDL_Vertex){
                .position = { 
                    centerX + radius * SDL_cosf(angle),
                    centerY + radius * SDL_sinf(angle)
                },
                .color = col
            };
        }
        
        // Индексы для треугольников
        int* indices = reinterpret_cast<int*>(malloc(sizeof(int) * segments * 3));
        for (int i = 0; i < segments; i++) {
            indices[i * 3] = 0; // Центр
            indices[i * 3 + 1] = i + 1;
            indices[i * 3 + 2] = (i + 1) % segments + 1;
        }
        
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        auto res = SDL_RenderGeometry(r, NULL, vertices, segments + 1, indices, segments * 3);
        
        free(vertices);
        free(indices);
        return res;
    }
};


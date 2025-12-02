#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <string>
#include <vector>

class TextureTest {
public:
    static bool test_texture_rendering(SDL_Renderer* renderer, const std::string& texture_path) {
        // Загружаем текстуру
        SDL_Surface* surface = IMG_Load(texture_path.c_str());
        if (!surface) {
            printf("Failed to load test texture\n");
            return false;
        }
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        
        if (!texture) {
            printf("Failed to create test texture\n");
            return false;
        }
        
        // Устанавливаем blend mode
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        // Рендерим обычным способом для сравнения
        SDL_FRect rect1 = {100, 100, 64, 64};
        SDL_RenderTexture(renderer, texture, NULL, &rect1);
        
        // Рендерим через geometry
        std::vector<SDL_Vertex> vertices = {
            {{200, 100}, {255, 255, 255, 255}, {0.0f, 0.0f}},
            {{264, 100}, {255, 255, 255, 255}, {1.0f, 0.0f}},
            {{264, 164}, {255, 255, 255, 255}, {1.0f, 1.0f}},
            {{200, 164}, {255, 255, 255, 255}, {0.0f, 1.0f}}
        };
        
        std::vector<int> indices = {0, 1, 2, 0, 2, 3};
        
        bool result = SDL_RenderGeometry(renderer, texture, 
                                       vertices.data(), vertices.size(),
                                       indices.data(), indices.size());
        
        SDL_DestroyTexture(texture);
        return result;
    }
};
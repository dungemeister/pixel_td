#pragma once
#include <SDL3/SDL.h>

class SdlWrapper{
public:
    SdlWrapper() = delete;
    static void W_SDL_SetRenderDrawColor(SDL_Renderer* renderer, const SDL_Color& color){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }

    static SDL_FColor W_SDL_ConvertToFColor(const SDL_Color& color){
        return {static_cast<float>(color.r) / 0xFF,
                static_cast<float>(color.g) / 0xFF,
                static_cast<float>(color.b) / 0xFF,
                static_cast<float>(color.a) / 0xFF};
    }
};
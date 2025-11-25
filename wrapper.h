#pragma once
#include <SDL3/SDL.h>

class SdlWrapper{
public:
    SdlWrapper() = delete;
    static void W_SDL_SetRenderDrawColor(SDL_Renderer* renderer, const SDL_Color& color){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    }
};
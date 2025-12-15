#pragma once
#include "SDL3/SDL.h"

namespace Colors {
    static SDL_Color lerp(const SDL_Color& a, const SDL_Color& b, float t){
        return {static_cast<Uint8>(a.r + (b.r - a.r) * t),
                static_cast<Uint8>(a.g + (b.g - a.g) * t),
                static_cast<Uint8>(a.b + (b.b - a.b) * t),
                static_cast<Uint8>(a.a + (b.a - a.a) * t)
                };
    }
    namespace Sunset {
        constexpr SDL_Color saffron = {0xF2, 0xC4, 0x47, 0xFF};
        constexpr SDL_Color pantone = {0xF7, 0x62, 0x18, 0xFF};
        constexpr SDL_Color folly = {0xFF, 0x1D, 0x68, 0xFF};
        constexpr SDL_Color magenta_dye = {0xB1, 0x00, 0x65, 0xFF};
        constexpr SDL_Color purple = {0x74, 0x05, 0x80, 0xFF};
    }
    namespace OceanSunset{
        constexpr SDL_Color bittersweet = {0xFF, 0x77, 0x5A, 0xFF};
        constexpr SDL_Color atomic_tangerine = {0xFD, 0x92, 0x48, 0xFF};
        constexpr SDL_Color moonstone = {0x00, 0xA7, 0xB7, 0xFF};
        constexpr SDL_Color dark_cyan = {0x22, 0x8C, 0x85, 0xFF};
        constexpr SDL_Color midnight_green = {0x10, 0x55, 0x5B, 0xFF};
    }
}

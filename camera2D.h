#pragma once
#include "vector2d.h"

struct Camera2D {
    // camera's center в world pixels
    float cx = 0.f;   
    float cy = 0.f;
    // camera's zoom
    float zoom = 1.f;

    int viewport_width = 1;    // размеры world viewport
    int viewport_height = 1;

    inline SDL_FPoint world_to_screen(SDL_FPoint w)
    {
        const float hx = viewport_width * 0.5f;
        const float hy = viewport_height * 0.5f;
        return SDL_FPoint{
            (w.x - cx) * zoom + hx,
            (w.y - cy) * zoom + hy
        };
    }

    static inline SDL_FPoint screen_to_world(const Camera2D& cam, SDL_FPoint pos)
    {
        const float hx = cam.viewport_width * 0.5f;
        const float hy = cam.viewport_height * 0.5f;
        return SDL_FPoint{
            (pos.x - hx) / cam.zoom + cam.cx,
            (pos.y - hy) / cam.zoom + cam.cy
        };
    }

    static inline bool in_rect(int x, int y, const SDL_Rect& r)
    {
        return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
    }

    inline void zoom_at_mouse(const SDL_Rect& worldVP,
                                int mouseX, int mouseY, float zoomMul)
    {
        if (!in_rect(mouseX, mouseY, worldVP)) return;

        SDL_FPoint mInVP { float(mouseX - worldVP.x), float(mouseY - worldVP.y) };

        const SDL_FPoint before = screen_to_world(*this, mInVP);

        zoom *= zoomMul;
        zoom = std::clamp(zoom, 1.0f, 4.0f);
        if(zoom <= 1.0f){
            cx = 0.f;
            cy = 0.f;
        }
        else{
            const SDL_FPoint after = screen_to_world(*this, mInVP);

            cx += (before.x - after.x);
            cy += (before.y - after.y);
        }
    }
};
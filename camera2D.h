#pragma once
#include "vector2d.h"

struct Camera2D{
    Camera2D(int width, int height)
    :zoom(1.f)
    ,viewport_width(width)
    ,viewport_height(height)
    ,world_center(width/2, height/2)
    {

    }

    Camera2D()
    :Camera2D(1920, 1080)
    {}

    float zoom;
    int viewport_width;
    int viewport_height;
    Vector2D world_center;
};

inline Vector2D world_to_screen(const Camera2D& cam, Vector2D world){
    const Vector2D half{ cam.viewport_width * 0.5f, cam.viewport_height * 0.5f };
    return {
        (world.x - cam.world_center.x) * cam.zoom + half.x,
        (world.y - cam.world_center.y) * cam.zoom + half.y
    };
}

inline Vector2D ScreenToWorld(const Camera2D& cam, Vector2D screen)
{
    const Vector2D half{ cam.viewport_width * 0.5f, cam.viewport_height * 0.5f };
    return {
        (screen.x - half.x) / cam.zoom + cam.world_center.x,
        (screen.y - half.y) / cam.zoom + cam.world_center.y
    };
}
#pragma once
#include "vector2d.h"
#include "SDL3/SDL.h"
#include "components.h"
#include <vector>
#include <unordered_map>

enum ComponentType{
    CASTLE_HEALTH,
    PLAYER_GOLD,
};

struct GuiComponent{
    GuiComponent()
    :horiz_padding(5.f)
    ,vert_padding(5.f) {}

    Vector2D pos;

    int start_row;
    int start_column;

    int rows;
    int columns;

    float tile_width;
    float tile_height;

    float horiz_padding;
    float vert_padding;

    SDL_FRect get_size() { return {pos.x, pos.y, columns * tile_width, rows * tile_height}; }
    SDL_FRect get_size_padded() { return {pos.x + horiz_padding,
                                          pos.y + vert_padding,
                                          columns * tile_width - 2 * horiz_padding,
                                          rows * tile_height - 2 * vert_padding}; }
    void update(float deltatime) {}

    // sprite string bar button
};

enum LayerAlignment{
    AlignCenter,
    AlignLeft,
    AlignRight,
};

struct LayerComponent{
    LayerComponent(SDL_FRect _rect, int _rows, int _columns, LayerComponent* _parent)
    :rows(_rows)
    ,columns(_columns)
    ,parent(_parent)
    ,pos({_rect.x, _rect.y})
    ,rect(_rect)
    {
        components.reserve(rows * columns);
        grid_width  = rect.w / columns;
        grid_height = rect.h / rows;
    }

    LayerComponent()
    :LayerComponent({}, 0, 0, nullptr) {}

    // void add_to_main_layer(SDL_FRect& rect, int rows, int columns){
    //     children.emplace_back(rect, rows, columns, this);
    // }

    GuiComponent add_component(int row, int column, int rows, int columns){
        GuiComponent comp;
        comp.start_row    = row;
        comp.start_column = column;
        comp.rows    = rows;
        comp.columns = columns;
        comp.tile_width  = grid_width;
        comp.tile_height = grid_height;
        comp.pos = {pos.x + comp.start_column * grid_width, pos.y + comp.start_row * grid_height}; 

        components.push_back(comp);
        return comp;
    }

    void set_grid(int _rows, int _columns){
        rows = _rows;
        columns = _columns;
        grid_width  = rect.w / columns;
        grid_height = rect.h / rows;
    }
    std::pair<int, int> get_grid() { return {rows, columns}; }

    LayerComponent* parent;
    std::vector<LayerComponent*> children;
    std::vector<GuiComponent> components;
    Vector2D pos;
    SDL_FRect rect;
    int rows;
    int columns;
    float grid_width;
    float grid_height;
};

struct HudSystem{
    HudSystem(const Vector2D& pos, float width, float height)
    :m_hud_layer({pos.x, pos.y, width, height}, 1, 1, nullptr)
    {}

    HudSystem()
    :HudSystem({}, 0, 0) {}

    void update(float deltatime){
        for(auto& comp: m_hud_layer.components){
            comp.update(deltatime);
        }
    }

    LayerComponent m_hud_layer;
};
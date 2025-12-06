#include "level.h"

std::optional<SDL_FPoint> Level::get_tile_position(const SDL_FPoint& pos){
    std::optional<SDL_FPoint> res;
    res = std::nullopt;

    int row = -1;
    int column = -1;
    if(pos.x >= m_pos.x && pos.x <= m_pos.x + m_level_width){

        column = static_cast<int>((pos.x - m_pos.x) / m_tile_width);
    }
    if(pos.y >= m_pos.y && pos.y <= m_pos.y + m_level_height){

        row = static_cast<int>((pos.y - m_pos.y) / m_tile_height);
    }
    if(row >= 0 && column >= 0)
    {
        auto tile = get_tile(row, column);
        std::cout << "r" << tile.row << " c" << tile.column << std::endl;
        res = {tile.pos.x, tile.pos.y};
    }

    return res;
}

TileComponent& Level::get_tile(int row, int column){
    for(auto& tile: m_tiles){
        if(tile.row == row && tile.column == column){
            return tile;
        }
    }
}

std::optional<TileComponent> Level::get_tile(const SDL_FPoint& pos){
    int row = -1;
    int column = -1;
    if(pos.x >= m_pos.x && pos.x <= m_pos.x + m_level_width){

        column = static_cast<int>((pos.x - m_pos.x) / m_tile_width);
    }
    if(pos.y >= m_pos.y && pos.y <= m_pos.y + m_level_height){

        row = static_cast<int>((pos.y - m_pos.y) / m_tile_height);
    }
    if(row >= 0 && column >= 0)
    {
        return get_tile(row, column);
    }
    return std::nullopt;
}

bool Level::set_tile_occupied(int row, int column, int state){
    TileComponent& tile = get_tile(row, column);
    tile.occupied = state;
    return true;
}
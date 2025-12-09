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
        res = {tile.pos.x, tile.pos.y};
    }

    return res;
}

TileComponent Level::get_tile(int row, int column){
    for(auto tile: m_tiles){
        if(tile.row == row && tile.column == column){
            return tile;
        }
    }
    return {};
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

bool Level::is_road_tile(int row, int column){
    for(auto road_tile: m_road_tiles){
        if(row == road_tile.row && column == road_tile.column)
            return true;
    }
    return false;
}

bool Level::is_road_tile(const SDL_FPoint& pos){
    for(auto road_tile: m_road_tiles){
        if(SDL_PointInRectFloat(&pos, &road_tile.collide_rect)){
            return true;
        }
    }
    return false;
}

bool Level::road_tile_has_dir(int row, int column){
    for(auto road_tile: m_road_tiles){
        if(row == road_tile.row && column == road_tile.column && road_tile.dir.x != road_tile.dir.y)
            return true;
    }
    return false;
}

bool Level::set_tile_occupied(int row, int column, int state){
    int index = column + row * m_columns;
    m_tiles[index].occupied = state;
    // TileComponent& tile = get_tile(row, column);
    // tile.occupied = state;
    return true;
}

bool Level::set_tile_decor_occupied(int row, int column, int state){
    int index = column + row * m_columns;
    m_tiles[index].decor_occupied = state;
    return true;
}

void Level::calc_road_directions(){
    std::vector<std::pair<float, float>> neighbours = {{0, 1}, {0, -1}, {1, 0}, {-1, 0},};

    for(auto it = m_road_tiles.rbegin(); it != m_road_tiles.rend(); ++it){
        for(auto neigh: neighbours){
            int row    = neigh.second + it->row;
            int column = neigh.first + it->column;
            if(is_road_tile(row, column) && !road_tile_has_dir(row, column)){
                it->dir = Vector2D(neigh.first, neigh.second);
                break;
            }
        }
    }
    return;
}

RoadTileComponent& Level::get_road_tile(int row, int column){
    for(auto& tile: m_road_tiles){
        if(tile.row == row && tile.column == column){
            return tile;
        }
    }
}

Vector2D Level::get_dir(Vector2D pos){
    auto tile_opt = get_tile({pos.x, pos.y});
    if(tile_opt.has_value())
    {
        auto tile = tile_opt.value();
        int row = tile.row;
        int column = tile.column;
        if(is_road_tile(row, column)){
            auto road_tile = get_road_tile(row, column);

            return road_tile.dir;
        }
    }
    return {};
}

Vector2D Level::get_tile_center(Vector2D pos){
    auto tile_opt = get_tile({pos.x, pos.y});
    if(tile_opt.has_value())
    {
        return {tile_opt.value().pos.x + m_tile_width / 2, tile_opt.value().pos.y + m_tile_height / 2};
    }
    return {};
}

bool Level::is_pos_in_castle (Vector2D pos){
    SDL_FPoint point = {pos.x, pos.y};
    auto tile = get_castle_tile();
    return SDL_PointInRectFloat(&point, &tile.collide_rect);
}

Vector2D Level::get_tile_position(int row, int column){
    auto tile = get_tile(row, column);
    return {tile.pos.x, tile.pos.y};
}

bool Level::is_occupied(const Vector2D& pos){
    auto tile = get_tile(pos.get_sdl_point());
    if(tile.has_value())
        return tile.value().occupied;
    SDL_Log("WARNING: pos (%d, %d) not in map", pos.x, pos.y);
    return true;
}

bool Level::is_decor_occupied(const Vector2D& pos){
    auto tile = get_tile(pos.get_sdl_point());
    if(tile.has_value())
        return tile.value().decor_occupied;
    SDL_Log("WARNING: pos (%d, %d) not in map", pos.x, pos.y);
    return true;
}
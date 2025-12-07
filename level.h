#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <SDL3/SDL.h>
#include <optional>
#include "vector2d.h"

struct TileComponent{
    TileComponent()
    :row(0)
    ,column(0)
    ,token()
    ,pos()
    ,occupied(0)
    {}

    TileComponent(size_t _row, size_t _column, const SDL_FPoint& _pos, const std::string& _token)
    :row(_row)
    ,column(_column)
    ,token(_token)
    ,pos(_pos)
    ,occupied(0)
    {}

    size_t      row;
    size_t      column;
    std::string token;
    SDL_FPoint  pos;
    int         occupied;

    const static int grass_tile_int   = 0;
    const static int spawner_tile_int = 1;
    const static int road_tile_int    = 2;
    const static int castle_tile_int  = 3;

};

struct RoadTileComponent{
    RoadTileComponent(size_t _row, size_t _column, Vector2D pos)
    :row(_row)
    ,column(_column)
    ,pos(pos)
    ,dir()
    {}

    size_t      row;
    size_t      column;
    Vector2D    pos;
    Vector2D    dir;
};

class Level{
public:
    Level()
    :m_columns(0)
    ,m_rows(0)
    ,m_tile_width(0)
    ,m_tile_height(0)
    ,m_pos()
    ,m_level_width(0)
    ,m_level_height(0)
    {}

    Level(std::string map_file, SDL_FPoint pos_coords, SDL_FPoint size)
    :m_file(map_file)
    ,m_level_width(size.x)
    ,m_level_height(size.y)
    ,m_tile_width(0)
    ,m_tile_height(0)
    {
        m_pos = pos_coords;
        std::fstream f(map_file);
        std::cout << "Map " << map_file << "\n";
        std::string token;
        std::string line;
        std::string value;
        std::getline(f, token);
        auto token_pos = token.find('x');
        if(token_pos == std::string::npos){
            SDL_Log("ERROR: fail to parse .map file");
            return;
        }
        m_rows = std::atoi(token.substr(0, token_pos).c_str());
        m_columns = std::atoi(token.substr(token_pos + 1, token.size() - 1).c_str());
        m_tile_width = m_level_width / m_columns;
        m_tile_height = m_level_height / m_rows;
        std::cout << "Size " << m_rows << "x" << m_columns << "\n";
        int row = 0;
        int castle_index = 0;
        while(std::getline(f, line)){
            std::stringstream ss(line);
            int column = 0;
            while (std::getline(ss, token, ' ')) {
                if (!token.empty()) { // Avoid adding empty strings if there are multiple spaces
                    m_tokens.push_back(token);
                    SDL_FPoint tile_pos = {m_pos.x + column * m_tile_width,
                            m_pos.y + row * m_tile_height};
                    m_tiles.emplace_back(row, column, tile_pos, token);
                    if(atoi(token.c_str()) == TileComponent::castle_tile_int){
                        castle_index = column + row * m_columns;
                    }
                }
                column++;
            }
            row++;
        }

        std::vector<std::pair<int, int>> neighbours = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        int cur_index = castle_index;
        while(atoi(m_tokens[cur_index].c_str()) != TileComponent::spawner_tile_int){
            for(auto neigh: neighbours){

                int cur_row = cur_index / m_columns;
                int cur_column = cur_index % m_columns;

                int row = cur_index / m_columns + neigh.second;
                int column = cur_index % m_columns+ neigh.first;
                auto token_val = atoi(m_tokens[column + row * m_columns].c_str());
                if( (token_val == TileComponent::spawner_tile_int ||
                     token_val == TileComponent::road_tile_int)){
                    if(is_road_tile(row, column)) continue;


                    

                    Vector2D pos = {m_pos.x + cur_column * m_tile_width,
                                    m_pos.y + cur_row * m_tile_height};
                    m_road_tiles.emplace_back(cur_row, cur_column, pos);
                    set_tile_occupied(cur_row, cur_column, 1);
                    cur_index = column + row * m_columns;
                    break;
                }
            }

        }
        
        calc_road_directions();
    }

    std::pair<size_t, size_t> get_resolution() const { return {m_rows, m_columns}; }
    const std::vector<std::string>& get_tokens() const { return m_tokens; }
    std::optional<SDL_FPoint> get_tile_position(const SDL_FPoint& position);
    TileComponent get_tile(int row, int column);
    std::optional<TileComponent> get_tile(const SDL_FPoint& position);
    const SDL_FPoint get_size() const { return {m_level_width, m_level_height};}
    const SDL_FPoint& get_position() const { return m_pos;}
    const SDL_FPoint get_tile_size() const { return {m_tile_width, m_tile_height}; }

    bool set_tile_occupied(int row, int column, int state);
    void calc_road_directions();
    bool is_road_tile(int row, int column);
    bool road_tile_has_dir(int row, int column);
    std::vector<RoadTileComponent> get_road_tiles() const { return m_road_tiles; }
    Vector2D get_dir(Vector2D pos);
    RoadTileComponent& get_road_tile(int row, int column);
    Vector2D get_tile_center(Vector2D pos);
private:
    std::vector<std::string> m_tokens;
    size_t                   m_columns;   
    size_t                   m_rows;   
    float                   m_tile_width;
    float                   m_tile_height;
    float                   m_level_width;
    float                   m_level_height;
    SDL_FPoint               m_pos;
    std::vector<TileComponent> m_tiles;
    std::vector<RoadTileComponent> m_road_tiles;
    std::string              m_file;
};
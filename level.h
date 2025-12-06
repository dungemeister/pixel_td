#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <SDL3/SDL.h>
#include <optional>

struct TileComponent{
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
        size_t row = 0;
        while(std::getline(f, line)){
            std::stringstream ss(line);
            size_t column = 0;
            while (std::getline(ss, token, ' ')) {
                if (!token.empty()) { // Avoid adding empty strings if there are multiple spaces
                    m_tokens.push_back(token);
                    SDL_FPoint tile_pos = {m_pos.x + column * m_tile_width,
                                           m_pos.y + row * m_tile_height};
                    m_tiles.emplace_back(row, column, tile_pos, token);
                    column++;
                }
            }
            row++;
        }
    }

    std::pair<size_t, size_t> get_resolution() const { return {m_rows, m_columns}; }
    const std::vector<std::string>& get_tokens() const { return m_tokens; }
    std::optional<SDL_FPoint> get_tile_position(const SDL_FPoint& position);
    TileComponent& get_tile(int row, int column);
    std::optional<TileComponent> get_tile(const SDL_FPoint& position);
    const SDL_FPoint get_size() const { return {m_level_width, m_level_height};}
    const SDL_FPoint& get_position() const { return m_pos;}
    const SDL_FPoint get_tile_size() const { return {m_tile_width, m_tile_height}; }

    bool set_tile_occupied(int row, int column, int state);
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
    std::string              m_file;
};
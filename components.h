#pragma once
#include <vector>
#include <string>
#include <math.h>
#include "misc.h"
#include <algorithm>
#include <iostream>
#include <SDL3/SDL.h>
#include "vector2d.h"

enum SpriteFlag{
    fUpperLeftSprite,
    fCenterSprite,
};

typedef struct
{
    float posX, posY;
    float width, height;
    float scale;
    float colR, colG, colB;
    float angle;
    int sprite;
    int border;
    int flag;
} sprite_data_t;

struct PositionComponent{
    float x,y;
    float angle;

    Vector2D get_vector2d() { return {x, y}; }
};
struct SpriteComponent{
    float r, g, b;
    float scale;
    float width, height;
    int index;
};

struct MoveComponent{
    float speed;
    float rotation_angle;
    Vector2D target;
    int targeted;
    void Initialize(float minSpeed, float maxSpeed)
    {
        // random angle
        float angle = RandomFloat01() * 3.1415926f * 2;
        // random movement speed between given min & max
        float speed = RandomFloat(minSpeed, maxSpeed);
        // // velocity x & y components
        // velX = cosf(angle) * speed;
        // velY = sinf(angle) * speed;
    }

};

struct BorderComponent{
    float x_min, x_max;
    float y_min, y_max;
    
};

typedef int EntityFlag_t;
typedef int EntityType_t;

enum EntityType{
    fTile       = 1 << 0,
    fRoad       = 1 << 1,
    fTower      = 1 << 2,
    fEnemy      = 1 << 3,
    fSpawner    = 1 << 4,
    fCastle     = 1 << 5,
    fTarget     = 1 << 6,
};

enum EntityFlag{
    fEntityPosition      = 1 << 0,
    fEntityMove          = 1 << 1,
    fEntitySprite        = 1 << 2,
    fEntityMapBorder     = 1 << 3,
    fEntitySpriteBatch   = 1 << 4,
    fEntitySpriteBorder  = 1 << 5,
};

typedef size_t EntityID;

struct Entities{
    std::vector<std::string>        m_names;
    std::vector<PositionComponent>  m_positions;
    std::vector<MoveComponent>      m_moves;
    std::vector<SpriteComponent>    m_sprites;
    std::vector<BorderComponent>    m_borders;
    std::vector<EntityFlag_t>       m_flags;
    std::vector<EntityType_t>       m_types;

    void reserve(size_t n){
        m_positions.reserve(n);
        m_names.reserve(n);
        m_moves.reserve(n);
        m_sprites.reserve(n);
        m_flags.reserve(n);
        m_borders.reserve(n);
        m_types.reserve(n);
    }
    size_t size() { return m_names.size(); }

    EntityID add_object(const std::string&& name){
        
        EntityID id = m_names.size();
        m_names.emplace_back(name);
        m_positions.push_back(PositionComponent());
        m_moves.push_back(MoveComponent());
        m_sprites.push_back(SpriteComponent());
        m_flags.push_back(0);
        m_borders.push_back(BorderComponent());
        m_types.push_back(0);

        return id;
    }
    void remove_object(const std::string& name){
        auto it = std::find(m_names.begin(), m_names.end(), name);
        if(it != m_names.end()){
            size_t id = it - m_names.begin();
            std::cout << "Found '" << name << "' at ID " << id << std::endl;
            remove_object(id);
        }
    }

    void remove_object(int id){
        if(id < size()){
            m_names.erase(m_names.begin() + id);
            m_positions.erase(m_positions.begin() + id);
            m_moves.erase(m_moves.begin() + id);
            m_sprites.erase(m_sprites.begin() + id);
            m_flags.erase(m_flags.begin() + id);
            m_borders.erase(m_borders.begin() + id);
            m_types.erase(m_types.begin() + id);
        }
    }
};

enum TowerType{
    RocketTower = 0,
};

struct MapComponent{
    std::pair<size_t, size_t> resolution;
    
};
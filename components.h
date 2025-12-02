#pragma once
#include <vector>
#include <string>
#include <math.h>
#include "misc.h"

typedef struct
{
    float posX, posY;
    float width, height;
    float scale;
    float colR, colG, colB;
    float angle;
    int sprite;
} sprite_data_t;

struct PositionComponent{
    float x,y;
    float angle;
};
struct SpriteComponent{
    float r, g, b;
    float scale;
    float width, height;
    int index;
};

struct MoveComponent{
    float velX, velY;
    float rotation_angle;
    void Initialize(float minSpeed, float maxSpeed)
    {
        // random angle
        float angle = RandomFloat01() * 3.1415926f * 2;
        // random movement speed between given min & max
        float speed = RandomFloat(minSpeed, maxSpeed);
        // velocity x & y components
        velX = cosf(angle) * speed;
        velY = sinf(angle) * speed;
    }
};

struct BorderComponent{
    float x_min, x_max;
    float y_min, y_max;
    
};

enum EntityFlag{
    fEntityPosition      = 1 << 0,
    fEntityMove          = 1 << 1,
    fEntitySprite        = 1 << 2,
    fEntityBorder        = 1 << 3,
    fEntitySpriteBatch   = 1 << 4,
};

typedef size_t EntityID;

struct Entities{
    std::vector<std::string>        m_names;
    std::vector<PositionComponent>  m_positions;
    std::vector<MoveComponent>      m_moves;
    std::vector<SpriteComponent>    m_sprites;
    std::vector<int>                m_flags;
    std::vector<BorderComponent>    m_borders;

    void reserve(size_t n){
        m_positions.reserve(n);
        m_names.reserve(n);
        m_moves.reserve(n);
        m_sprites.reserve(n);
        m_flags.reserve(n);
        m_borders.reserve(n);
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

        return id;
    }
};
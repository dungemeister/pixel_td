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
    fUpperLeftSprite = 1 << 0,
    fCenterSprite    = 1 << 1,
    fSpriteContour   = 1 << 2,
    fSpriteStencil   = 1 << 3,
    fSpriteBorder    = 1 << 4,
};

enum SpriteLayer{
    BACKGROUND,
    DECORATION,
    ENTITY,
};

struct PositionComponent{
    float x,y;
    float angle;

    Vector2D get_vector2d() { return {x, y}; }
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

enum EntityType{
    UNDEFINED  = 0,
    //BACKGROUND LAYER
    TILE       ,
    ROAD       ,
    SPAWNER    ,
    CASTLE     ,
    //DECORATION LAYER
    CASTLE_DECOR,
    SPAWNER_DECOR,
    BUSH0_DECOR,
    BUSH1_DECOR,
    BUSH2_DECOR,
    //ENTITY LAYER
    TOWER      ,
    ENEMY      ,
    TARGET     ,
    //EFFECTS LAYER
};

typedef int EntitySystems_t;
enum EntitySystems{
    eDummySystem         = 1 << 0,
    ePositionSystem      = 1 << 1,
    eMoveSystem          = 1 << 2,
    eSpriteSystem        = 1 << 3,
    eMapBorderSystem     = 1 << 4,
    eSpriteBatchSystem   = 1 << 5,
};

struct SpriteComponent
{
    float posX, posY;
    float width, height;
    float scale;
    float colR, colG, colB;
    float angle;
    int flag;
    SpriteLayer layer;
    EntityType  type;
};

typedef size_t EntityID;

struct Entities{
    std::vector<std::string>        m_names;
    std::vector<PositionComponent>  m_positions;
    std::vector<MoveComponent>      m_moves;
    std::vector<SpriteComponent>    m_sprites;
    std::vector<BorderComponent>    m_borders;
    std::vector<EntitySystems_t>      m_systems;
    std::vector<EntityType>         m_types;

    void reserve(size_t n){
        m_positions.reserve(n);
        m_names.reserve(n);
        m_moves.reserve(n);
        m_sprites.reserve(n);
        m_systems.reserve(n);
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
        m_systems.push_back(EntitySystems::eDummySystem);
        m_borders.push_back(BorderComponent());
        m_types.push_back(EntityType::UNDEFINED);

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
            m_systems.erase(m_systems.begin() + id);
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

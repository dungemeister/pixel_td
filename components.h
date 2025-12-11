#pragma once
#include <vector>
#include <string>
#include <math.h>
#include "misc.h"
#include <algorithm>
#include <iostream>
#include <SDL3/SDL.h>
#include "vector2d.h"
#include "level.h"

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
    ICE_TOWER  ,
    FIRE_TOWER  ,
    ENEMY      ,
    TARGET     ,
    //EFFECTS LAYER
};

typedef int EntitySystems_t;
enum EntitySystems{
    eDummySystem             = 1 << 0,
    ePositionSystem          = 1 << 1,
    eMoveSystem              = 1 << 2,
    eSpriteSystem            = 1 << 3,
    eMapBorderSystem         = 1 << 4,
    eSpriteBatchSystem       = 1 << 5,
    eSpriteAnimationSystem   = 1 << 6,
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
    int anim_index;
};

enum TowerType{
    RocketTower = 0,
};

struct MapComponent{
    std::pair<size_t, size_t> resolution;
    
};

struct AnimationComponent{
    float  cur_frame;
    size_t frames_size;
    size_t cur_index;
    float  fps;

};

typedef size_t EntityID;

struct Entities{
    std::vector<std::string>        m_names;
    std::vector<PositionComponent>  m_positions;
    std::vector<MoveComponent>      m_moves;
    std::vector<SpriteComponent>    m_sprites;
    std::vector<BorderComponent>    m_borders;
    std::vector<AnimationComponent> m_animations;
    std::vector<EntitySystems_t>    m_systems;
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
        m_animations.push_back(AnimationComponent());
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
            m_animations.erase(m_animations.begin() + id);
        }
    }

    EntityID spawn_enemies_targeted(const Level& level, const Vector2D& target, const SDL_FPoint& spawn_pos){
        auto id = add_object("enemy");
        auto tile_size = level.get_tile_size();
        auto path      = level.get_road_tiles();

        m_positions[id].angle = 0;
        m_positions[id].x = spawn_pos.x;
        m_positions[id].y = spawn_pos.y;
        m_systems[id] |= ePositionSystem;

        m_sprites[id].posX = spawn_pos.x;
        m_sprites[id].posY = spawn_pos.y;
        m_sprites[id].width = tile_size.x;
        m_sprites[id].height = tile_size.y;
        m_sprites[id].scale = 0.5;
        m_sprites[id].colR = 0.6;
        m_sprites[id].colG = 0.6;
        m_sprites[id].colB = 0.6;
        m_sprites[id].angle = 0;
        m_sprites[id].flag = fCenterSprite;
        m_sprites[id].layer = SpriteLayer::ENTITY;
        m_sprites[id].type = EntityType::ENEMY;
        m_sprites[id].anim_index = -1;
        m_systems[id] |= eSpriteSystem;

        m_borders[id].x_min = 0;
        m_borders[id].x_max = 0;
        // objects.m_borders[id].y_min = (tile.pos.y - tile_size.y) / 2;
        m_borders[id].y_min = (spawn_pos.y - tile_size.y / 4);
        m_borders[id].y_max = (spawn_pos.y + tile_size.y / 4);
        // objects.m_flags[id] |= fEntityMapBorder;

        // objects.m_moves[id].speed = RandomFloat(10., 20.);
        m_moves[id].speed = 10.f;
        m_moves[id].targeted = 1;
        m_moves[id].target = target;

        // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
        m_systems[id] |= eMoveSystem;

        m_types[id] = EntityType::ENEMY;

        return id;
    }

    size_t get_objects_size(EntityType type){
        size_t size = 0;
        for(int i = 0 ; i < Entities::size(); i++){
            if(m_types[i] == type)
            size++;
        }
        return size;
    }
    
    EntityID add_tower(Level& level, EntityType type, const Vector2D& pos){
        auto tile_opt = level.get_tile(pos.get_sdl_point());
        if(!level.is_road_tile(pos.get_sdl_point()) && tile_opt.has_value())
        {
            auto tile = tile_opt.value();
            auto id = add_object("tower");
            auto tile_size = level.get_tile_size();
            m_positions[id].angle = 90;
            m_positions[id].x = tile.pos.x;
            m_positions[id].y = tile.pos.y;
            m_systems[id] |= ePositionSystem;

            m_sprites[id].posX = tile.pos.x;
            m_sprites[id].posY = tile.pos.y - tile_size.y * 0.5;
            m_sprites[id].width = tile_size.x;
            m_sprites[id].height = tile_size.y * 1.2;
            m_sprites[id].scale = 1.2;
            m_sprites[id].colR = 0.6;
            m_sprites[id].colG = 0.6;
            m_sprites[id].colB = 0.6;
            m_sprites[id].angle = 0;
            m_sprites[id].flag = fUpperLeftSprite;
            m_sprites[id].layer = SpriteLayer::ENTITY;
            m_sprites[id].type = type;
            m_sprites[id].anim_index = -1;
            m_systems[id] |= eSpriteSystem;

            if(type == EntityType::FIRE_TOWER){
                m_animations[id].cur_frame = 0.f;
                m_animations[id].frames_size = 6;
                m_animations[id].fps = 0.8;
                m_systems[id] |= eSpriteAnimationSystem;
            }
            m_types[id] = FIRE_TOWER;

            level.set_tile_occupied(tile.row, tile.column, 1);
            return id;

        }
        return -1;
    }
};


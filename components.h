#pragma once
#include <vector>
#include <string>
#include <math.h>
#include "misc.h"
#include <algorithm>
#include <iostream>
#include <SDL3/SDL.h>
#include <unordered_map>
#include "vector2d.h"
#include "level.h"

typedef size_t EntityID;

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
    PROJECTILES,
};

struct PositionComponent{
    float x,y;
    float angle;

    Vector2D get_vector2d() { return {x, y}; }
    SDL_FPoint get_sdl_fpoint() { return {x, y}; }
};

struct VersionComponent{
    size_t version;
    bool operator==(const VersionComponent& other) { return other.version == version; }
};

struct MoveComponent{
    float speed;
    float rotation_angle;
    int targeted;
    Vector2D target;
    EntityID target_id;
    VersionComponent target_version;
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

enum EntityGlobalType{
    UNKNOWN_ENTITY,
    BACKGROUND_ENTITY,
    DECORATION_ENTITY,
    ENEMY_ENTITY,
    FRIEND_ENTITY,
    PROJECTILE_ENTITY,
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
    //ENTITY LAYER.TOWERS
    TOWER      ,
    ICE_TOWER  ,
    FIRE_TOWER  ,
    //ENTITY LAYER.ENEMIES
    VIKING,
    DRAGONIT,
    BEE,
    SERJANT,
    TANK,
    TARGET,
    //FIRING LAYER
    PROJECTILE,
    AOE,
    //EFFECTS LAYER
    HEARTH,
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
    eFiringSystem            = 1 << 7,
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

enum FiringType{
    eProjectile,
    eAOE,
    eIntantSpell,
};

struct FiringComponent{
    float interval;
    float cooldown;
    float radius;
    FiringType type;
};

struct HealthComponent{
    float health;
    int alive;
};



struct Entities{
Entities() {
    reserve(100);
    m_empty_id = add_object("empty");
    }
    
    EntityID m_empty_id;

    std::vector<std::string>        m_names;
    std::vector<PositionComponent>  m_positions;
    std::vector<MoveComponent>      m_moves;
    std::vector<SpriteComponent>    m_sprites;
    std::vector<BorderComponent>    m_borders;
    std::vector<AnimationComponent> m_animations;
    std::vector<FiringComponent>    m_firings;
    std::vector<HealthComponent>    m_health;
    std::vector<VersionComponent>   m_versions;
    std::vector<EntitySystems_t>    m_systems;
    std::vector<EntityGlobalType>   m_types;

    EntityID get_empty_id() { return m_empty_id; }

    void reserve(size_t n){
        m_names.reserve(n);
        m_positions.reserve(n);
        m_moves.reserve(n);
        m_sprites.reserve(n);
        m_borders.reserve(n);
        m_animations.reserve(n);
        m_firings.reserve(n);
        m_health.reserve(n);
        m_versions.reserve(n);
        m_systems.reserve(n);
        m_types.reserve(n);

    }
    size_t size() { return m_names.size(); }

    EntityID add_object(const std::string&& name){
        // SDL_Log("Added %s", name.c_str());
        for(int id = 0, n = m_names.size(); id < n; id++)
        {
            if(m_health[id].alive) continue;

            reset_object(id);
            m_health[id].alive = 1;
            m_names[id] = name;
            return id;

        }
        return add_new_object(name);
    }

    EntityID add_new_object(const std::string& name){
        EntityID id = m_names.size();
        m_names.emplace_back(name);
        m_positions.push_back(PositionComponent());
        m_moves.push_back(MoveComponent());
        m_sprites.push_back(SpriteComponent());
        m_systems.push_back(EntitySystems::eDummySystem);
        m_borders.push_back(BorderComponent());
        m_types.push_back(EntityGlobalType::UNKNOWN_ENTITY);
        m_animations.push_back(AnimationComponent());
        m_firings.push_back(FiringComponent());
        m_health.push_back(HealthComponent());
        m_versions.push_back(VersionComponent());

        m_health.back().alive = 1;
        return id;
    }

    void reset_object(EntityID id){
        // SDL_Log("Reseted Name: %s ID: %lu, Type: %d, Version: %d", m_names[id].c_str(), id, m_types[id], m_versions[id]);
        m_names[id].clear();
        m_positions[id] = PositionComponent();
        m_moves[id] = MoveComponent();
        m_sprites[id] = SpriteComponent();
        m_systems[id] = EntitySystems::eDummySystem;
        m_borders[id] = BorderComponent();
        m_types[id] = EntityGlobalType::UNKNOWN_ENTITY;
        m_animations[id] = AnimationComponent();
        m_firings[id] = FiringComponent();
        m_health[id].alive = 0;
        m_versions[id].version += 1;
    }

    void remove_object(const std::string& name){
        auto it = std::find(m_names.begin(), m_names.end(), name);
        if(it != m_names.end()){
            size_t id = it - m_names.begin();
            std::cout << "Found '" << name << "' at ID " << id << std::endl;
            remove_object(id);
        }
    }

    bool is_alive(EntityID id){
        return m_health[id].alive == 1;
    }

    VersionComponent get_version_component(EntityID id){
        return m_versions[id];
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
            m_firings.erase(m_firings.begin() + id);
            m_health.erase(m_health.begin() + id);
            m_versions.erase(m_versions.begin() + id);
            for(auto it = m_versions.begin() + id; it != m_versions.end(); ++it){
                it->version++;
            }
        }
    }

    void set_object_alive_state(EntityID id, int state){
        m_health[id].alive = state;
    }

    EntityID spawn_enemies_targeted(const Level& level, const Vector2D& target, const SDL_FPoint& spawn_pos, EntityType type){
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
        m_sprites[id].colR = 0.6;
        m_sprites[id].colG = 0.6;
        m_sprites[id].colB = 0.6;
        m_sprites[id].angle = 0;
        m_sprites[id].flag = fCenterSprite;
        m_sprites[id].layer = SpriteLayer::ENTITY;
        m_sprites[id].type = type;
        m_sprites[id].anim_index = -1;
        m_systems[id] |= eSpriteSystem;
        switch(type){
            case EntityType::VIKING:
            case EntityType::BEE:
                m_sprites[id].scale = 0.5;
            break;
            case EntityType::DRAGONIT:
                m_sprites[id].scale = 0.7;
            break;
            case EntityType::SERJANT:
                m_sprites[id].scale = 0.9;
            break;
            case EntityType::TANK:
                m_sprites[id].scale = 1.1;
            break;
            default:
            SDL_Log("WARNING: Unexpected enemy type %d", type);
        }

        m_borders[id].x_min = 0;
        m_borders[id].x_max = 0;
        // objects.m_borders[id].y_min = (tile.pos.y - tile_size.y) / 2;
        m_borders[id].y_min = (spawn_pos.y - tile_size.y / 4);
        m_borders[id].y_max = (spawn_pos.y + tile_size.y / 4);
        // objects.m_flags[id] |= fEntityMapBorder;

        // objects.m_moves[id].speed = RandomFloat(10., 20.);
        m_moves[id].speed = 100.f;
        m_moves[id].targeted = 1;
        m_moves[id].target = target;
        
        // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
        m_systems[id] |= eMoveSystem;

        m_types[id] = EntityGlobalType::ENEMY_ENTITY;

        return id;
    }

    size_t get_objects_size(EntityGlobalType type){
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
                m_animations[id].fps = 8;

                m_firings[id].interval = 1.f;
                m_firings[id].cooldown = m_firings[id].interval;
                m_firings[id].type = FiringType::eProjectile;
                m_firings[id].radius = 150.f;

                m_systems[id] |= eSpriteAnimationSystem | eFiringSystem;
            }
            m_types[id] = EntityGlobalType::FRIEND_ENTITY;

            level.set_tile_occupied(tile.row, tile.column, 1);
            return id;

        }
        return -1;
    }

    EntityID get_nearest_enemy(const Vector2D& tower_pos, float radius){
        float length = MAXFLOAT;
        EntityID res = get_empty_id();
        for(int id = 1, n = Entities::size(); id < n; id++){
            
            if(m_types[id] == EntityGlobalType::ENEMY_ENTITY){
                auto diff = (m_positions[id].get_vector2d() - tower_pos).magnitude();
                if(diff < length && diff < radius){
                    length = diff;
                    res = id;
                }
            }

        }
        return res;
    }

    EntityID get_nearest_enemy_to_point(const Vector2D& target){
        float length = MAXFLOAT;
        EntityID res = get_empty_id();
        for(int id = 1, n = Entities::size(); id < n; id++){
            
            if(m_types[id] == EntityGlobalType::ENEMY_ENTITY){
                auto diff = (m_positions[id].get_vector2d() - target).magnitude();
                if(diff < length){
                    length = diff;
                    res = id;
                }
            }

        }
        return res;
    }

    std::vector<EntityID> get_enemies_in_radius(const Vector2D& pos, float radius){
        std::vector<EntityID> res;
        for(int id = 1, n = Entities::size(); id < n; id++){
            
            if(m_types[id] == EntityGlobalType::ENEMY_ENTITY){
                auto diff = (m_positions[id].get_vector2d() - pos).magnitude();
                if(diff <= radius){
                    res.push_back(id);
                }
            }
        }
        return {get_empty_id()};
    }

    EntityID get_nearest_enemy_to_point_in_radius(const Vector2D& pos, const Vector2D& target, float radius){
        float length = MAXFLOAT;
        EntityID res = get_empty_id();
        float diff_to_target = 0.f;
        std::unordered_map<EntityID, float> targets_in_radius;
        targets_in_radius.reserve(32);

        for(int id = 1, n = Entities::size(); id < n; id++){
            
            if(m_types[id] == EntityGlobalType::ENEMY_ENTITY){
                auto diff_radius = (m_positions[id].get_vector2d() - pos).magnitude();
                if(diff_radius <= radius){
                    diff_to_target = (m_positions[id].get_vector2d() - target).magnitude();
                    targets_in_radius.emplace(id, diff_to_target);
                    if(diff_to_target < length){
                        length = diff_to_target;
                        res = id;
                    }
                }
            }

        }

        return res;
    }
    EntityID add_projectile(FiringType type, const SDL_FRect& rect, EntityID target_id){
        auto id = add_object("projectile");

        m_positions[id].x = rect.x;
        m_positions[id].y = rect.y;
        m_systems[id] |= ePositionSystem;

        m_sprites[id].posX = rect.x;
        m_sprites[id].posY = rect.y;
        m_sprites[id].width = rect.w;
        m_sprites[id].height = rect.h;
        m_sprites[id].scale = 1.2;
        m_sprites[id].colR = 0.6;
        m_sprites[id].colG = 0.6;
        m_sprites[id].colB = 0.6;
        m_sprites[id].angle = 0;
        m_sprites[id].flag = fCenterSprite;
        m_sprites[id].layer = SpriteLayer::ENTITY;
        m_sprites[id].type = EntityType::PROJECTILE;
        m_sprites[id].anim_index = -1;
        m_systems[id] |= eSpriteSystem;

        m_moves[id].targeted = 1;
        m_moves[id].target_id = target_id;
        m_moves[id].target = m_positions[target_id].get_vector2d();
        m_moves[id].target_version = m_versions[target_id];
        m_moves[id].rotation_angle = (m_moves[id].target - Vector2D(rect.x, rect.y)).angle();
        // SDL_Log("Proj angle %f", m_moves[id].rotation_angle);
        m_moves[id].speed = 200.f;
        m_systems[id] |= eMoveSystem;

        m_types[id] = EntityGlobalType::PROJECTILE_ENTITY;

        return id;
    }

    EntityID get_object(EntityType type){
        for(int id = get_empty_id() + 1, n = Entities::size(); id < n; id++){
            
            if(m_sprites[id].type == type){
                return id;
            }

        }
        return get_empty_id();
    }
};


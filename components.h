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
#include <variant>

typedef size_t EntityID;

enum SpriteFlag{
    fUpperLeftSprite   = 1 << 0,
    fCenterSprite      = 1 << 1,
    fSpriteContour     = 1 << 2,
    fSpriteStencil     = 1 << 3,
    fSpriteBorder      = 1 << 4,
    fSpriteHealthBar   = 1 << 5,
    fSpriteTone        = 1 << 6,
    fSpriteCooldownBar = 1 << 7,
};

enum SpriteLayer{
    //GamePlay layers
    BACKGROUND,
    DECORATION,
    ENTITY,
    PROJECTILES,
    HUD,
    //Menu layers
    PAUSE_MENU,
    SETTINGS,
    MAIN_MENU,
};

struct PositionComponent{
    PositionComponent()
    :x(0.f)
    ,y(0.f)
    ,angle(0.f)
    ,collision_radius(0.f) {}

    float x,y;
    float angle;
    float collision_radius;
    Vector2D get_vector2d() { return {x, y}; }
    SDL_FPoint get_sdl_fpoint() { return {x, y}; }
};

struct VersionComponent{
    size_t version;
    bool operator==(const VersionComponent& other) { return other.version == version; }
    bool operator!=(const VersionComponent& other) { return other.version != version; }
};

struct MoveComponent{
    MoveComponent()
    :speed_magnitude(1.f)
    ,speed(0.f)
    ,rotation_angle(0.f)
    ,targeted(0)
    ,target()
    ,target_id(0)
    ,target_version()
    {}

    float speed_magnitude;
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
    HUD_ENTITY,
};

enum SpriteType{
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
    FIRECAMP_DECOR,
    //ENTITY LAYER.TOWERS
    TOWER      ,
    ICE_TOWER  ,
    FIRE_TOWER  ,
    POISON_TOWER,
    CLOUD_TOWER,
    //ENTITY LAYER.ENEMIES
    VIKING_SPRITE,
    DRAGONIT_SPRITE,
    BEE_SPRITE,
    SERJANT_SPRITE,
    TANK_SPRITE,
    TARGET_SPRITE,
    //FIRING LAYER
    FIRE_PROJECTILE,
    ICE_PROJECTILE,
    FIRE_AOE,
    ICE_AOE,
    EXPLOSION_AOE,
    POISON_PROJECTILE,
    POISON_AOE,
    CLOUD_PROJECTILE,
    CLOUD_AOE,
    //EFFECTS LAYER
    HEALTH_BAR,
    //HUD
    HUD_LAYOUT,
    HUD_HEARTH,
    HUD_COINS,
    HUD_PLAYER_GOLD,
    HUD_HEALTH_BAR,
    HUD_TEXT,
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
    eDamageSystem            = 1 << 8,
    eHealthSystem            = 1 << 9,
    eCollisionSystem         = 1 << 10,
};

struct SpriteComponent
{

    float posX, posY; //Position
    float width, height; //Size
    Vector2D center; // Center position
    float scale; //Width & height scale coef
    float colR, colG, colB; //Color blending
    float angle; //rotation angle
    int flag; //Sprite flag
    SpriteLayer layer; //Drawing layer
    SpriteType  type;  //One of the registered sprite types
    int anim_index;    //For animated sprite index of current texture
    Vector2D forward;  //Forward vector of the sprite for rotation calculation 
    std::variant<int, float, std::string> value; //Some value for handling
    float radius;

    void calc_radius(){
        // radius = std::sqrt(scale / 2 * (width * width + height * height)) / 2;
        radius = width * scale / 2;
    }
    void calc_center(){
        center = {posX + width * scale / 2, posY + height * scale / 2};
    }
    void update_pos_from_center(){
        posX = center.x - width * scale / 2;
        posY = center.y - height * scale / 2;
    }
};

enum TowerType{
    FIRE_TOWER_DATA,
    ICE_TOWER_DATA,
    POISON_TOWER_DATA,
    CLOUD_TOWER_DATA,

    TOWER_TYPES_SIZE,
};

enum EnemyType{
    VIKING,
    DRAGONIT,
    BEE,
    SERJANT,
    TANK,

    ENEMY_TYPES_SIZE,
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

struct HealthComponent{
    enum DeadPendingReason{
        //Reasons for enemies
        KILLED    = 1 << 0,
        PASSED    = 1 << 1,
        //Reasons for projectiles
        EXPLOUDED = 1 << 2,
        //Reasons for towers
        SELLED    = 1 << 3,

    };

    float max_health;
    float cur_health;
    float regeneration;
    int alive;
    DeadPendingReason dead_pending;
};

struct TowerDescription{
    float cost;
    float remove_cost;
    float radius;
    float firing_interval;
    TowerType type;
    int level;
    float slowing_magnitude;
    float slowing_time;
    float periodic_damage;
    float periodic_time;
    float burst_damage;
    float projectile_speed;
    FiringType firing_type;
    std::unordered_map<int, float> experience_distribution;

    std::string get_type_string() const {
        switch(type){
            case TowerType::FIRE_TOWER_DATA:
                return "Fire Tower";
            case TowerType::ICE_TOWER_DATA:
                return "Ice Tower";
            case TowerType::POISON_TOWER_DATA:
                return "Poison Tower";
            case TowerType::CLOUD_TOWER_DATA:
                return "Cloud Tower";
            default:
                return "Unknown";
        }
    }
};

struct EnemyDescription{
    float max_health;
    float cur_health;
    float regeneration;
    float damage;
    float speed;
    float bounty;
    EnemyType type;
    float armor;

    std::string get_type_string() const {
        switch(type){
            case EnemyType::VIKING:
                return "Viking";
            case EnemyType::BEE:
                return "Bee";
            case EnemyType::DRAGONIT:
                return "Gragonit";
            case EnemyType::SERJANT:
                return "Serjant";
            case EnemyType::TANK:
                return "Tank";
            default:
                return "Unknown";
        }
    }
};

struct FiringComponent{
    float interval;
    float cooldown;
    float radius;
    TowerDescription* descr;
};

enum BuffType{
    IGNITE,
    SLOW,

    DAMAGE_BOOST,
    REGENERATION,

    COUNT,
};

struct BuffComponent{
    EntityID id;
    VersionComponent version;
    BuffType type;
    float duration;
    float elapsed_time;
    float magnitude;

};

struct BuffTypeInfo{
    const char* name;
    float duration;
};

static constexpr BuffTypeInfo s_buff_info[static_cast<int>(BuffType::COUNT)] = {
    {"Ignite", 5.f},
    {"Slow", 2.f},
    {"Damage Boost", 5.f},
    {"Regeneration", 5.f},
};

struct Entities{
Entities() {
    reserve(100);
    m_empty_id = add_object("empty");
    }
    typedef std::variant<std::monostate, TowerDescription, EnemyDescription> Descriptor;

    EntityID m_empty_id;

    std::vector<std::string>                        m_names;
    std::vector<PositionComponent>                  m_positions;
    std::vector<MoveComponent>                      m_moves;
    std::vector<SpriteComponent>                    m_sprites;
    std::vector<BorderComponent>                    m_borders;
    std::vector<AnimationComponent>                 m_animations;
    std::vector<FiringComponent>                    m_firings;
    std::vector<HealthComponent>                    m_health;
    std::vector<VersionComponent>                   m_versions;
    std::vector<EntitySystems_t>                    m_systems;
    std::vector<EntityGlobalType>                   m_types;
    std::vector<Descriptor>                         m_descriptions;
    
    std::vector<std::variant<std::monostate, EnemyType, TowerType>> m_concrete_types;

    std::unordered_map<TowerType, TowerDescription> m_towers_descr;
    std::unordered_map<EnemyType, EnemyDescription> m_enemies_descr;

    EntityID get_empty_id() const { return m_empty_id; }

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
        m_concrete_types.reserve(n);
        m_descriptions.reserve(n);

    }
    size_t size() const { return m_names.size(); }

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
        m_concrete_types.push_back(std::monostate{});
        m_descriptions.push_back(std::monostate{});
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
        m_concrete_types[id] = std::monostate{};
        m_descriptions[id] = std::monostate{};
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

    void set_object_dead_pending_reason(EntityID id, HealthComponent::DeadPendingReason reason){
        m_health[id].dead_pending = reason;
    }

    EntityID spawn_enemies_targeted(const Level& level, const Vector2D& target, const SDL_FPoint& spawn_pos, EnemyType type){
        auto id = add_object("enemy");
        auto tile_size = level.get_tile_size();
        auto path      = level.get_road_tiles();

        m_sprites[id].posX = spawn_pos.x;
        m_sprites[id].posY = spawn_pos.y;
        m_sprites[id].width = tile_size.x;
        m_sprites[id].height = tile_size.y;
        m_sprites[id].colR = 0.6;
        m_sprites[id].colG = 0.6;
        m_sprites[id].colB = 0.6;
        m_sprites[id].angle = 0;
        m_sprites[id].flag = fCenterSprite | fSpriteTone | fSpriteBorder;
        m_sprites[id].layer = SpriteLayer::ENTITY;
        m_sprites[id].anim_index = -1;
        m_systems[id] |= eSpriteSystem;
        auto enemy_descr = m_enemies_descr.find(type);
        if(enemy_descr == m_enemies_descr.end())
            SDL_Log("Fail to get enemy type %d", type);
        else{
            auto descr = enemy_descr->second;
            m_health[id].max_health = descr.max_health;
            m_health[id].cur_health = descr.cur_health;
            m_health[id].regeneration = descr.regeneration;
            m_sprites[id].flag |= fSpriteHealthBar;
            m_moves[id].speed = descr.speed;
            m_concrete_types[id] = type;
            m_descriptions[id] = EnemyDescription(enemy_descr->second);
            switch(type){
                case EnemyType::VIKING:
                    m_sprites[id].type = SpriteType::VIKING_SPRITE;
                    m_sprites[id].scale = 0.5;
                break;
                case EnemyType::BEE:
                    m_sprites[id].type = SpriteType::BEE_SPRITE;
                    m_sprites[id].scale = 0.5;
                break;
                case EnemyType::DRAGONIT:
                    m_sprites[id].type = SpriteType::DRAGONIT_SPRITE;
                    m_sprites[id].scale = 0.7;
                break;
                case EnemyType::SERJANT:
                    m_sprites[id].type = SpriteType::SERJANT_SPRITE;
                    m_sprites[id].scale = 0.9;

                    m_animations[id].cur_frame = 0.f;
                    m_animations[id].frames_size = 4;
                    m_animations[id].fps = 6;
                    m_systems[id] |= eSpriteAnimationSystem;
                break;
                case EnemyType::TANK:
                    m_sprites[id].type = SpriteType::TANK_SPRITE;
                    m_sprites[id].scale = 1.1;
                break;
                default:
                    SDL_Log("WARNING: Unexpected enemy type %d", type);
                    m_sprites[id].type = SpriteType::UNDEFINED;
                    m_sprites[id].scale = 1.1;
            }
        }
        m_sprites[id].calc_center();
        m_sprites[id].calc_radius();

        m_positions[id].angle = 0;
        m_positions[id].x = spawn_pos.x;
        m_positions[id].y = spawn_pos.y;
        m_positions[id].collision_radius = m_sprites[id].radius / 2;
        m_systems[id] |= ePositionSystem;

        m_borders[id].x_min = 0;
        m_borders[id].x_max = 0;
        // objects.m_borders[id].y_min = (tile.pos.y - tile_size.y) / 2;
        m_borders[id].y_min = (spawn_pos.y - tile_size.y / 4);
        m_borders[id].y_max = (spawn_pos.y + tile_size.y / 4);
        // objects.m_flags[id] |= fEntityMapBorder;

        m_moves[id].targeted = 1;
        m_moves[id].target = target;
        
        // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
        m_systems[id] |= eMoveSystem | eCollisionSystem;

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
    
    EntityID add_tower(Level& level, TowerType type, const Vector2D& pos){
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
            m_sprites[id].calc_center();
            m_sprites[id].calc_radius();
            m_sprites[id].colR = 0.6;
            m_sprites[id].colG = 0.6;
            m_sprites[id].colB = 0.6;
            m_sprites[id].angle = 0;
            m_sprites[id].flag = fUpperLeftSprite | fSpriteCooldownBar | fSpriteBorder;
            m_sprites[id].layer = SpriteLayer::ENTITY;
            m_sprites[id].anim_index = -1;
            m_systems[id] |= eSpriteSystem;
            TowerDescription* tower_descr;
            switch(type){
                case TowerType::FIRE_TOWER_DATA:
                    tower_descr = get_tower_descr(TowerType::FIRE_TOWER_DATA);
                    m_sprites[id].type = SpriteType::FIRE_TOWER;
                    m_animations[id].cur_frame = 0.f;
                    m_animations[id].frames_size = 6;
                    m_animations[id].fps = 8;

                    m_firings[id].interval = tower_descr->firing_interval;
                    m_firings[id].cooldown = m_firings[id].interval;
                    m_firings[id].radius = tower_descr->radius;
                    m_firings[id].descr = tower_descr;

                    m_systems[id] |= eSpriteAnimationSystem | eFiringSystem;

                break;
                case TowerType::ICE_TOWER_DATA:
                    tower_descr = get_tower_descr(TowerType::ICE_TOWER_DATA);
                    m_sprites[id].type = SpriteType::ICE_TOWER;
                    m_firings[id].interval = tower_descr->firing_interval;
                    m_firings[id].cooldown = m_firings[id].interval;
                    m_firings[id].radius = tower_descr->radius;
                    m_firings[id].descr = tower_descr;

                    m_systems[id] |= eFiringSystem;
                break;
                case TowerType::POISON_TOWER_DATA:
                    tower_descr = get_tower_descr(TowerType::POISON_TOWER_DATA);
                    m_sprites[id].type = SpriteType::POISON_TOWER;
                    m_firings[id].interval = tower_descr->firing_interval;
                    m_firings[id].cooldown = m_firings[id].interval;
                    m_firings[id].radius = tower_descr->radius;
                    m_firings[id].descr = tower_descr;

                    m_systems[id] |= eFiringSystem;
                break;
                case TowerType::CLOUD_TOWER_DATA:
                    tower_descr = get_tower_descr(TowerType::CLOUD_TOWER_DATA);
                    m_animations[id].cur_frame = 0.f;
                    m_animations[id].frames_size = 5;
                    m_animations[id].fps = 3;

                    m_sprites[id].type = SpriteType::CLOUD_TOWER;
                    m_firings[id].interval = tower_descr->firing_interval;
                    m_firings[id].cooldown = m_firings[id].interval;
                    m_firings[id].radius = tower_descr->radius;
                    m_firings[id].descr = tower_descr;

                    m_systems[id] |= eFiringSystem;
                break;
            }

            m_sprites[id].value = m_firings[id].cooldown;
            m_descriptions[id] = TowerDescription(*tower_descr);

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
    EntityID add_projectile(TowerDescription* descr, const SDL_FRect& rect, EntityID target_id){
        if(!descr) return get_empty_id();
        
        auto id = add_object("projectile");

        m_positions[id].x = rect.x;
        m_positions[id].y = rect.y;
        m_systems[id] |= ePositionSystem;

        m_sprites[id].posX = rect.x;
        m_sprites[id].posY = rect.y;
        m_sprites[id].width = rect.w;
        m_sprites[id].height = rect.h;
        m_sprites[id].scale = 1.2;
        m_sprites[id].calc_center();
        m_sprites[id].calc_radius();
        m_sprites[id].colR = 0.6;
        m_sprites[id].colG = 0.6;
        m_sprites[id].colB = 0.6;
        m_sprites[id].angle = 0;
        m_sprites[id].flag = fCenterSprite | fSpriteBorder;
        m_sprites[id].layer = SpriteLayer::PROJECTILES;
        m_sprites[id].anim_index = -1;
        m_systems[id] |= eSpriteSystem;

        m_moves[id].targeted = 1;
        m_moves[id].target_id = target_id;
        // m_moves[id].target = m_sprites[target_id].center;
        m_moves[id].target = m_positions[target_id].get_vector2d();
        m_moves[id].target_version = m_versions[target_id];
        m_moves[id].rotation_angle = (m_moves[id].target - Vector2D(rect.x, rect.y)).angle();
        // SDL_Log("Proj angle %f", m_moves[id].rotation_angle);
        switch(descr->type){
            case TowerType::FIRE_TOWER_DATA:
                m_sprites[id].type = SpriteType::FIRE_PROJECTILE;
                m_sprites[id].forward = {-1, 1}; //Sprite forward direction;

            break;
            case TowerType::ICE_TOWER_DATA:
                m_sprites[id].type = SpriteType::ICE_PROJECTILE;
                m_sprites[id].forward = {-0, -1}; //Sprite forward direction;
            break;
            case TowerType::POISON_TOWER_DATA:
                m_sprites[id].type = SpriteType::POISON_PROJECTILE;
                m_sprites[id].forward = {1, 0}; //Sprite forward direction;
                m_sprites[id].scale = 1.2;
            break;
            case TowerType::CLOUD_TOWER_DATA:
                m_sprites[id].type = SpriteType::CLOUD_PROJECTILE;
                m_sprites[id].forward = {1, -1}; //Sprite forward direction;
                m_sprites[id].scale = 1.2;
            break;
            default:
                SDL_Log("WARNING: add projectile from unknown tower type");
        }
        m_firings[id].descr = descr;
        m_moves[id].speed = descr->projectile_speed;
        m_systems[id] |= eMoveSystem | eDamageSystem;

        m_types[id] = EntityGlobalType::PROJECTILE_ENTITY;

        return id;
    }

    EntityID get_object(SpriteType type){
        for(int id = get_empty_id() + 1, n = Entities::size(); id < n; id++){
            
            if(m_sprites[id].type == type){
                return id;
            }

        }
        return get_empty_id();
    }

    TowerDescription create_tower_descr(TowerType type){
        TowerDescription tower;
        switch(type){
            case TowerType::FIRE_TOWER_DATA:
                tower.cost = 10;
                tower.firing_interval = 2.f;
                tower.level = 0;
                tower.radius = 150.f;
                tower.remove_cost = tower.cost * 0.5f;
                tower.type = type;
                tower.slowing_magnitude = 0.f;
                tower.slowing_time = 0;
                tower.periodic_damage = 5.f;
                tower.periodic_time = 3.f;
                tower.burst_damage = 5.f;
                tower.projectile_speed = 200.f;
                tower.firing_type = FiringType::eProjectile;
                tower.experience_distribution.emplace(0, 100.f);
                tower.experience_distribution.emplace(1, 200.f);
                tower.experience_distribution.emplace(2, 400.f);
                tower.experience_distribution.emplace(3, 700.f);
            break;
            case TowerType::ICE_TOWER_DATA:
                tower.cost = 5;
                tower.firing_interval = 1.f;
                tower.level = 0;
                tower.radius = 150.f;
                tower.remove_cost = tower.cost * 0.5f;
                tower.type = type;
                tower.slowing_magnitude = 0.20f;
                tower.slowing_time = 1.f;
                tower.periodic_damage = 2.f;
                tower.periodic_time = 5.f;
                tower.burst_damage = 1.f;
                tower.projectile_speed = 250.f;
                tower.firing_type = FiringType::eProjectile;
                tower.experience_distribution.emplace(0, 150.f);
                tower.experience_distribution.emplace(1, 300.f);
                tower.experience_distribution.emplace(2, 500.f);
                tower.experience_distribution.emplace(3, 700.f);
            break;
            case TowerType::POISON_TOWER_DATA:
                tower.cost = 7;
                tower.firing_interval = 1.f;
                tower.level = 0;
                tower.radius = 200.f;
                tower.remove_cost = tower.cost * 0.5f;
                tower.type = type;
                tower.slowing_magnitude = 0.10f;
                tower.slowing_time = 1.5f;
                tower.periodic_damage = 0.5f;
                tower.periodic_time = 3.f;
                tower.burst_damage = 3.f;
                tower.projectile_speed = 350.f;
                tower.firing_type = FiringType::eProjectile;
                tower.experience_distribution.emplace(0, 150.f);
                tower.experience_distribution.emplace(1, 300.f);
                tower.experience_distribution.emplace(2, 500.f);
                tower.experience_distribution.emplace(3, 700.f);
            break;
            case TowerType::CLOUD_TOWER_DATA:
                tower.cost = 15;
                tower.firing_interval = 2.5f;
                tower.level = 0;
                tower.radius = 250.f;
                tower.remove_cost = tower.cost * 0.5f;
                tower.type = type;
                tower.slowing_magnitude = 0.25f;
                tower.slowing_time = 1.5f;
                tower.periodic_damage = 0.5f;
                tower.periodic_time = 3.f;
                tower.burst_damage = 8.f;
                tower.projectile_speed = 400.f;
                tower.firing_type = FiringType::eProjectile;
                tower.experience_distribution.emplace(0, 150.f);
                tower.experience_distribution.emplace(1, 300.f);
                tower.experience_distribution.emplace(2, 500.f);
                tower.experience_distribution.emplace(3, 700.f);
            break;
            default:
                SDL_Log("WARNING: Registering unknown tower type");
        }
        m_towers_descr.emplace(type, tower);

        return tower;
    }

    TowerDescription* get_tower_descr(TowerType type){
        auto it = m_towers_descr.find(type);
        if(it != m_towers_descr.end()){
            return &it->second;
        }
        return nullptr;
    }
    
    EnemyDescription create_enemy_descr(EnemyType type){
        EnemyDescription enemy;
        enemy.type = type;
        switch(type){
            case EnemyType::VIKING:
                enemy.armor  = 5.f;
                enemy.bounty = 2.f;
                enemy.damage = 2.f;
                enemy.max_health = 3.f;
                enemy.cur_health = 3.f;
                enemy.speed  = 125.f;
            break;
            case EnemyType::BEE:
                enemy.armor  = 2.f;
                enemy.bounty = 1.f;
                enemy.damage = 1.f;
                enemy.max_health = 1.f;
                enemy.cur_health = 1.f;
                enemy.speed  = 150.f;
            break;
            case EnemyType::DRAGONIT:
                enemy.armor  = 8.f;
                enemy.bounty = 3.f;
                enemy.damage = 2.f;
                enemy.max_health = 5.f;
                enemy.cur_health = 5.f;
                enemy.speed  = 100.f;
            break;
            case EnemyType::SERJANT:
                enemy.armor  = 10.f;
                enemy.bounty = 8.f;
                enemy.damage = 5.f;
                enemy.max_health = 15.f;
                enemy.cur_health = 15.f;
                enemy.speed  = 75.f;
            break;
            case EnemyType::TANK:
                enemy.armor  = 15.f;
                enemy.bounty = 15.f;
                enemy.damage = 10.f;
                enemy.max_health = 25.f;
                enemy.cur_health = 25.f;
                enemy.speed  = 50.f;
            break;
            default:
                SDL_Log("WARNING: Unknown enemy type %d", type);
        }
        m_enemies_descr.emplace(type, enemy);

        return enemy;
    }

    EnemyDescription* get_enemy_descr(EnemyType type){
        auto it = m_enemies_descr.find(type);
        if(it != m_enemies_descr.end()){
            return &it->second;
        }
        return nullptr;
    }

    EntityID get_object_from_position(const Vector2D& pos){
        for(int id = 1, n = Entities::size(); id < n; id++){
            SDL_FRect rect = {m_sprites[id].posX,
                              m_sprites[id].posY,
                              m_sprites[id].width,
                              m_sprites[id].height};
            auto point = pos.get_sdl_point();
            if(SDL_PointInRectFloat(&point, &rect) &&
               is_alive(id) &&
                (m_types[id] == EntityGlobalType::ENEMY_ENTITY ||
                 m_types[id] == EntityGlobalType::FRIEND_ENTITY))
                return id;
        }

        return get_empty_id();
    }

    void damage_entity(EntityID id, float damage){
        m_health[id].cur_health -= damage;
        if(auto enemy_descr = std::get_if<EnemyDescription>(&m_descriptions[id])){
            enemy_descr->cur_health = m_health[id].cur_health;
        }
        
    }

    float get_current_health(EntityID id){
        if(id > size()){
            SDL_Log("WARNING: obj with [%ld] out of range. Returned health=0.f", id);
            return 0.f;
        } 
        return m_health[id].cur_health;
    }

    float get_effective_speed(EntityID id){
        if(id > size()){
            SDL_Log("WARNING: obj with [%ld] out of range. Returned speed=0.f", id);
            return 0.f;
        } 
        return m_moves[id].speed * m_moves[id].speed_magnitude;
    }

    bool is_object_dead_pending(EntityID id) const { return m_health[id].dead_pending != 0; }
    
};


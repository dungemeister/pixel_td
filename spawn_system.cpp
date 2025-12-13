#include "spawn_system.h"

void SpawnSystem::update(Entities& objects, const Level& level, float deltatime){
    m_spawn_timeout -= deltatime;
    if(m_spawn_timeout <= 0.){
        auto enemies_objects = objects.get_objects_size(EntityGlobalType::ENEMY_ENTITY);
        auto tile_size = level.get_tile_size();
        int spawn_size = 0;
        if(m_max_enemies - static_cast<int>(enemies_objects) > m_spawn_size)
            spawn_size = m_spawn_size;
        else
            spawn_size = m_max_enemies - enemies_objects;
        for(int i = 0; i < spawn_size; i++){
            auto target = level.get_castle_tile().center_pos;
            auto spawn  = level.get_spawner_tile().center_pos;
            Vector2D random_pos = {spawn.x, RandomFloat(spawn.y - tile_size.y / 4, spawn.y + tile_size.y / 2)};
            objects.spawn_enemies_targeted(level, target, random_pos.get_sdl_point(), static_cast<SpriteType>(static_cast<int>(SpriteType::VIKING) + rand()%5));

        }
        m_spawn_timeout = m_spawn_interval;
    }
}
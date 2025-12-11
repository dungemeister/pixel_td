#pragma once
#include "components.h"
#include "level.h"

class CastleDamageSystem{
public:
    void update(Entities& objects, Level& level, float deltatime){
        for(int id = 0, n = objects.size(); id < n; id++){
            if(objects.m_types[id] == EntityType::ENEMY){

                Vector2D pos = objects.m_positions[id].get_vector2d();
                if(level.is_pos_in_castle(pos)){
                    std::cout << "Enemy " << id << " damaged castle" << std::endl;
                    objects.remove_object(id);
                }
            }
            else if(objects.m_types[id] == EntityType::PROJECTILE){
                Vector2D pos = objects.m_positions[id].get_vector2d();
                Vector2D target = objects.m_moves[id].target;
                auto diff = target - pos;
                if(diff.magnitude() <= 0.01f){
                    auto enemy_id = objects.m_moves[id].target_id;
                    SDL_Log("Enemy %lu damaged", enemy_id);
                    objects.remove_object(id);
                    objects.remove_object(enemy_id);
                }
            }
            
        }
    }
};
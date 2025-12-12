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
                    objects.reset_object(id);
                }
            }
            else if(objects.m_types[id] == EntityType::PROJECTILE){
                Vector2D pos = objects.m_positions[id].get_vector2d();
                Vector2D target = objects.m_moves[id].target;
                auto diff = target - pos;
                auto enemy_id = objects.m_moves[id].target_id;
                if(diff.magnitude() <= 0.1f){
                    // objects.update_other_projectiles(id);
                    if(objects.m_types[enemy_id] == EntityType::ENEMY &&
                       objects.is_alive(enemy_id) &&
                       objects.get_version_component(enemy_id).version == objects.m_moves[id].target_version.version){

                            objects.reset_object(enemy_id);
                       }
                    
                    objects.reset_object(id);
                }
            }
            
        }
    }
};
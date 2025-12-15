#pragma once
#include "components.h"
#include "level.h"
#include <functional>
#include "hud_system.h"

class CastleDamageSystem{
public:
    void update(Entities& objects, Level& level, float deltatime, std::unordered_map<ComponentType, std::function<void(float)>> callbacks){
        for(int id = 0, n = objects.size(); id < n; id++){
            if(objects.m_types[id] == EntityGlobalType::ENEMY_ENTITY){

                Vector2D pos = objects.m_positions[id].get_vector2d();
                if(level.is_pos_in_castle(pos)){
                    std::cout << "Enemy " << id << " damaged castle" << std::endl;
                    objects.reset_object(id);
                    auto health_cb = callbacks.find(ComponentType::CASTLE_HEALTH);
                    if(health_cb != callbacks.end())
                        health_cb->second(10.f);
                    
                }
            }
            else if(objects.m_types[id] == EntityGlobalType::PROJECTILE_ENTITY){
                Vector2D pos = objects.m_positions[id].get_vector2d();
                Vector2D target = objects.m_moves[id].target;
                auto diff = target - pos;
                auto enemy_id = objects.m_moves[id].target_id;
                if(diff.magnitude() <= 0.1f){
                    // objects.update_other_projectiles(id);
                    if(objects.m_types[enemy_id] == EntityGlobalType::ENEMY_ENTITY &&
                       objects.is_alive(enemy_id) &&
                       objects.get_version_component(enemy_id).version == objects.m_moves[id].target_version.version){

                            auto burst_damage = objects.m_firings[id].descr->burst_damage;
                            objects.m_health[enemy_id].cur_health -= burst_damage;
                            if(objects.m_health[enemy_id].cur_health <= 0.f)
                                objects.reset_object(enemy_id);
                       }
                    
                    objects.reset_object(id);
                }
            }
            
        }
    }
};
#pragma once
#include "components.h"
#include "level.h"
#include <functional>
#include "hud_system.h"
#include "buff_system.h"

class CastleDamageSystem{
public:
    void update(Entities& objects, Level& level, BuffSystem& buffs, float deltatime){
        for(int id = 0, n = objects.size(); id < n; id++){
            if(objects.m_types[id] == EntityGlobalType::ENEMY_ENTITY){

                Vector2D pos = objects.m_sprites[id].center;
                if(level.is_pos_in_castle(pos)){
                    std::cout << "Enemy " << id << " damaged castle" << std::endl;
                    
                    objects.set_object_alive_state(id, 0);
                    objects.set_object_dead_pending_reason(id, HealthComponent::DeadPendingReason::PASSED);
                    
                }
            }
            else if(objects.m_types[id] == EntityGlobalType::PROJECTILE_ENTITY){
                Vector2D pos = objects.m_positions[id].get_vector2d();
                Vector2D target = objects.m_moves[id].target;
                auto diff = target - pos;
                auto enemy_id = objects.m_moves[id].target_id;
                if(diff.magnitude() <= 0.1f){
                    if(objects.m_types[enemy_id] == EntityGlobalType::ENEMY_ENTITY &&
                       objects.is_alive(enemy_id) &&
                       objects.get_version_component(enemy_id).version == objects.m_moves[id].target_version.version){
                            
                            auto burst_damage = objects.m_firings[id].descr->burst_damage;
                            auto descr = objects.m_firings[id].descr;
                            switch(objects.m_sprites[id].type){
                                case SpriteType::FIRE_PROJECTILE:
                                    buffs.add_buff(enemy_id,
                                                   objects.get_version_component(enemy_id),
                                                   BuffType::IGNITE,
                                                   descr->periodic_damage,
                                                   descr->periodic_time);
                                break;
                                case SpriteType::ICE_PROJECTILE:
                                    buffs.add_buff(enemy_id,
                                                   objects.get_version_component(enemy_id),
                                                   BuffType::SLOW,
                                                   descr->slowing_magnitude,
                                                   descr->slowing_time);
                                    buffs.add_buff(enemy_id,
                                                   objects.get_version_component(enemy_id),
                                                   BuffType::IGNITE,
                                                   descr->periodic_damage,
                                                   descr->periodic_time);

                                break;
                            }

                            objects.damage_entity(enemy_id, burst_damage);
                            
                            
                            if(objects.m_health[enemy_id].cur_health <= 0.f){
                                objects.set_object_alive_state(enemy_id, 0);
                                objects.set_object_dead_pending_reason(enemy_id, HealthComponent::DeadPendingReason::KILLED);
                            }
                                
                       }
                    objects.set_object_alive_state(id, 0);
                    objects.set_object_dead_pending_reason(id, HealthComponent::DeadPendingReason::EXPLOUDED);
                }
            }
            
        }
    }

    

};
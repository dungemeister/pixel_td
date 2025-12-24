#pragma once
#include "components.h"
#include "level.h"
#include <functional>
#include "hud_system.h"
#include "buff_system.h"

class CastleDamageSystem{
public:
    void update(Entities& objects, Level& level, BuffSystem& buffs, float deltatime, std::unordered_map<ComponentType, std::function<bool(float)>> callbacks){
        for(int id = 0, n = objects.size(); id < n; id++){
            if(objects.m_types[id] == EntityGlobalType::ENEMY_ENTITY){

                Vector2D pos = objects.m_sprites[id].center;
                if(level.is_pos_in_castle(pos)){
                    std::cout << "Enemy " << id << " damaged castle" << std::endl;
                    auto concrete_enemy_type = objects.m_concrete_types[id];
                    if(!std::holds_alternative<EnemyType>(concrete_enemy_type)){
                        SDL_Log("WARNING: wrong concrete enemy type");
                        objects.reset_object(id);
                        continue;
                    }
                    //Finding assosiated callback and enemy descriptor
                    auto health_cb = callbacks.find(ComponentType::CASTLE_HEALTH);
                    auto descr = objects.m_enemies_descr.find(std::get<EnemyType>(concrete_enemy_type));
                    if(health_cb != callbacks.end() &&
                       descr != objects.m_enemies_descr.end()){
                        
                        health_cb->second(descr->second.damage);
                    }
                    
                    objects.reset_object(id);
                    
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
                                auto concrete_enemy_type = objects.m_concrete_types[enemy_id];
                                if(!std::holds_alternative<EnemyType>(concrete_enemy_type)){
                                    SDL_Log("WARNING: wrong concrete enemy type deathrattle");
                                }
                                else{
                                    auto gold_cb = callbacks.find(ComponentType::PLAYER_GOLD);
                                    auto descr = objects.m_enemies_descr.find(std::get<EnemyType>(concrete_enemy_type));
                                    if(gold_cb != callbacks.end() &&
                                       descr != objects.m_enemies_descr.end()){
                                        gold_cb->second(descr->second.bounty);
                                    }
                                }
                                objects.reset_object(enemy_id);
                            }
                       }
                    
                    objects.reset_object(id);
                }
            }
            
        }
    }

    

};
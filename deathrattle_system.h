#pragma once
#include "components.h"
#include "functional"
#include "hud_system.h"

struct DeathrattleSystem{

void update(Entities& objects, float deltatime, const std::unordered_map<ComponentType, std::function<bool(float)>>& callbacks){
    for(int id = 0, n = objects.size(); id < n; id++){
        if(objects.is_alive(id)) continue;
        
        if(objects.m_types[id] == EntityGlobalType::ENEMY_ENTITY){
            if(!objects.is_object_dead_pending(id)) continue;

            auto concrete_enemy_type = objects.m_concrete_types[id];
            if(!std::holds_alternative<EnemyType>(concrete_enemy_type)){
                SDL_Log("WARNING: wrong concrete enemy type");
                objects.reset_object(id);
                continue;
            }
            auto descr = objects.m_enemies_descr.find(std::get<EnemyType>(concrete_enemy_type));
            auto gold_cb = callbacks.find(ComponentType::PLAYER_GOLD);
            auto health_cb = callbacks.find(ComponentType::CASTLE_HEALTH);
            switch(objects.m_health[id].dead_pending){
                //Enemy killed by tower/debuff
                case HealthComponent::DeadPendingReason::KILLED:
                    if(gold_cb != callbacks.end() &&
                        descr != objects.m_enemies_descr.end()){
                        gold_cb->second(descr->second.bounty);
                    }
                break;
                //Enemy passed to the castle
                case HealthComponent::DeadPendingReason::PASSED:
                    if(health_cb != callbacks.end() &&
                        descr != objects.m_enemies_descr.end()){
                        health_cb->second(descr->second.damage);
                    }
                break;
                default:
                    SDL_Log("WARNING: unknown dead pending reason for enemy");
            }
            
            objects.reset_object(id);
        }
        if(objects.m_types[id] == EntityGlobalType::PROJECTILE_ENTITY){

            objects.reset_object(id);
        }
    }
}
};
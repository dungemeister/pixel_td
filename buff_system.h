#pragma once
#include "components.h"

struct BuffSystem{

void add_buff(EntityID id, VersionComponent version, BuffType type, float magnitude,float duration = -1.f){
    if(duration == -1.f){
        duration = s_buff_info[static_cast<int>(type)].duration;
    }

    BuffComponent buff = {
        .id = id,
        .version = version,
        .type = type,
        .duration = duration,
        .elapsed_time = 0.f,
        .magnitude = magnitude,
    };

    m_pending_buffs.emplace_back(std::move(buff));
}

void update(Entities& objects, float deltatime){

    m_buffs.insert(m_buffs.begin(), m_pending_buffs.begin(), m_pending_buffs.end());
    m_pending_buffs.clear();

    for(size_t i = 0; i < m_buffs.size(); i++){
        auto& buff = m_buffs[i];
        //Update buff ticks
        auto id = buff.id;
        buff.duration -= deltatime;
        buff.elapsed_time += deltatime;
        //Apply buff to object
        if(!apply_buff(buff, objects, deltatime)){
            erase_buff(buff);
            i--;
        }
        //Remove expired multiplicative buff or remove buff from not alive object 
        if(buff.duration <= 0.f){

            remove_buff(buff, objects);
            erase_buff(buff);
            i--;
        }

    }
}

void add_pending_buff(const BuffComponent&& buff){
    m_pending_buffs.emplace_back(buff);
}

bool apply_buff(const BuffComponent& buff, Entities& objects, float deltatime){
    if(objects.get_version_component(buff.id) != buff.version)
    {
        SDL_Log("WARNING: object version [%ld], buff version [%ld]",
                objects.get_version_component(buff.id).version,
                buff.version.version);
        return false;
    }
    const float TICK_INTERVAL = 0.5f;
    auto value = std::fmod(buff.elapsed_time, TICK_INTERVAL);
    switch(buff.type){
        case BuffType::IGNITE:
            // SDL_Log("fmod :%.3f; deltatime :%.3f", value, deltatime);

            if(objects.is_alive(buff.id) && value <= deltatime){
                SDL_Log("Ignite tick ID[%ld]", buff.id);
                objects.damage_entity(buff.id, buff.magnitude * TICK_INTERVAL);
                if(objects.get_current_health(buff.id) < 0.f){
                    objects.set_object_alive_state(buff.id, 0);
                    objects.set_object_dead_pending_reason(buff.id, HealthComponent::DeadPendingReason::KILLED);
                }
                
            }
        break;
        case BuffType::SLOW:
            if(objects.is_alive(buff.id)){
                //TODO: recalculate multiplier
                objects.m_moves[buff.id].speed_magnitude = 1.f - buff.magnitude;

            }
        break;
    }
    return true;
}

void remove_buff(const BuffComponent& buff, Entities& objects){
    switch(buff.type){
        case BuffType::SLOW:
            if(objects.is_alive(buff.id)){
                //TODO: recalculate multiplier
                objects.m_moves[buff.id].speed_magnitude = 1.f;

            }
        break;
    }
}

void erase_buff(BuffComponent& buff){
    std::swap(buff, m_buffs.back());
    m_buffs.pop_back();
}

std::vector<BuffComponent> m_buffs;
std::vector<BuffComponent> m_pending_buffs;
};
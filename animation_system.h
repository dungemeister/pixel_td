#pragma once
#include "components.h"

struct AnimationSystem{
    void update(Entities& objects, float deltatime) {
        for(size_t id = 0, n = objects.size(); id < n; id++){
            
            if((objects.m_systems[id] & eSpriteAnimationSystem) != 0){
                if(m_registered_types.count(objects.m_sprites[id].type) <= 0)
                    continue;

                SpriteComponent& sprite = objects.m_sprites[id];
                AnimationComponent& anim = objects.m_animations[id];
                MoveComponent& move = objects.m_moves[id];
                anim.cur_frame += deltatime * anim.fps * move.speed_magnitude;
                while(anim.cur_frame >= anim.frames_size){
                    anim.cur_frame -= anim.frames_size;
                }
                anim.cur_index = static_cast<int>(anim.cur_frame);
                sprite.anim_index = anim.cur_index;
            }
        }

    }

    void register_type(SpriteType type, size_t textures_size){
        m_registered_types.emplace(type, textures_size);
    }
    void unregister_type(SpriteType type){
        m_registered_types.erase(type);
    }
    std::unordered_map<SpriteType, size_t> m_registered_types;
};

#pragma once
#include "components.h"
#include "level.h"

class CastleDamageSystem{
public:
    void update(Entities& objects, Level& level, float deltatime){
        for(int id = 0, n = objects.size(); id < n; id++){
            if(objects.m_types[id] != EntityType::ENEMY) continue;
            
            Vector2D pos = {objects.m_positions[id].x, objects.m_positions[id].y};
            if(level.is_pos_in_castle(pos)){
                std::cout << "Enemy " << id << " damaged castle" << std::endl;
                objects.remove_object(id);
            }
        }
    }
};
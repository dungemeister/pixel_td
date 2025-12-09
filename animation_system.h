#pragma once
#include "components.h"

struct AnimationSystem{
    void update(Entities& objects, float deltatime) {
        for(size_t i = 0; i < objects.size(); i++){
            
            if(objects.m_systems[i] & ePositionSystem &&
               objects.m_systems[i] & eMoveSystem &&
               objects.m_systems[i] & eMapBorderSystem &&
               (objects.m_types[i] == ENEMY || objects.m_types[i] == TOWER)){
                auto speed = objects.m_moves[i].speed * deltatime;
                objects.m_positions[i].x += speed;
                objects.m_positions[i].y += speed;
                // if(objects.m_positions[i].x  >= objects.m_borders[i].x_max || objects.m_positions[i].x <= objects.m_borders[i].x_min)
                //     objects.m_moves[i].velX *= -1;
                // if(objects.m_positions[i].y >= objects.m_borders[i].y_max || objects.m_positions[i].y <= objects.m_borders[i].y_min)
                //     objects.m_moves[i].velY *= -1;
                
                objects.m_positions[i].angle += objects.m_moves[i].rotation_angle;
            }
        }

    }
};

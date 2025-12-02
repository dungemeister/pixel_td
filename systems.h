#pragma once
#include "components.h"

struct MoveSystem{
    void update(Entities& objects, float deltatime) {
        for(size_t i = 0; i < objects.size(); i++){
            
            if(objects.m_flags[i] & fEntityPosition && objects.m_flags[i] & fEntityMove){
                // std::cout << "Moving " << objects.m_names[i] << "\n";
                objects.m_positions[i].x += objects.m_moves[i].velX;
                objects.m_positions[i].y += objects.m_moves[i].velY;
                if(objects.m_positions[i].x  >= objects.m_borders[i].x_max || objects.m_positions[i].x <= objects.m_borders[i].x_min)
                    objects.m_moves[i].velX *= -1;
                if(objects.m_positions[i].y >= objects.m_borders[i].y_max || objects.m_positions[i].y <= objects.m_borders[i].y_min)
                    objects.m_moves[i].velY *= -1;
                
                objects.m_positions[i].angle += objects.m_moves[i].rotation_angle;
            }
        }

    }
};

static MoveSystem s_move_system;

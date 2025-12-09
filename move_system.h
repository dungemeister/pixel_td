#pragma once
#include "components.h"
#include "level.h"

class MoveSystem{
public:
    void update(Entities& objects, Level& level, float deltatime){
    
        for(int id = 0, n = objects.size(); id < n; id++){
            if((objects.m_systems[id] & eMoveSystem) == 0) continue;
            
            if(objects.m_moves[id].targeted){
                Vector2D pos = {objects.m_positions[id].x, objects.m_positions[id].y};
                Vector2D diff = objects.m_moves[id].target - pos;
                Vector2D force = CalculateLaneConstraintForce(level, pos);

                float distanceToTarget = diff.magnitude();

                //Determine the distance to move this frame.
                float distanceMove = objects.m_moves[id].speed * deltatime;
                if (distanceMove > distanceToTarget)
                    distanceMove = distanceToTarget;

                //Find the normal from the flow field.
                Vector2D direction(level.get_dir(pos) + force);
                Vector2D directionNormal = direction.normalize();
                Vector2D posAdd = directionNormal * distanceMove;


                //Check if it needs to move in the x direction.  If so then check if the new x position, plus an amount of spacing 
                //(to keep from moving too close to the wall) is within a wall or not and update the position as required.
                // const float spacing = 0.35f;
                // int x = (int)(pos.x + posAdd.x + copysign(spacing, posAdd.x));
                // int y = (int)(pos.y);
                pos.x += posAdd.x;

                // //Do the same for the y direction.
                // x = (int)(pos.x);
                // y = (int)(pos.y + posAdd.y + copysign(spacing, posAdd.y));
                // if (posAdd.y != 0.0f)
                pos.y += posAdd.y;
                
                objects.m_positions[id].x = pos.x;
                objects.m_positions[id].y = pos.y;
                objects.m_sprites[id].posX = objects.m_positions[id].x;
                objects.m_sprites[id].posY = objects.m_positions[id].y;
            }
            else{

            }
        }
    }
    Vector2D CalculateLaneConstraintForce(Level& level, Vector2D pos) {
        // Проекция позиции на центральную линию пути
        Vector2D closestPoint = level.get_tile_center(pos);
        Vector2D laneCenter = closestPoint;
        
        // Допустимое отклонение от центра
        Vector2D toCenter = laneCenter - pos;
        float distanceFromCenter = toCenter.magnitude();
        float laneWidth = 20.f;
        float maxForce = .2f;
        // Если мы слишком далеко от центра, мягко возвращаем
        if (distanceFromCenter > laneWidth * 0.4) {
            // Сила тем сильнее, чем дальше от центра
            float strength = (distanceFromCenter / (laneWidth * 0.5)) - 0.8;
            return toCenter.normalize() * strength * maxForce;
        }
        
        return {};
    }
};

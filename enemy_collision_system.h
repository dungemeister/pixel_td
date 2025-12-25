#pragma once
#include "components.h"
#include "level.h"

class EnemyCollistionSystem{
public:
    void update(Entities& objects, Level& level, float deltatime){
        std::vector<std::pair<int, int>> collisions;
        collisions.reserve(objects.size() * 2);

        for(int id1 = 0, n = objects.size(); id1 < n; id1++){
            if((objects.m_types[id1] != EntityGlobalType::ENEMY_ENTITY) || 
               (objects.m_systems[id1] & eCollisionSystem) == 0) continue;

            for(int id2 = id1 + 1; id2 < n; id2++){
                if(objects.m_types[id2] != EntityGlobalType::ENEMY_ENTITY || 
                  (objects.m_systems[id1] & eCollisionSystem) == 0) continue;
                if(objects.m_sprites[id1].flag & fCenterSprite){
                    if (circles_overlap(
                        objects.m_sprites[id1].center, objects.m_sprites[id1].radius,
                        objects.m_sprites[id2].center, objects.m_sprites[id2].radius)) {
                        
                        collisions.emplace_back(id1, id2);
                    }
                }
                if(objects.m_sprites[id1].flag & fUpperLeftSprite){
                    if (circles_overlap(
                        objects.m_sprites[id1].posX, objects.m_sprites[id1].radius,
                        objects.m_sprites[id2].posY, objects.m_sprites[id2].radius)) {
                        
                        collisions.emplace_back(id1, id2);
                    }
                }
            }
        }

        for(const auto& [id1, id2]: collisions){
            auto& obj1 = objects.m_sprites[id1].center;
            float radius1 = objects.m_sprites[id1].radius;
            auto& obj2 = objects.m_sprites[id2].center;
            float radius2 = objects.m_sprites[id2].radius;

            Vector2D diff = obj1 - obj2;
            float distance = diff.magnitude();

            if(distance < 0.0001f){
                diff = Vector2D(0.1f, 0.f);
                distance = 0.1f;
            }

            float overlap = radius1 + radius2 - distance;
            if(overlap > 0){
                Vector2D correction = diff.normalize() * overlap / 200;
                obj1 += correction;
                objects.m_sprites[id1].update_pos_from_center();

                obj2 -= correction;
                objects.m_sprites[id2].update_pos_from_center();
            }
        }
    }

    bool circles_overlap(Vector2D pos1, float radius1, Vector2D pos2, float radius2){
        auto diff = pos1 - pos2;
        return diff.magnitude() <= (radius1 + radius2);
    }
};

#pragma once
#include "components.h"
#include "level.h"

class EnemyCollistionSystem{
public:
    void update(Entities& objects, Level& level, float deltatime){
        std::vector<std::pair<int, int>> collisions;
        collisions.reserve(objects.size() * 2);

        for(int id1 = 0, n = objects.size(); id1 < n; id1++){
            if(objects.m_types[id1] != EntityGlobalType::ENEMY_ENTITY) continue;

            for(int id2 = id1 + 1; id2 < n; id2++){
                if(objects.m_types[id2] != EntityGlobalType::ENEMY_ENTITY) continue;

                if (circles_overlap(
                    objects.m_positions[id1].get_vector2d(), 10.f,
                    objects.m_positions[id2].get_vector2d(), 10.f)) {
                    
                    collisions.emplace_back(id1, id2);
                }
            }
        }

        for(const auto& [id1, id2]: collisions){
            auto& obj1 = objects.m_positions[id1];
            float radius1 = 10.f;
            auto& obj2 = objects.m_positions[id2];
            float radius2 = 10.f;

            Vector2D diff = obj1.get_vector2d() - obj2.get_vector2d();
            float distance = diff.magnitude();

            if(distance < 0.0001f){
                diff = Vector2D(0.1f, 0.f);
                distance = 0.1f;
            }

            float overlap = radius1 + radius2 - distance;
            if(overlap > 0){
                Vector2D correction = diff.normalize() * overlap;
                obj1.x += correction.x;
                obj1.y += correction.y;
                
                obj2.x -= correction.x;
                obj2.y -= correction.y;
            }
        }
    }

    bool circles_overlap(Vector2D pos1, float radius1, Vector2D pos2, float radius2){
        auto diff = pos1 - pos2;
        return diff.magnitude() <= (radius1 + radius2);
    }
};

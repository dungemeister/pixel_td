#pragma once
#include "components.h"
#include "level.h"
#include <unordered_map>
#include "render_system.h"

struct FiringSystem{

    void update(Entities& objects, Level& level, float deltatime){
        for(int id = 0, n = objects.size(); id < n; id++){
            if((objects.m_systems[id] & eFiringSystem) == 0) continue;

            FiringComponent& firing_comp =  objects.m_firings[id];
            PositionComponent& pos_comp = objects.m_positions[id];
            firing_comp.cooldown -= deltatime;
            if(firing_comp.cooldown <= 0){
                switch(firing_comp.descr->firing_type){
                    case FiringType::eProjectile:
                        auto tile_opt = level.get_tile(objects.m_positions[id].get_sdl_fpoint());
                        if(!tile_opt.has_value()) continue;

                        auto tile = tile_opt.value();
                        auto castle_tile = level.get_castle_tile();
                        auto enemy_id = objects.get_nearest_enemy_to_point_in_radius(tile.center_pos,
                                                                                     castle_tile.center_pos,
                                                                                     objects.m_firings[id].radius);

                        if(enemy_id != objects.get_empty_id()){
                            auto tile_size = level.get_tile_size();
                            float rect_scale = 0.2;
                            SDL_FRect rect = {tile.center_pos.x,
                                              tile.center_pos.y,
                                              tile_size.x * rect_scale,
                                              tile_size.y * rect_scale};
                            auto tower_descr = objects.get_tower_descr(firing_comp.descr->type);
                            auto id = objects.add_projectile(tower_descr,
                                                             rect,
                                                             enemy_id);
                            firing_comp.cooldown = firing_comp.interval;
                        }
                        else{
                            firing_comp.cooldown = 0;
                        }
                    break;
                }
                
            }
        }
    }

};
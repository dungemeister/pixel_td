#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "render_system.h"
#include "level.h"
#include "animation_system.h"
#include "castle_damage_system.h"
#include "move_system.h"
#include "enemy_collision_system.h"
#include "spawn_system.h"

class Game{
public:
    Game();
    ~Game() = default;
    void destroy_game();
    void loop();
    void handle_input();
    void update_game(float deltatime);
    void draw_output();

private:
    SDL_Window*         m_window;
    SDL_Renderer*       m_renderer;
    SDL_Cursor*         m_cursor;
    SDL_FPoint          m_cursor_pos;
    RenderSystem*       m_render_system;
    AnimationSystem     m_animation_system;
    MoveSystem          m_move_system;
    CastleDamageSystem  m_castle_damage_system;
    EnemyCollistionSystem m_enemy_collision_system;
    SpawnSystem         m_spawn_system;
    Entities            m_objects;

    std::vector<Level> m_levels;
    Level              m_cur_level;

    int m_running;
    int m_width;
    int m_height;

    Vector2D m_target;

    void init();
    void load_cursor();
    void init_render_system();
    void init_game();
    void resize_callback();
    void handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event);
    bool add_tower(Entities& objects, TowerType type, const TileComponent& tile);
    void add_target(Entities& objects, const Vector2D& pos);
    void spawn_enemies_targeted(Entities& objects, const Vector2D& target, const SDL_FPoint& spawn_pos);
    void load_level_tiles();
    void load_decorations();
    void load_decor_sprite(const Vector2D& pos, EntityType type);
    void load_decor_random_sprites(EntityType type, size_t size);
};
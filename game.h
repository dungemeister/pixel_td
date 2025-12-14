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
#include "firing_system.h"
#include "hud_system.h"

class Game{
public:
enum GameState{
    MainMenu,
    Gameplay,
    PauseMenu,
    Settings,
};
    Game();
    ~Game() = default;
    void destroy_game();
    void loop();
    void handle_input();
    void update_game();
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
    FiringSystem        m_firing_system;
    HudSystem           m_hud_system;
    Entities            m_objects;

    std::vector<Level> m_levels;
    Level              m_cur_level;

    Uint64 m_current_ticks;
    
    TowerDescription* m_selected_tower;
    std::unordered_map<SDL_Scancode, TowerDescription> m_towers_scancode;

    std::unordered_map<ComponentType, float> m_components_data;
    std::unordered_map<ComponentType, std::function<void(float)>> m_components_callbacks;

    GameState m_state;
    int m_running;
    int m_width;
    int m_height;

    void init();
    void load_cursor();
    void init_render_system();
    void init_game();
    void resize_callback();
    void handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event);
    void load_level_tiles();
    void load_decorations();
    void load_towers();
    void load_hearth();
    void load_coins();
    void load_hud();
    void load_layout();
    void load_decor_sprite(const Vector2D& pos, SpriteType type);
    void load_decor_random_sprites(SpriteType type, size_t size);
    void register_type(SpriteType type, const std::vector<std::string>& textures);

    void update_castle_health(float value);
    void register_towers();
};
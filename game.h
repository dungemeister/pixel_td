#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

#include "render_system.h"
#include "level.h"
#include "animation_system.h"
#include "castle_damage_system.h"
#include "move_system.h"
#include "enemy_collision_system.h"
#include "spawn_system.h"
#include "firing_system.h"
#include "hud_system.h"
#include "hud_render_system.h"
#include "buff_system.h"
#include "deathrattle_system.h"

#include "ui_layout.h"
#include "ui_label.h"
#include "ui_image.h"
#include "ui_slider.h"
#include "ui_circle.h"

class Game{
public:
enum GameState{
    MainMenu,
    Gameplay,
    PauseMenu,
    Settings,
};
    Game();
    ~Game();
    void destroy_game();
    void loop();
    void handle_input();
    void handle_update();
    void handle_draw();

private:
    SDL_Window*                           m_window;
    SDL_Renderer*                         m_renderer;
    SDL_Cursor*                           m_cursor;
    SDL_FPoint                            m_cursor_pos;
    std::unique_ptr<RenderSystem>         m_render_system;
    AnimationSystem                       m_animation_system;
    MoveSystem                            m_move_system;
    CastleDamageSystem                    m_castle_damage_system;
    EnemyCollistionSystem                 m_enemy_collision_system;
    SpawnSystem                           m_spawn_system;
    FiringSystem                          m_firing_system;
    BuffSystem                            m_buff_system;
    DeathrattleSystem                     m_drattle_system;
    Entities                              m_objects;

    std::vector<Level> m_levels;
    Level              m_cur_level;

    Uint64 m_current_ticks;
    
    Entities            m_pause_menu_objects;
    HudSystem           m_pause_menu;
    HudSystem           m_main_menu;

    std::unique_ptr<UILayout>   m_descriptions_layout;
    std::unique_ptr<UILayout>   m_player_stats_layout;
    std::unique_ptr<UILayout>   m_map_layout;

    TowerDescription* m_selected_tower;
    EnemyDescription* m_selected_enemy;
    std::unordered_map<SDL_Scancode, TowerDescription> m_towers_scancode;
    std::unordered_map<SDL_Scancode, EnemyDescription> m_enemies_scancode;

    std::unordered_map<ComponentType, float> m_components_data;
    std::unordered_map<ComponentType, std::function<bool(float)>> m_components_callbacks;

    
    GameState m_state;
    int m_running;
    int m_width;
    int m_height;

    float       m_castle_health = 100.f;
    const float m_max_castle_health = 100.f;
    
    float m_player_gold   = 40.f;

    float m_hud_font_size = 24.f;
    TTF_Font* m_info_font;
private:
    void init();
    void load_cursor(const std::string& sprite);
    void init_render_system();
    void init_game();
    void init_pause_menu();
    void resize_callback();
    void handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event);
    void load_level_tiles();
    void load_decorations();
    void load_towers();
    void load_hearth_callback();
    void load_gold_callback();
    void load_hud();
    void load_hud_layout();
    void load_decor_sprite(const Vector2D& pos, SpriteType type);
    void load_decor_random_sprites(SpriteType type, size_t size, float scale);
    void register_type(SpriteType type, const std::vector<std::string>& textures);
    void register_towers();
    void register_enemies();
    
    void update_game();

    void update_description_layout(const SpriteComponent& sprite, const Entities::Descriptor& descr);
    void remove_descriptor_widgets(){ m_descriptions_layout->RemoveWidgets(); }
};
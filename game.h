#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "render_system.h"
#include "level.h"
#include "animation_system.h"

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
    
    std::vector<Level> m_levels;
    Level              m_cur_level;

    int m_running;
    int m_width;
    int m_height;

    void init();
    void load_cursor();
    void init_render_system();
    void init_game();
    void resize_callback();
    void handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event);
    bool add_tower(Entities& objects, TowerType type, const TileComponent& tile);
};
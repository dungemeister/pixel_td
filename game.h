#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "render_system.h"

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
    SDL_Window*   m_window;
    SDL_Renderer* m_renderer;
    SDL_Cursor*   m_cursor;
    SDL_FPoint    m_cursor_pos;
    RenderSystem* m_render_system;

    int m_running;
    int m_width;
    int m_height;

    void init();
    void load_cursor();
    void init_render_system();
    void init_game();
    void handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event);
};
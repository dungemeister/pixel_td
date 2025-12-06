
#include "game.h"
#include "figures.h"
#include "wrapper.h"
#include "math.h"
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include "test.h"

float fps;
static Entities s_objects;

Game::Game()
    :m_window(nullptr)
    ,m_renderer(nullptr)
    ,m_cursor(nullptr)
    ,m_cursor_pos()
    ,m_width(16 * 120)
    ,m_height(9 * 120)
{
    init();
}

void Game::init(){
    init_render_system();
    init_game();
}

void Game::load_cursor(){
    SDL_Surface* orig_surface = IMG_Load("assets/quot-stickers.png");

    if(!orig_surface){
        SDL_Log("IMG_Load: %s", SDL_GetError());
        SDL_DestroySurface(orig_surface);
        return;
    }
    SDL_Surface* scaled_surface = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    if(!scaled_surface){
        SDL_Log("IMG_Load: %s", SDL_GetError());
        SDL_DestroySurface(orig_surface);
        SDL_DestroySurface(scaled_surface);
        return;
    }
    SDL_Rect r = {0, 0, 64, 64};
    SDL_BlitSurfaceScaled(orig_surface, NULL, scaled_surface, &r, SDL_SCALEMODE_NEAREST);
    m_cursor = SDL_CreateColorCursor(scaled_surface, 0, 0);
    if(!m_cursor){
        SDL_Log("SDL_CreateColorCursor: %s", SDL_GetError());
        SDL_DestroySurface(orig_surface);
        SDL_DestroySurface(scaled_surface);
        return;
    }
    SDL_DestroySurface(orig_surface);
    SDL_DestroySurface(scaled_surface);
    SDL_SetCursor(m_cursor);
}

void Game::init_render_system(){
    int res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    assert(res != 0);

    m_window = SDL_CreateWindow("Anime TD", m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_render_system = new RenderSystem(m_window);

    // load_cursor();
    
}

void Game::init_game(){
    SDL_FPoint level_size = {1024., 720.};
    SDL_FPoint level_pos = {(m_width - level_size.x) / 2, (m_height - level_size.y) / 2};
    m_levels.emplace_back("assets/map/level2.map", level_pos, level_size);
    m_levels.emplace_back("assets/map/level1.map", level_pos, level_size);
    m_cur_level = m_levels.back();
    m_levels.pop_back();

    s_objects.reserve(100);

    // EntityID obj = s_objects.add_object("tower");
    
    // s_objects.m_positions[obj].x = 0;
    // s_objects.m_positions[obj].y = 0;
    // s_objects.m_flags[obj] |= fEntityPosition;

    // s_objects.m_sprites[obj].index = obj;
    // s_objects.m_sprites[obj].r = 0.6;
    // s_objects.m_sprites[obj].g = 0.6;
    // s_objects.m_sprites[obj].b = 0.6;
    // s_objects.m_sprites[obj].index = RandomInt(0,5);
    // s_objects.m_flags[obj] |= fEntitySprite;

    // s_objects.m_moves[obj].Initialize(10., 30.);
    // s_objects.m_flags[obj] |= fEntityMove;

    //Tiles
    auto res = m_cur_level.get_resolution();
    auto tokens = m_cur_level.get_tokens();
    auto size = m_cur_level.get_size();
    auto tile_size = m_cur_level.get_tile_size();
    auto level_position = m_cur_level.get_position();
    auto rows = res.first;
    auto columns = res.second;

    // int tile_width = std::min((m_width - 16*10) / res.second, 64UL);
    // int tile_height = std::min((m_height - 9*10) / res.first, 64UL);
    int tile_width = tile_size.x;
    int tile_height = tile_size.y;
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < columns; c++){

            EntityID obj = s_objects.add_object("tile");
            int sprite_index = std::atoi(tokens[columns * r + c].c_str());
            {
                auto tile = m_cur_level.get_tile(r, c);
                auto tile_pos = tile.pos;

                s_objects.m_sprites[obj].index = sprite_index;
                s_objects.m_sprites[obj].scale = 1;
                s_objects.m_sprites[obj].width = tile_width;
                s_objects.m_sprites[obj].height = tile_height;
                s_objects.m_sprites[obj].r = 0.6;
                s_objects.m_sprites[obj].g = 0.6;
                s_objects.m_sprites[obj].b = 0.6;
                // s_objects.m_flags[obj] |= fEntitySprite | fEntitySpriteBorder;
                s_objects.m_flags[obj] |= fEntitySprite;

                s_objects.m_positions[obj].x = tile_pos.x;
                s_objects.m_positions[obj].y = tile_pos.y;
                s_objects.m_positions[obj].angle = 0;
                s_objects.m_flags[obj] |= fEntityPosition;

                s_objects.m_moves[obj].velX = RandomFloat(.1, 3.);
                s_objects.m_moves[obj].velY = RandomFloat(.1, 3.);
                s_objects.m_moves[obj].rotation_angle = RandomFloat(-1.f, 1.f);
                // s_objects.m_flags[obj] |= fEntityMove;

                s_objects.m_borders[obj].x_min = tile_width * c;
                s_objects.m_borders[obj].x_max = tile_width * c;
                s_objects.m_borders[obj].y_min = tile_height * r;
                s_objects.m_borders[obj].y_max = tile_height * r;
                s_objects.m_flags[obj] |= fEntityMapBorder;

                s_objects.m_types[obj] = fTile;
            }

            // std::cout << "Pos " << s_objects.m_positions[obj].x << " " << s_objects.m_positions[obj].y << "\n";

        }

    }

}

void Game::destroy_game(){
    delete m_render_system;
}

void Game::loop(){
    Uint64 lastTime = SDL_GetTicks();
    int frames = 0;
    Uint64 interval = 100; // 1 секунда в мс
    while(m_running){
        Uint64 currentTime = SDL_GetTicks();
        frames++;

        if (currentTime - lastTime >= interval) {
            fps = static_cast<float>(frames) / (currentTime - lastTime) * 1000.0f;
            lastTime = currentTime;
            frames = 0;
        }

        handle_input();
        update_game(std::min(static_cast<float>(currentTime - lastTime) / 1000, 0.05f));
        draw_output();
        int sleep = 8 - (SDL_GetTicks() - currentTime);
        if(sleep > 0){
            SDL_Delay(sleep); // Ограничивает ~120 FPS
        }
    }
}

void Game::handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event){
    if(mouse_event.button == SDL_BUTTON_LEFT){
        auto tile = m_cur_level.get_tile({mouse_event.x, mouse_event.y});
        if(tile.has_value() && tile.value().occupied == 0){
            if(add_tower(objects, RocketTower, tile.value()))
                std::cout << "Added tower to tile [" << tile.value().row << ", "
                                                     << tile.value().column <<
                                                 "]" << std::endl;

        }
        else{
            std::cout << "Cannot add tower to " << mouse_event.x << " " << mouse_event.y <<
                         ". Tile already occupied or not in level map" << std::endl;
        }
    }
}

void Game::handle_input(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_EVENT_QUIT:
                m_running = 0;
            break;
            case SDL_EVENT_WINDOW_RESIZED:
                int w,h;
                if(SDL_GetWindowSize(m_window, &w, &h)){
                    m_width = w;
                    m_height = h;
                    std::cout << "Window resized " << w << " " << h << std::endl;
                    resize_callback();
                }
            break;
            case SDL_EVENT_MOUSE_MOTION:
                m_cursor_pos.x = event.motion.x;
                m_cursor_pos.y = event.motion.y;
            break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                handle_mouse_event(s_objects, event.button);
            break;
        }
    }
}

void Game::update_game(float deltatime){
    // std::cout << deltatime <<"\n";
    m_animation_system.update(s_objects, deltatime);

    m_render_system->clean_batch_frame();
    m_render_system->clean_frame();
    for(size_t i = 0, n = s_objects.size(); i != n; i++){
        if(s_objects.m_flags[i] & fEntitySpriteBatch == 0 &&
           s_objects.m_flags[i] & fEntitySprite == 0 ){
            continue;
        }

        sprite_data_t spr_data;
        spr_data.posX   = s_objects.m_positions[i].x;
        spr_data.posY   = s_objects.m_positions[i].y;
        spr_data.angle  = s_objects.m_positions[i].angle;
        spr_data.colR   = s_objects.m_sprites[i].r;
        spr_data.colG   = s_objects.m_sprites[i].g;
        spr_data.colB   = s_objects.m_sprites[i].b;
        spr_data.sprite = s_objects.m_sprites[i].index;
        spr_data.width  = s_objects.m_sprites[i].width;
        spr_data.height = s_objects.m_sprites[i].height;
        spr_data.border = s_objects.m_flags[i] & fEntitySpriteBorder;

        if(s_objects.m_flags[i] & fEntitySpriteBatch){
            m_render_system->add_sprite_to_batch(spr_data);
        }
        else if(s_objects.m_flags[i] & fEntitySprite)
            m_render_system->add_to_frame(std::move(spr_data));
    }
}

void Game::draw_output(){
    auto r = SDL_GetRenderer(m_window);
    SdlWrapper::W_SDL_SetRenderDrawColor(r, {0x18, 0x18, 0x18, 0xFF});
    SDL_RenderClear(r);

    m_render_system->render_batch();
    m_render_system->render();
    // TextureTest::test_texture_rendering(r, "assets/rocket_tower.png");
    SDL_Color cursor_circle_color = {0xF7, 0x62, 0x18, 0x80};
    bool res = Circle::render_circle_filled(r, m_cursor_pos.x, m_cursor_pos.y, 100, cursor_circle_color);
    if(!res){
        SDL_Log("render circle: %s", SDL_GetError());
    }
    char fps_str[256];
    sprintf(fps_str, "FPS: %.2f", fps);
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    SDL_RenderDebugText(r, 0, 20, fps_str);
    SDL_RenderPresent(SDL_GetRenderer(m_window));
}

bool Game::add_tower(Entities& objects, TowerType type, const TileComponent& tile){
    auto id = objects.add_object("tower");
    auto tile_size = m_cur_level.get_tile_size();

    objects.m_positions[id].angle = 0;
    objects.m_positions[id].x = tile.pos.x;
    objects.m_positions[id].y = tile.pos.y;
    objects.m_flags[id] |= fEntityPosition;

    objects.m_sprites[id].index = 4;
    objects.m_sprites[id].scale = 1;
    objects.m_sprites[id].width = tile_size.x;
    objects.m_sprites[id].height = tile_size.y;
    objects.m_sprites[id].r = 0.6;
    objects.m_sprites[id].g = 0.6;
    objects.m_sprites[id].b = 0.6;
    objects.m_flags[id] |= fEntitySprite;

    objects.m_borders[id].x_min = 0;
    objects.m_borders[id].x_max = 0;
    // objects.m_borders[id].y_min = (tile.pos.y - tile_size.y) / 2;
    objects.m_borders[id].y_min = (tile.pos.y - tile_size.y / 4);
    objects.m_borders[id].y_max = (tile.pos.y + tile_size.y / 4);
    objects.m_flags[id] |= fEntityMapBorder;

    objects.m_moves[id].velX = 0;
    objects.m_moves[id].velY = RandomFloat(.3, .5);
    // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
    objects.m_flags[id] |= fEntityMove;

    objects.m_types[id] = fTower;

    return m_cur_level.set_tile_occupied(tile.row, tile.column, 1);

}

void Game::resize_callback(){
    if(!&m_cur_level) return;

    auto res    = m_cur_level.get_resolution();
    auto tokens = m_cur_level.get_tokens();
    auto rows = res.first;
    auto columns = res.second;

    int tile_width  = (m_width - 16*10) / res.second;
    int tile_height = (m_height - 9*10) / res.first;
    for(size_t i = 0; i < s_objects.size(); i++){
        s_objects.m_sprites[i].width = tile_width;
        s_objects.m_sprites[i].height = tile_height;
    }
}
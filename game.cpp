
#include "game.h"
#include "figures.h"
#include "wrapper.h"
#include "math.h"
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include "test.h"

float fps;

Game::Game()
    :m_window(nullptr)
    ,m_renderer(nullptr)
    ,m_cursor(nullptr)
    ,m_cursor_pos()
    ,m_width(16 * 120)
    ,m_height(9 * 120)
    ,m_target()
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
    SDL_FPoint level_size = {m_width * 0.8f, m_height * 0.8f};
    SDL_FPoint level_pos = {(m_width - level_size.x) / 2, (m_height - level_size.y) / 2};
    m_levels.emplace_back("assets/map/level2.map", level_pos, level_size);
    m_levels.emplace_back("assets/map/level1.map", level_pos, level_size);
    m_cur_level = m_levels.back();
    m_levels.pop_back();

    m_objects.reserve(100);

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

            EntityID obj = m_objects.add_object("tile");
            int sprite_index = std::atoi(tokens[columns * r + c].c_str());
            {
                auto tile = m_cur_level.get_tile(r, c);
                auto tile_pos = tile.pos;

                switch(sprite_index) {
                case TileComponent::grass_tile_int:
                    m_objects.m_sprites[obj].index = fTile;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = tile_height;
                    m_objects.m_sprites[obj].r = 0.6;
                    m_objects.m_sprites[obj].g = 0.6;
                    m_objects.m_sprites[obj].b = 0.6;
                    m_objects.m_flags[obj] |= fEntitySpriteBatch | fEntitySpriteBorder;
                break;
                case TileComponent::road_tile_int:
                    m_objects.m_sprites[obj].index = fRoad;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = tile_height;
                    m_objects.m_sprites[obj].r = 0.6;
                    m_objects.m_sprites[obj].g = 0.6;
                    m_objects.m_sprites[obj].b = 0.6;
                    m_objects.m_flags[obj] |= fEntitySpriteBatch | fEntitySpriteBorder;
                break;
                case TileComponent::spawner_tile_int:
                    m_objects.m_sprites[obj].index = fSpawner;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = 0;
                    m_objects.m_sprites[obj].r = 0.6;
                    m_objects.m_sprites[obj].g = 0.6;
                    m_objects.m_sprites[obj].b = 0.6;
                    m_objects.m_flags[obj] |= fEntitySprite | fEntitySpriteBorder;
                break;
                case TileComponent::castle_tile_int:
                    m_objects.m_sprites[obj].index = fCastle;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = 0;
                    m_objects.m_sprites[obj].r = 0.6;
                    m_objects.m_sprites[obj].g = 0.6;
                    m_objects.m_sprites[obj].b = 0.6;
                    m_objects.m_flags[obj] |= fEntitySprite | fEntitySpriteBorder;
                break;
                    // s_objects.m_flags[obj] |= fEntitySprite;
                }

                m_objects.m_positions[obj].x = tile_pos.x;
                m_objects.m_positions[obj].y = tile_pos.y;
                m_objects.m_positions[obj].angle = 0;
                m_objects.m_flags[obj] |= fEntityPosition;

                m_objects.m_moves[obj].speed = RandomFloat(.1, 3.);
                // m_objects.m_moves[obj].velY = RandomFloat(.1, 3.);
                m_objects.m_moves[obj].rotation_angle = RandomFloat(-1.f, 1.f);
                // s_objects.m_flags[obj] |= fEntityMove;

                m_objects.m_borders[obj].x_min = tile_width * c;
                m_objects.m_borders[obj].x_max = tile_width * c;
                m_objects.m_borders[obj].y_min = tile_height * r;
                m_objects.m_borders[obj].y_max = tile_height * r;
                m_objects.m_flags[obj] |= fEntityMapBorder;

                m_objects.m_types[obj] = fTile;
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
    SDL_FPoint mouse_pos = {mouse_event.x, mouse_event.y};
    Vector2D   mouse_vec = {mouse_event.x, mouse_event.y};

    if(mouse_event.button == SDL_BUTTON_LEFT){
        auto tile = m_cur_level.get_tile(mouse_pos);
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
    else if(mouse_event.button == SDL_BUTTON_RIGHT){
        add_target(objects, mouse_vec);
    }
    else if(mouse_event.button == SDL_BUTTON_MIDDLE){
        if(m_cur_level.is_road_tile(mouse_pos)){

            spawn_enemies_targeted(objects, {RandomFloat(10., 500), RandomFloat(10., 500)},
            mouse_pos);
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
                handle_mouse_event(m_objects, event.button);
            break;
        }
    }
}

void Game::update_game(float deltatime){
    // std::cout << deltatime <<"\n";
    // m_animation_system.update(s_objects, deltatime);
    m_move_system.update(m_objects, m_cur_level, deltatime);
    m_castle_damage_system.update(m_objects, m_cur_level, deltatime);

    m_render_system->clean_batch_frame();
    m_render_system->clean_frame();
    for(size_t i = 0, n = m_objects.size(); i != n; i++){
        if(m_objects.m_flags[i] & fEntitySpriteBatch == 0 &&
           m_objects.m_flags[i] & fEntitySprite == 0 ){
            continue;
        }

        sprite_data_t spr_data;
        spr_data.posX   = m_objects.m_positions[i].x;
        spr_data.posY   = m_objects.m_positions[i].y;
        spr_data.angle  = m_objects.m_positions[i].angle;
        spr_data.colR   = m_objects.m_sprites[i].r;
        spr_data.colG   = m_objects.m_sprites[i].g;
        spr_data.colB   = m_objects.m_sprites[i].b;
        spr_data.sprite = m_objects.m_sprites[i].index;
        spr_data.width  = m_objects.m_sprites[i].width;
        spr_data.height = m_objects.m_sprites[i].height;
        spr_data.border = m_objects.m_flags[i] & fEntitySpriteBorder;
        if(m_objects.m_types[i] & fEnemy){
            spr_data.flag = fCenterSprite;
        }
        else{
            spr_data.flag = fUpperLeftSprite;

        }

        if(m_objects.m_flags[i] & fEntitySpriteBatch){
            m_render_system->add_sprite_to_batch(spr_data);
        }
        else if(m_objects.m_flags[i] & fEntitySprite)
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

    objects.m_positions[id].angle = 90;
    objects.m_positions[id].x = tile.pos.x;
    objects.m_positions[id].y = tile.pos.y;
    objects.m_flags[id] |= fEntityPosition;

    objects.m_sprites[id].index = fTower;
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

    objects.m_moves[id].speed = 0;
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

    // int tile_width  = (m_width - 16*10) / res.second;
    // int tile_height = (m_height - 9*10) / res.first;
    // for(size_t i = 0; i < s_objects.size(); i++){
    //     s_objects.m_sprites[i].width = tile_width;
    //     s_objects.m_sprites[i].height = tile_height;
    // }
}

void Game::add_target(Entities& objects, const Vector2D& pos){
    
    m_target = pos;
    objects.remove_object("target");

    auto id = objects.add_object("target");
    auto tile_size = m_cur_level.get_tile_size();

    objects.m_positions[id].angle = 0;
    objects.m_positions[id].x = pos.x;
    objects.m_positions[id].y = pos.y;
    objects.m_flags[id] |= fEntityPosition;

    objects.m_sprites[id].index = fTarget;
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
    objects.m_borders[id].y_min = (pos.y - tile_size.y / 4);
    objects.m_borders[id].y_max = (pos.y + tile_size.y / 4);
    objects.m_flags[id] |= fEntityMapBorder;

    objects.m_moves[id].speed = 0;
    // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
    objects.m_flags[id] |= fEntityMove;

    objects.m_types[id] = fTarget;

    std::cout << "New Target" << std::endl;

}

void Game::spawn_enemies_targeted(Entities& objects, const SDL_FPoint& target, const SDL_FPoint& spawn_pos){
    auto id = objects.add_object("enemy");
    auto tile_size = m_cur_level.get_tile_size();
    auto path = m_cur_level.get_road_tiles();

    objects.m_positions[id].angle = 0;
    objects.m_positions[id].x = spawn_pos.x;
    objects.m_positions[id].y = spawn_pos.y;
    objects.m_flags[id] |= fEntityPosition;

    objects.m_sprites[id].index = fEnemy;
    objects.m_sprites[id].scale = 1;
    objects.m_sprites[id].width = tile_size.x * 0.5;
    objects.m_sprites[id].height = tile_size.y * 0.5;
    objects.m_sprites[id].r = 0.6;
    objects.m_sprites[id].g = 0.6;
    objects.m_sprites[id].b = 0.6;
    objects.m_flags[id] |= fEntitySprite;

    objects.m_borders[id].x_min = 0;
    objects.m_borders[id].x_max = 0;
    // objects.m_borders[id].y_min = (tile.pos.y - tile_size.y) / 2;
    objects.m_borders[id].y_min = (spawn_pos.y - tile_size.y / 4);
    objects.m_borders[id].y_max = (spawn_pos.y + tile_size.y / 4);
    objects.m_flags[id] |= fEntityMapBorder;

    objects.m_moves[id].speed = RandomFloat(10., 20.);
    objects.m_moves[id].targeted = 1;
    objects.m_moves[id].target = m_target;

    // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
    objects.m_flags[id] |= fEntityMove;

    objects.m_types[id] = fEnemy;


}
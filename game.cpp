
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
    ,m_spawn_system()
    ,m_current_ticks(0)
    ,m_hud_system()
    ,m_selected_tower(nullptr)
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
    SDL_FPoint level_size = {1024, 1024};
    SDL_FPoint level_pos = {(m_width - level_size.x) / 2, (m_height - level_size.y) / 2};
    m_levels.emplace_back("assets/map/level2.map", level_pos, level_size);
    m_levels.emplace_back("assets/map/level1.map", level_pos, level_size);
    m_cur_level = m_levels.back();
    m_levels.pop_back();

    m_objects.reserve(256);

    //Register entities types
    register_type(SpriteType::UNDEFINED,{"assets/quot_stickers.png"});
    register_type(SpriteType::TILE,             {"assets/grass.png"});
    register_type(SpriteType::ROAD,             {"assets/dirt_road.png"});
    register_type(SpriteType::TOWER,            {"assets/tower.bmp"});
    register_type(SpriteType::SPAWNER,          {"assets/dirt_road.png",
                                                 "assets/ground1.bmp"});
    register_type(SpriteType::CASTLE,           {"assets/dirt_road.png"});
    
    register_type(SpriteType::ICE_TOWER,        {"assets/ice_tower.png"});
    register_type(SpriteType::FIRE_TOWER,       {"assets/fire/fire_tower1.png",
                                                 "assets/fire/fire_tower2.png",
                                                 "assets/fire/fire_tower3.png",
                                                 "assets/fire/fire_tower4.png",
                                                 "assets/fire/fire_tower5.png",
                                                 "assets/fire/fire_tower3.png"});
    register_type(SpriteType::VIKING,            {"assets/enemies/viking.png"});
    register_type(SpriteType::DRAGONIT,          {"assets/enemies/dragonit.png"});
    register_type(SpriteType::BEE,               {"assets/enemies/bee.png"});
    register_type(SpriteType::SERJANT,           {"assets/enemies/serjant.png"});
    register_type(SpriteType::TANK,              {"assets/enemies/tank.png"});
    
    register_type(SpriteType::FIRE_PROJECTILE,       {"assets/fire/fireball.png"});
    register_type(SpriteType::FIRE_AOE,              {});
    register_type(SpriteType::ICE_PROJECTILE,       {"assets/ice_bullet.png"});

    register_type(SpriteType::CASTLE_DECOR,     {"assets/statue.bmp"});
    register_type(SpriteType::SPAWNER_DECOR,    {"assets/spawner.bmp"});
    register_type(SpriteType::BUSH0_DECOR,      {"assets/bush0.bmp"});
    register_type(SpriteType::BUSH1_DECOR,      {"assets/bush1.bmp"});
    register_type(SpriteType::BUSH2_DECOR,      {"assets/bush2.bmp"});

    register_type(SpriteType::HEALTH_BAR,       {});
    
    register_type(SpriteType::HUD_LAYOUT,       {});
    register_type(SpriteType::HEARTH,           {"assets/hearth.png", "assets/broken_hearth.png"});
    register_type(SpriteType::COINS,            {"assets/coins.png"});

    register_towers();

    load_level_tiles();
    load_decorations();
    load_towers();
    
    load_hud();
}

void Game::destroy_game(){
    delete m_render_system;
}

void Game::loop(){
    Uint64 lastTime = SDL_GetTicks();
    int frames = 0;
    Uint64 interval = 500; // 1 секунда в мс
    while(m_running){
        Uint64 frame_ticks = SDL_GetTicks();
        frames++;
        if (frame_ticks - lastTime >= interval) {
            fps = static_cast<float>(frames) / (frame_ticks - lastTime) * 1000.0f;
            lastTime = frame_ticks;
            frames = 0;
        }

        handle_input();
        update_game();
        draw_output();
        int sleep = 8 - (SDL_GetTicks() - frame_ticks);
        if(sleep > 0){
            SDL_Delay(sleep); // Ограничивает ~120 FPS
        }
    }
}

void Game::handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event){
    SDL_FPoint mouse_pos = {mouse_event.x, mouse_event.y};
    Vector2D   mouse_vec = {mouse_event.x, mouse_event.y};

    if(mouse_event.button == SDL_BUTTON_LEFT){
        if(m_cur_level.is_occupied(mouse_vec)) return;

        if(m_components_data[ComponentType::PLAYER_GOLD] > 0 &&
           m_selected_tower != nullptr){

            if(objects.add_tower(m_cur_level, m_selected_tower->type, mouse_vec) > 0){
                std::cout << "Added tower to tile (" << mouse_pos.x << ", "
                                                        << mouse_pos.y <<
                                                    ")" << std::endl;
                auto callback = m_components_callbacks[ComponentType::PLAYER_GOLD];
                callback(m_selected_tower->cost);
                m_selected_tower = nullptr;
            }
            else{
                std::cout << "Cannot add tower to " << mouse_event.x << " " << mouse_event.y <<
                            ". Tile already occupied or not in level map" << std::endl;
            }
        }
    }
    else if(mouse_event.button == SDL_BUTTON_RIGHT){
        if(m_cur_level.is_road_tile(mouse_pos)){
            auto id = objects.spawn_enemies_targeted(m_cur_level,
                                                     m_cur_level.get_castle_pos(),
                                                     mouse_pos,
                                                     static_cast<SpriteType>(static_cast<int>(SpriteType::VIKING) + rand()%5));

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
            case SDL_EVENT_KEY_DOWN:
                auto scancode = event.key.scancode;
                if(m_towers_scancode.count(scancode) > 0){
                    m_selected_tower = &m_towers_scancode[scancode];
                    SDL_Log("Selected Tower %d", m_selected_tower->type);
                }
            break;
        }
    }
}

void Game::update_game(){
    float deltatime = std::min(static_cast<float>(SDL_GetTicks() - m_current_ticks) / 1000, 0.008f);
    m_current_ticks = SDL_GetTicks();
    // std::cout << deltatime <<"\n";
    m_spawn_system.update(m_objects, m_cur_level, deltatime);
    m_firing_system.update(m_objects, m_cur_level, deltatime);
    m_move_system.update(m_objects, m_cur_level, deltatime);
    m_enemy_collision_system.update(m_objects, m_cur_level, deltatime);
    m_castle_damage_system.update(m_objects, m_cur_level, deltatime, m_components_callbacks.at(CASTLE_HEALTH));
    m_animation_system.update(m_objects, deltatime);
    m_hud_system.update(deltatime);
    // m_render_system->clean_batch_frame();
    // m_render_system->clean_frame();
    
}

void Game::draw_output(){
    auto r = SDL_GetRenderer(m_window);
    SdlWrapper::W_SDL_SetRenderDrawColor(r, {0x18, 0x18, 0x18, 0xFF});
    SDL_RenderClear(r);

    // m_render_system->render_batch();
    m_render_system->render(m_objects);
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

void Game::load_level_tiles(){
    //Tiles
    auto res = m_cur_level.get_resolution();
    auto tokens = m_cur_level.get_tokens();
    auto size = m_cur_level.get_size();
    auto tile_size = m_cur_level.get_tile_size();
    auto level_position = m_cur_level.get_position();
    auto rows = res.first;
    auto columns = res.second;

    int tile_width = tile_size.x;
    int tile_height = tile_size.y;
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < columns; c++){

            EntityID obj = m_objects.add_object("tile");
            int sprite_index = std::atoi(tokens[columns * r + c].c_str());
            {
                auto tile = m_cur_level.get_tile(r, c);
                auto tile_pos = tile.pos;

                m_objects.m_positions[obj].x = tile_pos.x;
                m_objects.m_positions[obj].y = tile_pos.y;
                m_objects.m_sprites[obj].posX = tile_pos.x;
                m_objects.m_sprites[obj].posY = tile_pos.y;
                m_objects.m_positions[obj].angle = 0;

                switch(sprite_index) {
                case TileComponent::grass_tile_int:
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = tile_height;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].colR = 0.6;
                    m_objects.m_sprites[obj].colG = 0.6;
                    m_objects.m_sprites[obj].colB = 0.6;
                    m_objects.m_sprites[obj].angle = 0;
                    m_objects.m_sprites[obj].flag = fUpperLeftSprite;
                    m_objects.m_sprites[obj].layer = SpriteLayer::BACKGROUND;
                    m_objects.m_sprites[obj].type = SpriteType::TILE;
                    m_objects.m_systems[obj] |= eSpriteSystem;
                break;
                case TileComponent::road_tile_int:
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = tile_height;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].colR = 0.6;
                    m_objects.m_sprites[obj].colG = 0.6;
                    m_objects.m_sprites[obj].colB = 0.6;
                    m_objects.m_sprites[obj].angle = 0;
                    m_objects.m_sprites[obj].flag = fUpperLeftSprite;
                    m_objects.m_sprites[obj].layer = SpriteLayer::BACKGROUND;
                    m_objects.m_sprites[obj].type = SpriteType::ROAD;
                    m_objects.m_systems[obj] |= eSpriteSystem;
                break;
                case TileComponent::spawner_tile_int:
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = tile_height;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].colR = 0.6;
                    m_objects.m_sprites[obj].colG = 0.6;
                    m_objects.m_sprites[obj].colB = 0.6;
                    m_objects.m_sprites[obj].angle = 0;
                    m_objects.m_sprites[obj].flag = fUpperLeftSprite;
                    m_objects.m_sprites[obj].layer = SpriteLayer::BACKGROUND;
                    m_objects.m_sprites[obj].type = SpriteType::SPAWNER;
                    m_objects.m_systems[obj] |= eSpriteSystem;
                break;
                case TileComponent::castle_tile_int:
                    m_objects.m_sprites[obj].width = tile_width;
                    m_objects.m_sprites[obj].height = tile_height;
                    m_objects.m_sprites[obj].scale = 1;
                    m_objects.m_sprites[obj].colR = 0.6;
                    m_objects.m_sprites[obj].colG = 0.6;
                    m_objects.m_sprites[obj].colB = 0.6;
                    m_objects.m_sprites[obj].angle = 0;
                    m_objects.m_sprites[obj].flag = fUpperLeftSprite;
                    m_objects.m_sprites[obj].layer = SpriteLayer::BACKGROUND;
                    m_objects.m_sprites[obj].type = SpriteType::CASTLE;
                    m_objects.m_systems[obj] |= eSpriteSystem;
                break;
                }

                m_objects.m_systems[obj] |= ePositionSystem;

                m_objects.m_moves[obj].speed = RandomFloat(.1, 3.);
                // m_objects.m_moves[obj].velY = RandomFloat(.1, 3.);
                m_objects.m_moves[obj].rotation_angle = RandomFloat(-1.f, 1.f);
                // s_objects.m_flags[obj] |= fEntityMove;

                m_objects.m_borders[obj].x_min = tile_width * c;
                m_objects.m_borders[obj].x_max = tile_width * c;
                m_objects.m_borders[obj].y_min = tile_height * r;
                m_objects.m_borders[obj].y_max = tile_height * r;
                m_objects.m_systems[obj] |= eMapBorderSystem;

                m_objects.m_types[obj] = EntityGlobalType::BACKGROUND_ENTITY;
            }


        }

    }
    for(size_t i = 0, n = m_objects.size(); i != n; i++){
        if((m_objects.m_systems[i] & eSpriteBatchSystem) == 0 &&
           (m_objects.m_systems[i] & eSpriteSystem) == 0 ){
            continue;
        }
    }
}

void Game::load_decorations(){
    auto tile_size = m_cur_level.get_tile_size();
    auto castle_pos = m_cur_level.get_castle_pos();
    auto spawner_pos = m_cur_level.get_spawner_pos();
    auto resolution = m_cur_level.get_resolution();

//WARNING: sum of the decor elements should be less then grass tiles quantity otherwise deadlock
    load_decor_sprite(castle_pos, SpriteType::CASTLE_DECOR);
    load_decor_sprite(spawner_pos, SpriteType::SPAWNER_DECOR);
    load_decor_random_sprites(SpriteType::BUSH0_DECOR, 33);
    load_decor_random_sprites(SpriteType::BUSH1_DECOR, 44);
    load_decor_random_sprites(SpriteType::BUSH2_DECOR, 40);

}

void Game::load_decor_sprite(const Vector2D& pos, SpriteType type){
    if(m_cur_level.is_decor_occupied(pos)){
        SDL_Log("ERROR: fail to load decor sprite");
        return;
    }
    //TODO refactor get functions and mb remove std::optional 
    auto tile = m_cur_level.get_tile(pos.get_sdl_point()).value();
    auto tile_pos = tile.pos;
    auto tile_size = m_cur_level.get_tile_size();

    SDL_FRect dst = {tile_pos.x, tile_pos.y, tile_size.x, tile_size.y};

    EntityID obj = m_objects.add_object("decor");
    
    SpriteComponent& sprite = m_objects.m_sprites[obj];
    float height_shift = 0.3; // Shifting y component to increase texture height
    float width_shift = 0.0; // Shifting y component to increase texture height
    sprite.posX = dst.x - (dst.w * width_shift);
    sprite.posY = dst.y - (dst.h * height_shift);
    sprite.width = dst.w * (1 + width_shift);
    sprite.height = dst.h * (1 + height_shift);
    sprite.scale = 1;
    sprite.colR = 0.6;
    sprite.colG = 0.6;
    sprite.colB = 0.6;
    sprite.angle = 0;
    sprite.flag = fUpperLeftSprite;
    sprite.layer = SpriteLayer::DECORATION;
    sprite.type = type;
    m_objects.m_systems[obj] |= eSpriteSystem;
    m_cur_level.set_tile_decor_occupied(tile.row, tile.column, 1);
}

void Game::load_decor_random_sprites(SpriteType type, size_t size){
    auto resolution = m_cur_level.get_resolution();
    auto tile_size = m_cur_level.get_tile_size();
    for(int i = 0; i < size; i++){
        auto row = rand() % resolution.first;
        auto column = rand() % resolution.second;
        if(m_cur_level.is_road_tile(row, column) || m_cur_level.is_decor_occupied(row, column)){
            i--;
            continue;
        }
        auto pos = m_cur_level.get_tile_position(row, column);
        std::vector<SDL_FRect> subtiles = {{pos.x, pos.y, tile_size.x / 2, tile_size.y / 2},
                                    {pos.x + tile_size.x / 2, pos.y, tile_size.x / 2, tile_size.y / 2},
                                    {pos.x, pos.y + tile_size.y / 2, tile_size.x / 2, tile_size.y / 2},
                                    {pos.x + tile_size.x / 2, pos.y + tile_size.y / 2, tile_size.x / 2, tile_size.y / 2}};
        SDL_FRect& dst = subtiles[rand()%4];

        EntityID obj = m_objects.add_object("decor");
        
        SpriteComponent& sprite = m_objects.m_sprites[obj];
        float height_shift = 0.0; // Shifting y component to increase texture height
        float width_shift = 0.0; // Shifting y component to increase texture height
        sprite.posX = dst.x - (dst.w * width_shift);
        sprite.posY = dst.y - (dst.h * height_shift);
        sprite.width = dst.w * (1 + width_shift);
        sprite.height = dst.h * (1 + height_shift);
        sprite.scale = RandomFloat(0.7f, 1.5f);
        sprite.colR = 0.6;
        sprite.colG = 0.6;
        sprite.colB = 0.6;
        sprite.angle = 0;
        sprite.flag = fUpperLeftSprite;
        sprite.layer = SpriteLayer::DECORATION;
        sprite.type = type;
        m_objects.m_systems[obj] |= eSpriteSystem;
        m_cur_level.set_tile_decor_occupied(row, column, 1);
    }
}

void Game::load_towers(){
    auto id = m_objects.add_tower(m_cur_level, TowerType::FIRE_TOWER_DATA, {761.657, 382.826});
    id = m_objects.add_tower(m_cur_level, TowerType::ICE_TOWER_DATA, {942.076, 378.737});
    id = m_objects.add_tower(m_cur_level, TowerType::FIRE_TOWER_DATA, {615.781, 267.516});
}

void Game::register_type(SpriteType type, const std::vector<std::string>& textures){

    m_render_system->register_type_sprite(type, textures);
    m_animation_system.register_type(type, textures.size());
}

void Game::load_hud(){
    m_hud_system = HudSystem({0, 0}, (m_width - m_cur_level.get_size().x) / 2, m_height);
    m_hud_system.m_hud_layer.set_grid(15, 2);
    
    load_layout();
    load_hearth();
    load_coins();
}

void Game::load_layout(){
    auto id = m_objects.add_object("hud_layout");
    Vector2D pos = m_hud_system.m_hud_layer.pos;

    m_objects.m_positions[id].angle = 0;
    m_objects.m_positions[id].x = pos.x;
    m_objects.m_positions[id].y = pos.y;
    m_objects.m_systems[id] |= ePositionSystem;

    m_objects.m_sprites[id].posX = pos.x;
    m_objects.m_sprites[id].posY = pos.y;
    m_objects.m_sprites[id].width = m_hud_system.m_hud_layer.rect.w;
    m_objects.m_sprites[id].height = m_hud_system.m_hud_layer.rect.h;
    m_objects.m_sprites[id].scale = 1;
    m_objects.m_sprites[id].colR = 0.6;
    m_objects.m_sprites[id].colG = 0.6;
    m_objects.m_sprites[id].colB = 0.6;
    m_objects.m_sprites[id].angle = 0;
    m_objects.m_sprites[id].flag = fUpperLeftSprite;
    m_objects.m_sprites[id].layer = SpriteLayer::HUD;
    m_objects.m_sprites[id].type = SpriteType::HUD_LAYOUT;
    m_objects.m_sprites[id].anim_index = 0;
    m_objects.m_systems[id] |= eSpriteSystem;
    m_objects.m_types[id] = EntityGlobalType::HUD_ENTITY;
}

void Game::load_hearth(){
    m_components_data.emplace(ComponentType::CASTLE_HEALTH, 100.f);
    m_components_callbacks.emplace(ComponentType::CASTLE_HEALTH, [this](float value){
        auto it = m_components_data.find(ComponentType::CASTLE_HEALTH);
        if(it != m_components_data.end()){
            it->second -= value;
            SDL_Log("Health %.1f", it->second);
            EntityID hearth_id = m_objects.get_object(SpriteType::HEARTH);
            m_objects.m_sprites[hearth_id].anim_index = (m_objects.m_sprites[hearth_id].anim_index + 1) % 2;
        }
    });

    auto id = m_objects.add_object("hud_hearth");
    auto grid = m_hud_system.m_hud_layer.get_grid();
    auto component = m_hud_system.m_hud_layer.add_component(grid.first - 1, 0);

    m_objects.m_positions[id].angle = 0;
    m_objects.m_positions[id].x = component.pos.x;
    m_objects.m_positions[id].y = component.pos.y;
    m_objects.m_systems[id] |= ePositionSystem;

    m_objects.m_sprites[id].posX = component.pos.x;
    m_objects.m_sprites[id].posY = component.pos.y;
    m_objects.m_sprites[id].width = 64;
    m_objects.m_sprites[id].height = 64;
    m_objects.m_sprites[id].scale = 1;
    m_objects.m_sprites[id].colR = 0.6;
    m_objects.m_sprites[id].colG = 0.6;
    m_objects.m_sprites[id].colB = 0.6;
    m_objects.m_sprites[id].angle = 0;
    m_objects.m_sprites[id].flag = fUpperLeftSprite;
    m_objects.m_sprites[id].layer = SpriteLayer::HUD;
    m_objects.m_sprites[id].type = SpriteType::HEARTH;
    m_objects.m_sprites[id].anim_index = 0;
    m_objects.m_systems[id] |= eSpriteSystem;
    m_objects.m_types[id] = EntityGlobalType::HUD_ENTITY;

}

void Game::load_coins(){
    m_components_data.emplace(ComponentType::PLAYER_GOLD, 100.f);
    m_components_callbacks.emplace(ComponentType::PLAYER_GOLD, [this](float value){
        auto it = m_components_data.find(ComponentType::PLAYER_GOLD);
        if(it != m_components_data.end()){
            it->second -= value;
            SDL_Log("Gold %.1f", it->second);
        }
    });

    auto id = m_objects.add_object("hud_coins");
    auto grid = m_hud_system.m_hud_layer.get_grid();
    auto component = m_hud_system.m_hud_layer.add_component(grid.first - 2, 0);

    m_objects.m_positions[id].angle = 0;
    m_objects.m_positions[id].x = component.pos.x;
    m_objects.m_positions[id].y = component.pos.y;
    m_objects.m_systems[id] |= ePositionSystem;

    m_objects.m_sprites[id].posX = component.pos.x;
    m_objects.m_sprites[id].posY = component.pos.y;
    m_objects.m_sprites[id].width = 64;
    m_objects.m_sprites[id].height = 64;
    m_objects.m_sprites[id].scale = 1;
    m_objects.m_sprites[id].colR = 0.6;
    m_objects.m_sprites[id].colG = 0.6;
    m_objects.m_sprites[id].colB = 0.6;
    m_objects.m_sprites[id].angle = 0;
    m_objects.m_sprites[id].flag = fUpperLeftSprite;
    m_objects.m_sprites[id].layer = SpriteLayer::HUD;
    m_objects.m_sprites[id].type = SpriteType::COINS;
    m_objects.m_sprites[id].anim_index = 0;
    m_objects.m_systems[id] |= eSpriteSystem;
    m_objects.m_types[id] = EntityGlobalType::HUD_ENTITY;
}

void Game::register_towers(){

    auto fire_tower = m_objects.create_tower_descr(TowerType::FIRE_TOWER_DATA);
    m_towers_scancode.emplace(SDL_SCANCODE_1, fire_tower);

    auto ice_tower = m_objects.create_tower_descr(TowerType::ICE_TOWER_DATA);
    m_towers_scancode.emplace(SDL_SCANCODE_2, ice_tower);
}
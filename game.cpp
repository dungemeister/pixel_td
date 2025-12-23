
#include "game.h"
#include "figures.h"
#include "wrapper.h"
#include "math.h"
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include "test.h"
#include "palette.h"
#include <memory>
#include <sstream>
#include <iomanip>


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
    ,m_selected_tower(nullptr)
    ,m_state(GameState::Gameplay)
    ,m_pause_menu()
    ,m_running(1)
    ,m_info_font(nullptr)
    ,m_descriptions_layout(nullptr)
    ,m_map_layout(nullptr)
{
    init();
}

Game::~Game(){
    destroy_game();
}

void Game::init(){
    init_render_system();
    init_pause_menu();
    init_game();
}

void Game::init_pause_menu(){

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
    res = TTF_Init();
    if(!res){
        SDL_Log("%s", SDL_GetError());
        assert(res != 0);
    }

    m_info_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", m_hud_font_size);
    if(!m_info_font){
        SDL_Log("%s", SDL_GetError());
        assert(m_info_font != nullptr);
    }

    m_window = SDL_CreateWindow("Anime TD", m_width, m_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_render_system = std::make_unique<RenderSystem>(m_window);

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

    // SDL_FRect map_rect = {level_pos.x - 3 * level_size.x / 64,
    //                       level_pos.y - 3 * level_size.y / 64,
    //                       level_size.x + 3 * level_size.x / 64 * 2,
    //                       level_size.y  + 3 * level_size.y / 64 * 2};
    // m_map_layout = new UILayout(this, SDL_GetRenderer(m_window), map_rect);
    // UIImage* map_frame_img = new UIImage("assets/map_frame.png", m_map_layout);
    // map_frame_img->SetDestSize({map_rect.w, map_rect.h});
    // m_map_layout->PushBackWidgetHorizontal(map_frame_img);

    //Register entities types
    register_type(SpriteType::UNDEFINED,             {"assets/quot_stickers.png"});
    register_type(SpriteType::TILE,                  {"assets/grass.png"});
    register_type(SpriteType::ROAD,                  {"assets/dirt_road.png"});
    register_type(SpriteType::TOWER,                 {"assets/tower.bmp"});
    register_type(SpriteType::SPAWNER,               {"assets/dirt_road.png",
                                                      "assets/ground1.bmp"});
    register_type(SpriteType::CASTLE,                {"assets/dirt_road.png"});
    
    register_type(SpriteType::ICE_TOWER,             {"assets/ice/ice_tower.png"});
    register_type(SpriteType::FIRE_TOWER,            {"assets/fire/fire_tower1.png",
                                                      "assets/fire/fire_tower2.png",
                                                      "assets/fire/fire_tower3.png",
                                                      "assets/fire/fire_tower4.png",
                                                      "assets/fire/fire_tower5.png",
                                                      "assets/fire/fire_tower3.png"});
    // register_type(SpriteType::POISON_TOWER,          {"assets/poison_tower.png"});
    register_type(SpriteType::POISON_TOWER,          {"assets/poison/poison1_tower.png"});
    register_type(SpriteType::CLOUD_TOWER,           {"assets/cloud/cloud_tower.png"});

    register_type(SpriteType::VIKING_SPRITE,         {"assets/enemies/viking.png"});
    register_type(SpriteType::DRAGONIT_SPRITE,       {"assets/enemies/dragonit.png"});
    register_type(SpriteType::BEE_SPRITE,            {"assets/enemies/bee.png"});
    register_type(SpriteType::SERJANT_SPRITE,        {"assets/enemies/serjant_walk1.png",
                                                      "assets/enemies/serjant_walk2.png",
                                                      "assets/enemies/serjant_walk3.png",
                                                      "assets/enemies/serjant_walk4.png"});
    register_type(SpriteType::TANK_SPRITE,           {"assets/enemies/tank.png"});
    
    register_type(SpriteType::FIRE_PROJECTILE,       {"assets/fire/fireball.png"});
    register_type(SpriteType::FIRE_AOE,              {});
    register_type(SpriteType::ICE_PROJECTILE,        {"assets/ice/ice_bullet.png"});
    register_type(SpriteType::POISON_PROJECTILE,     {"assets/poison/poisonous_ball.png"});
    register_type(SpriteType::CLOUD_PROJECTILE,      {"assets/cloud/cloud_projectile.png"});

    register_type(SpriteType::CASTLE_DECOR,          {"assets/statue.bmp"});
    register_type(SpriteType::SPAWNER_DECOR,         {"assets/spawner.bmp"});
    register_type(SpriteType::BUSH0_DECOR,           {"assets/bush0.bmp"});
    register_type(SpriteType::BUSH1_DECOR,           {"assets/bush1.bmp"});
    register_type(SpriteType::BUSH2_DECOR,           {"assets/bush2.bmp"});
    register_type(SpriteType::FIRECAMP_DECOR,        {"assets/firecamp.png"});

    register_type(SpriteType::HEALTH_BAR,            {});
    
    register_type(SpriteType::HUD_LAYOUT,            {});
    register_type(SpriteType::HUD_HEARTH,            {"assets/heart.png", "assets/broken_hearth.png"});
    register_type(SpriteType::HUD_COINS,             {"assets/coins.png"});
    register_type(SpriteType::HUD_HEALTH_BAR,        {});
    register_type(SpriteType::HUD_PLAYER_GOLD,       {});
    register_type(SpriteType::HUD_TEXT,              {});
    
    //Register enemies and tower types based on current level
    register_towers();
    register_enemies();

    //Load level tiles and decoration sprites
    load_level_tiles();
    load_decorations();
    load_towers();
    
    //Load HUD and UI
    load_hud();
}

void Game::destroy_game(){
    SDL_DestroyRenderer(SDL_GetRenderer(m_window));
    SDL_DestroyWindow(m_window);
    TTF_Quit();
    SDL_Quit();
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
        handle_update();
        handle_draw();
        int sleep = 8 - (SDL_GetTicks() - frame_ticks);
        if(sleep > 0){
            SDL_Delay(sleep); // Ограничивает ~120 FPS
        }
    }
}

void Game::handle_mouse_event(Entities& objects, const SDL_MouseButtonEvent& mouse_event){
    SDL_FPoint mouse_pos = {mouse_event.x, mouse_event.y};
    Vector2D   mouse_vec = {mouse_event.x, mouse_event.y};
    remove_descriptor_widgets();
    if(mouse_event.button == SDL_BUTTON_LEFT){
        
        if(m_selected_tower == nullptr &&
           m_cur_level.is_occupied(mouse_vec)){

            auto id = objects.get_object_from_position(mouse_vec);
            if(id == objects.get_empty_id()){
                SDL_Log("WARNING: Fail to get object ID from position");
                return;
            }

            if(std::holds_alternative<std::monostate>(objects.m_descriptions[id])){
                SDL_Log("WARNING: Fail to get tower description for object ID %ld", id);
                return;
            }
            update_description_layout(objects.m_sprites[id], objects.m_descriptions[id]);
            if (auto tower = std::get_if<TowerDescription>(&objects.m_descriptions[id])){
                SDL_Log("Selected tower %ld, type %d", id, tower->type);
            }
            if (auto enemy = std::get_if<EnemyDescription>(&objects.m_descriptions[id])){
                SDL_Log("Selected enemy %ld, type %d", id, enemy->type);
            }
        }
        else if(m_components_data[ComponentType::PLAYER_GOLD] > 0 &&
                m_selected_tower != nullptr &&
                !m_cur_level.is_occupied(mouse_vec)){
            
                auto callback = m_components_callbacks.find(ComponentType::PLAYER_GOLD);
                if(callback != m_components_callbacks.end()){
                    auto res = callback->second(-m_selected_tower->cost);
                    if(res){
                        objects.add_tower(m_cur_level, m_selected_tower->type, mouse_vec);
                        SDL_Log("Added tower to tile (%.1f, %.1f)", mouse_pos.x, mouse_pos.y);
                    }
                    else{
                        SDL_Log("Not enough gold. Current gold %.1f", m_components_data[ComponentType::PLAYER_GOLD]);
                    }
                }
                else{
                    SDL_Log("WARNING: cannot find player's gold callback");
                }
                m_selected_tower = nullptr;
            }
        else{
            SDL_Log("WARNING: player's gold %.1f", m_components_data[ComponentType::PLAYER_GOLD]);
        }
    }
    else if(mouse_event.button == SDL_BUTTON_RIGHT){
        if(m_cur_level.is_road_tile(mouse_pos)){
            auto id = objects.spawn_enemies_targeted(m_cur_level,
                                                     m_cur_level.get_castle_pos(),
                                                     mouse_pos,
                                                     static_cast<EnemyType>(static_cast<int>(EnemyType::VIKING) + rand()%static_cast<int>(EnemyType::ENEMY_TYPES_SIZE)));

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
                if(scancode == SDL_SCANCODE_SPACE){
                    if(m_state == GameState::PauseMenu){
                        m_state = GameState::Gameplay;
                    }
                    else if(m_state == GameState::Gameplay){
                        m_state = GameState::PauseMenu;
                    }
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
    m_castle_damage_system.update(m_objects, m_cur_level, deltatime, m_components_callbacks);
    m_animation_system.update(m_objects, deltatime);
    
}

void Game::handle_draw(){
    auto r = SDL_GetRenderer(m_window);
    SdlWrapper::W_SDL_SetRenderDrawColor(r, {0x18, 0x18, 0x18, 0xFF});
    SDL_RenderClear(r);

    // m_render_system->render_batch();
    switch(m_state){
        case Game::Gameplay:
            m_render_system->render(m_objects);
        break;
        case Game::PauseMenu:
            m_render_system->render(m_pause_menu_objects);
        break;
    }
    
    m_player_stats_layout->draw();
    m_descriptions_layout->draw();
    // TextureTest::test_texture_rendering(r, "assets/rocket_tower.png");
    // SDL_Color cursor_circle_color = {0xF7, 0x62, 0x18, 0x80};
    // bool res = Circle::render_circle_filled(r, m_cursor_pos.x, m_cursor_pos.y, 100, cursor_circle_color);
    // if(!res){
    //     SDL_Log("render circle: %s", SDL_GetError());
    // }
    char fps_str[256];
    sprintf(fps_str, "FPS: %.2f", fps);
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    SDL_RenderDebugText(r, m_width - strlen(fps_str) * 8, 20, fps_str);
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
    load_decor_random_sprites(SpriteType::BUSH0_DECOR, 33, RandomFloat(0.7f, 1.5f));
    load_decor_random_sprites(SpriteType::BUSH1_DECOR, 44, RandomFloat(0.7f, 1.5f));
    load_decor_random_sprites(SpriteType::BUSH2_DECOR, 40, RandomFloat(0.7f, 1.5f));

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
    sprite.center = {sprite.posX + sprite.width / 2,
                     sprite.posY - sprite.height / 2};
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

void Game::load_decor_random_sprites(SpriteType type, size_t size, float scale=1.f){
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
        sprite.center = {sprite.posX + sprite.width / 2,
                         sprite.posY - sprite.height / 2};
        sprite.scale = scale;
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
    id = m_objects.add_tower(m_cur_level, TowerType::POISON_TOWER_DATA, {791.7, 254.0});
}

void Game::register_type(SpriteType type, const std::vector<std::string>& sprites){

    m_render_system->register_type_sprite(type, sprites);
    m_animation_system.register_type(type, sprites.size());

}

void Game::load_hud(){

    load_hud_layout();
    load_hearth_callback();
    load_gold_callback();
}

void Game::load_hearth_callback(){
    m_components_data.emplace(ComponentType::CASTLE_HEALTH, m_castle_health);
    m_components_callbacks.emplace(ComponentType::CASTLE_HEALTH, [this](float value){
        auto it = m_components_data.find(ComponentType::CASTLE_HEALTH);
        if(it != m_components_data.end()){
            it->second -= value;
            SDL_Log("Health %.1f", it->second);
            
            // m_castle_health_text->SetText(std::to_string(static_cast<int>(it->second)));
            // m_slider->set_value(it->second);
            if(it->second <= 0.f){
                m_running = false;
                
                SDL_Log("GAME OVER");
            }
            return true;
        }
        return false;
    });
}

void Game::load_gold_callback(){
    m_components_data.emplace(ComponentType::PLAYER_GOLD, m_player_gold);
    m_components_callbacks.emplace(ComponentType::PLAYER_GOLD, [this](float value){
        /*Callback used for increase/decrease player gold. Adding tower decrease gold and 
        killing enemies increase gold*/

        auto it = m_components_data.find(ComponentType::PLAYER_GOLD);
        if(it != m_components_data.end() &&
          (it->second + value) > 0.f){
         
            it->second += value;
            SDL_Log("Gold %.1f", it->second);
            // m_player_gold_text->SetText(std::to_string(static_cast<int>(it->second)));
            return true;
        }
        return false;
    });
}

void Game::register_towers(){

    auto fire_tower = m_objects.create_tower_descr(TowerType::FIRE_TOWER_DATA);
    m_towers_scancode.emplace(SDL_SCANCODE_1, fire_tower);

    auto ice_tower = m_objects.create_tower_descr(TowerType::ICE_TOWER_DATA);
    m_towers_scancode.emplace(SDL_SCANCODE_2, ice_tower);

    auto poison_tower = m_objects.create_tower_descr(TowerType::POISON_TOWER_DATA);
    m_towers_scancode.emplace(SDL_SCANCODE_3, poison_tower);

    auto cloud_tower = m_objects.create_tower_descr(TowerType::CLOUD_TOWER_DATA);
    m_towers_scancode.emplace(SDL_SCANCODE_4, cloud_tower);
}

void Game::register_enemies(){
    auto enemy = m_objects.create_enemy_descr(EnemyType::VIKING);
    enemy      = m_objects.create_enemy_descr(EnemyType::BEE);
    enemy      = m_objects.create_enemy_descr(EnemyType::DRAGONIT);
    enemy      = m_objects.create_enemy_descr(EnemyType::SERJANT);
    enemy      = m_objects.create_enemy_descr(EnemyType::TANK);
}

void Game::handle_update(){
    switch(m_state){
        case GameState::Gameplay:
            update_game();
        break;
        case GameState::PauseMenu:

        break;
    }
}

void Game::load_hud_layout(){
    auto level_pos = m_cur_level.get_position();
    auto level_size = m_cur_level.get_size();
    SDL_FRect rect = {0, 0, 0, 0};
    //Load Player Stats Layout
    m_player_stats_layout = std::make_unique<UILayout>(this, SDL_GetRenderer(m_window), rect);

    auto health_layout = std::make_unique<UILayout>(this, SDL_GetRenderer(m_window), rect);
    //Add hearth image
    auto heart_image = std::make_unique<UIImage>("assets/heart.png", "I_heart");
    heart_image->SetDestSize({64, 64});
    //Add health label
    auto health_label = std::make_unique<UILabel>(std::to_string(static_cast<int>(m_castle_health)), "L_heart");
    health_label->SetFontSize(44);
    //Add health slider
    auto slider = std::make_unique<UISlider>("S_health");
    slider->set_range(0.f, m_max_castle_health);
    slider->set_value(m_castle_health);
    slider->SetSize({health_layout->GetRect().w,
                     health_layout->GetRect().y + 10,
                     100,
                     32});

    health_layout->PushBackWidgetHorizontal(std::move(heart_image));
    health_layout->PushBackWidgetHorizontal(std::move(health_label));
    health_layout->PushBackWidgetHorizontal(std::move(slider));
    rect.y = health_layout->GetRect().h;
    m_player_stats_layout->AddLayoutVertical(std::move(health_layout));
    
    auto gold_layout = std::make_unique<UILayout>(this, SDL_GetRenderer(m_window), rect);
    auto gold_image = std::make_unique<UIImage>("assets/coins.png", "I_gold");
    gold_image->SetDestSize({64, 64});
    auto gold_label = std::make_unique<UILabel>(std::to_string(static_cast<int>(m_player_gold)), "L_gold");
    gold_label->SetFontSize(44);
    gold_layout->PushBackWidgetHorizontal(std::move(gold_image));
    gold_layout->PushBackWidgetHorizontal(std::move(gold_label));
    m_player_stats_layout->AddLayoutVertical(std::move(gold_layout));

    //Load Entites Description Layout
    SDL_FRect descr_rect = {level_pos.x + level_size.x, 0};
    m_descriptions_layout = std::make_unique<UILayout>(this, SDL_GetRenderer(m_window), descr_rect);

}

void Game::update_description_layout(const SpriteComponent& sprite, const Entities::Descriptor& descr){
    auto sprites = m_render_system->get_registered_type_textures_pathes(sprite.type);
    auto render = SDL_GetRenderer(m_window);
    if(auto tower = std::get_if<TowerDescription>(&descr)){
        auto name_str = tower->get_type_string();

        std::string sprite_path = "";
        if(sprites.size() > 0)
            sprite_path = sprites[0];
        else{
            SDL_Log("WARNING: Sprites for type %d is empty", sprite.type);
        }
        auto layout_pos = m_descriptions_layout->GetPosition();
        // Tower Image
        auto descr_img = std::make_unique<UIImage>(sprite_path, "I_tower");
        descr_img->SetDestSize({128, 128});
        // Image frame
        auto frame_img = std::make_unique<UIImage>("assets/frame.png", "I_frame");
        
        m_descriptions_layout->PushBackWidgetHorizontal(std::move(descr_img));
        auto t = m_descriptions_layout->GetWidget("I_tower");
        if(!t){
            SDL_Log("ERROR: cant find 'I_tower' widget in layout");
            return;
        }
        SDL_FRect pos = t->GetSize();
        auto lower_desc = std::make_unique<UILayout>(this, render, pos);

        auto frame_pos = t->GetPosition();
        frame_img->SetPosition({frame_pos.x - 4, frame_pos.y - 4});
        frame_img->SetDestSize({128 + 8, 128 + 8});
        lower_desc->PushBackWidget(std::move(frame_img));
        
        auto name = std::make_unique<UILabel>(name_str, "I_name");
        lower_desc->PushBackWidgetVertical(std::move(name));
        // m_descriptions_layout->PushBackWidgetVertical(std::move(name));

        //Add damage param
        std::stringstream dmg_ss;
        dmg_ss << std::fixed << std::setprecision(1) << "Damage: " << tower->burst_damage;
        auto damage_text   = std::make_unique<UILabel>(dmg_ss.str(), "T_damage");
        lower_desc->PushBackWidgetVertical(std::move(damage_text));
        //Add remove cost param
        std::stringstream rem_cost_ss;
        rem_cost_ss << std::fixed << std::setprecision(1) << "Remove cost: " << tower->remove_cost;
        auto rem_cost_lbl   = std::make_unique<UILabel>(rem_cost_ss.str(), "T_remove_cost");
        lower_desc->PushBackWidgetVertical(std::move(rem_cost_lbl));
        //Add projectile speed param
        std::stringstream proj_speed_ss;
        proj_speed_ss << std::fixed << std::setprecision(1) << "Proj speed: " << tower->projectile_speed;
        auto proj_speed_lbl   = std::make_unique<UILabel>(proj_speed_ss.str(), "T_remove_cost");
        lower_desc->PushBackWidgetVertical(std::move(proj_speed_lbl));

        m_descriptions_layout->AddLayoutVertical(std::move(lower_desc));
        
        auto tower_circle = std::make_unique<UICircle>("C_radius");
        auto color = SdlWrapper::W_SDL_ConvertToFColor(Colors::Sunset::saffron);
        color.a = 0.5f;
        tower_circle->set_params(sprite.center.get_sdl_point(), tower->radius, color);
        m_descriptions_layout->PushBackWidget(std::move(tower_circle));
    }
    else if(auto enemy = std::get_if<EnemyDescription>(&descr)){
        auto name_str = enemy->get_type_string();

        std::string sprite_path = "";
        if(sprites.size() > 0)
            sprite_path = sprites[0];
        else{
            SDL_Log("WARNING: Sprites for type %d is empty", sprite.type);
        }
        auto layout_pos = m_descriptions_layout->GetRect();
        auto descr_img = std::make_unique<UIImage>(sprite_path, "I_enemy");
        descr_img->SetDestSize({128, 128});
        m_descriptions_layout->PushBackWidgetHorizontal(std::move(descr_img));
        auto t = m_descriptions_layout->GetWidget("I_enemy");
        if(!t){
            SDL_Log("ERROR: cant find 'I_enemy' widget in layout");
            return;
        }
        SDL_FRect pos = t->GetSize();
        auto lower_desc = std::make_unique<UILayout>(this, render, pos);

        auto frame_img = std::make_unique<UIImage>("assets/frame.png", "I_frame");
        auto frame_pos = t->GetPosition();
        frame_img->SetPosition({frame_pos.x - 4, frame_pos.y - 4});
        frame_img->SetDestSize({128 + 8, 128 + 8});
        lower_desc->PushBackWidget(std::move(frame_img));
        
        auto name = std::make_unique<UILabel>(name_str, "I_name");
        lower_desc->PushBackWidgetVertical(std::move(name));

        //Add damage description
        std::stringstream dmg_ss;
        dmg_ss << std::fixed << std::setprecision(1) << " Damage: " << enemy->damage;
        auto dmg_label = std::make_unique<UILabel>(dmg_ss.str(), "L_damage");
        lower_desc->PushBackWidgetVertical(std::move(dmg_label));
        //Add bounty description
        std::stringstream bnt_ss;
        bnt_ss << std::fixed << std::setprecision(1) << " Bounty: " << enemy->bounty;
        auto bnt_label = std::make_unique<UILabel>(bnt_ss.str(), "L_bounty");
        lower_desc->PushBackWidgetVertical(std::move(bnt_label));
        //Add health description
        std::stringstream hlth_ss;
        hlth_ss << std::fixed << std::setprecision(1) << " Max Health: " << enemy->health;
        auto hlth_label = std::make_unique<UILabel>(hlth_ss.str(), "L_max_health");
        lower_desc->PushBackWidgetVertical(std::move(hlth_label));

        m_descriptions_layout->AddLayoutVertical(std::move(lower_desc));
    }

}
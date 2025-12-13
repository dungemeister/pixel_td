
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
    ,m_spawn_system()
    ,m_current_ticks(0)
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
    register_type(EntityType::UNDEFINED,{"assets/quot_stickers.png"});
    register_type(EntityType::TILE,             {"assets/grass.png"});
    register_type(EntityType::ROAD,             {"assets/dirt_road.png"});
    register_type(EntityType::TOWER,            {"assets/tower.bmp"});
    register_type(EntityType::SPAWNER,          {"assets/dirt_road.png",
                                                 "assets/ground1.bmp"});
    register_type(EntityType::CASTLE,           {"assets/dirt_road.png"});
    
    register_type(EntityType::ICE_TOWER,        {"assets/ice_tower.png"});
    register_type(EntityType::FIRE_TOWER,       {"assets/fire/fire_tower1.png",
                                                 "assets/fire/fire_tower2.png",
                                                 "assets/fire/fire_tower3.png",
                                                 "assets/fire/fire_tower4.png",
                                                 "assets/fire/fire_tower5.png",
                                                 "assets/fire/fire_tower3.png"});
    register_type(EntityType::VIKING,            {"assets/enemies/viking.png"});
    register_type(EntityType::DRAGONIT,          {"assets/enemies/dragonit.png"});
    register_type(EntityType::BEE,               {"assets/enemies/bee.png"});
    register_type(EntityType::SERJANT,           {"assets/enemies/serjant.png"});
    register_type(EntityType::TANK,              {"assets/enemies/tank.png"});
    
    register_type(EntityType::PROJECTILE,       {"assets/fire/fireball.png"});

    register_type(EntityType::CASTLE_DECOR,     {"assets/statue.bmp"});
    register_type(EntityType::SPAWNER_DECOR,    {"assets/spawner.bmp"});
    register_type(EntityType::BUSH0_DECOR,      {"assets/bush0.bmp"});
    register_type(EntityType::BUSH1_DECOR,      {"assets/bush1.bmp"});
    register_type(EntityType::BUSH2_DECOR,      {"assets/bush2.bmp"});
    
    register_type(EntityType::HEARTH,           {"assets/hearth.png", "assets/broken_hearth.png"});

    load_level_tiles();
    load_decorations();
    load_towers();
    load_hearth();
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

        if(objects.add_tower(m_cur_level, EntityType::FIRE_TOWER, mouse_vec) > 0){
            std::cout << "Added tower to tile (" << mouse_pos.x << ", "
                                                    << mouse_pos.y <<
                                                ")" << std::endl;

        }
        else{
            std::cout << "Cannot add tower to " << mouse_event.x << " " << mouse_event.y <<
                         ". Tile already occupied or not in level map" << std::endl;
        }
    }
    else if(mouse_event.button == SDL_BUTTON_MIDDLE){
        if(m_cur_level.is_occupied(mouse_vec)) return;

        if(objects.add_tower(m_cur_level, EntityType::ICE_TOWER, mouse_vec) > 0){
            std::cout << "Added tower to tile (" << mouse_pos.x << ", "
                                                    << mouse_pos.y <<
                                                ")" << std::endl;

        }
        else{
            std::cout << "Cannot add tower to " << mouse_event.x << " " << mouse_event.y <<
                         ". Tile already occupied or not in level map" << std::endl;
        }
    }
    else if(mouse_event.button == SDL_BUTTON_RIGHT){
        if(m_cur_level.is_road_tile(mouse_pos)){
            auto id = objects.spawn_enemies_targeted(m_cur_level, m_target, mouse_pos, static_cast<EntityType>(static_cast<int>(EntityType::VIKING) + rand()%5));
            m_render_system->load_to_layer(objects.m_sprites[id]);

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

void Game::update_game(){
    float deltatime = std::min(static_cast<float>(SDL_GetTicks() - m_current_ticks) / 1000, 0.008f);
    m_current_ticks = SDL_GetTicks();
    // std::cout << deltatime <<"\n";
    m_spawn_system.update(m_objects, m_cur_level, deltatime);
    m_firing_system.update(m_objects, m_cur_level, *m_render_system, deltatime);
    m_move_system.update(m_objects, m_cur_level, deltatime);
    m_enemy_collision_system.update(m_objects, m_cur_level, deltatime);
    m_castle_damage_system.update(m_objects, m_cur_level, deltatime);
    m_animation_system.update(m_objects, deltatime);

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

void Game::add_target(Entities& objects, const Vector2D& pos){
    
    m_target = pos;
    objects.remove_object("target");

    auto id = objects.add_object("target");
    auto tile_size = m_cur_level.get_tile_size();

    objects.m_positions[id].angle = 0;
    objects.m_positions[id].x = pos.x;
    objects.m_positions[id].y = pos.y;
    // objects.m_flags[id] |= fEntityPosition;

    // objects.m_sprites[id].index = EntityType::TARGET;
    // objects.m_sprites[id].scale = 1;
    // objects.m_sprites[id].width = tile_size.x;
    // objects.m_sprites[id].height = tile_size.y;
    // objects.m_sprites[id].r = 0.6;
    // objects.m_sprites[id].g = 0.6;
    // objects.m_sprites[id].b = 0.6;
    // objects.m_flags[id] |= fEntitySprite;

    objects.m_borders[id].x_min = 0;
    objects.m_borders[id].x_max = 0;
    // objects.m_borders[id].y_min = (tile.pos.y - tile_size.y) / 2;
    objects.m_borders[id].y_min = (pos.y - tile_size.y / 4);
    objects.m_borders[id].y_max = (pos.y + tile_size.y / 4);
    // objects.m_flags[id] |= fEntityMapBorder;

    objects.m_moves[id].speed = 0;
    // objects.m_moves[id].rotation_angle = RandomFloat(-1.f, 1.f);
    // objects.m_flags[id] |= fEntityMove;

    objects.m_types[id] = EntityGlobalType::FRIEND_ENTITY;

    std::cout << "New Target" << std::endl;

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
                    m_objects.m_sprites[obj].type = EntityType::TILE;
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
                    m_objects.m_sprites[obj].type = EntityType::ROAD;
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
                    m_objects.m_sprites[obj].type = EntityType::SPAWNER;
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
                    m_objects.m_sprites[obj].type = EntityType::CASTLE;
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
        m_render_system->load_to_layer(m_objects.m_sprites[i]);
    }
}

void Game::load_decorations(){
    auto tile_size = m_cur_level.get_tile_size();
    auto castle_pos = m_cur_level.get_castle_pos();
    auto spawner_pos = m_cur_level.get_spawner_pos();
    auto resolution = m_cur_level.get_resolution();

//WARNING: sum of the decor elements should be less then grass tiles quantity otherwise deadlock
    load_decor_sprite(castle_pos, EntityType::CASTLE_DECOR);
    load_decor_sprite(spawner_pos, EntityType::SPAWNER_DECOR);
    load_decor_random_sprites(EntityType::BUSH0_DECOR, 33);
    load_decor_random_sprites(EntityType::BUSH1_DECOR, 44);
    load_decor_random_sprites(EntityType::BUSH2_DECOR, 40);

}

void Game::load_decor_sprite(const Vector2D& pos, EntityType type){
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
    m_render_system->load_to_layer(sprite);
    m_cur_level.set_tile_decor_occupied(tile.row, tile.column, 1);
}

void Game::load_decor_random_sprites(EntityType type, size_t size){
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
        m_render_system->load_to_layer(sprite);
        m_cur_level.set_tile_decor_occupied(row, column, 1);
    }
}

void Game::load_towers(){
    auto id = m_objects.add_tower(m_cur_level, EntityType::ICE_TOWER, {761.657, 382.826});
    id = m_objects.add_tower(m_cur_level, EntityType::FIRE_TOWER, {942.076, 378.737});
    id = m_objects.add_tower(m_cur_level, EntityType::FIRE_TOWER, {615.781, 267.516});
}

void Game::register_type(EntityType type, const std::vector<std::string>& textures){

    m_render_system->register_type_sprite(type, textures);
    m_animation_system.register_type(type, textures.size());
}

void Game::load_hearth(){
    auto id = m_objects.add_object("hearth");
    Vector2D pos = {100, 100};
    m_objects.m_positions[id].angle = 0;
    m_objects.m_positions[id].x = pos.x;
    m_objects.m_positions[id].y = pos.y;
    m_objects.m_systems[id] |= ePositionSystem;

    m_objects.m_sprites[id].posX = pos.x;
    m_objects.m_sprites[id].posY = pos.y;
    m_objects.m_sprites[id].width = 64;
    m_objects.m_sprites[id].height = 64;
    m_objects.m_sprites[id].scale = 1;
    m_objects.m_sprites[id].colR = 0.6;
    m_objects.m_sprites[id].colG = 0.6;
    m_objects.m_sprites[id].colB = 0.6;
    m_objects.m_sprites[id].angle = 0;
    m_objects.m_sprites[id].flag = fCenterSprite;
    m_objects.m_sprites[id].layer = SpriteLayer::BACKGROUND;
    m_objects.m_sprites[id].type = EntityType::HEARTH;
    m_objects.m_sprites[id].anim_index = -1;
    m_objects.m_systems[id] |= eSpriteSystem;

    m_render_system->load_to_layer(m_objects.m_sprites[id]);
}
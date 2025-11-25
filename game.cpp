#include <vector>
#include <string>

#include "render_system.h"
#include "figures.h"
#include "wrapper.h"
#include "math.h"
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <SDL3_image/SDL_image.h>
#include "level.h"
#include "test.h"

static float RandomFloat01() { return (float)rand() / (float)RAND_MAX; }
static float RandomFloat(float min, float max) { return RandomFloat01() * (max - min) + min; }

static int   RandomInt  (int min, int max) { return min + rand() % (max - min + 1); }

int g_running = 1;
int g_width = 1280;
int g_height = 720;
float fps;
SDL_FPoint g_cursor_pos;
SDL_Window* g_window = nullptr;
RenderSystem* g_render_system = nullptr;
SDL_Renderer* g_cursor_renderer = nullptr;

struct PositionComponent{
    float x,y;
    float angle;
};
struct SpriteComponent{
    float r, g, b;
    float scale;
    float width, height;
    int index;
};

struct MoveComponent{
    float velX, velY;
    float rotation_angle;
    void Initialize(float minSpeed, float maxSpeed)
    {
        // random angle
        float angle = RandomFloat01() * 3.1415926f * 2;
        // random movement speed between given min & max
        float speed = RandomFloat(minSpeed, maxSpeed);
        // velocity x & y components
        velX = cosf(angle) * speed;
        velY = sinf(angle) * speed;
    }
};

struct BorderComponent{
    float x_min, x_max;
    float y_min, y_max;
    
};

enum EntityFlag{
    fEntityPosition      = 1 << 0,
    fEntityMove          = 1 << 1,
    fEntitySprite        = 1 << 2,
    fEntityBorder        = 1 << 3,
    fEntitySpriteBatch   = 1 << 4,
};

typedef size_t EntityID;

struct Entities{
    std::vector<std::string>        m_names;
    std::vector<PositionComponent>  m_positions;
    std::vector<MoveComponent>      m_moves;
    std::vector<SpriteComponent>    m_sprites;
    std::vector<int>                m_flags;
    std::vector<BorderComponent>    m_borders;

    void reserve(size_t n){
        m_positions.reserve(n);
        m_names.reserve(n);
        m_moves.reserve(n);
        m_sprites.reserve(n);
        m_flags.reserve(n);
        m_borders.reserve(n);
    }
    size_t size() { return m_names.size(); }

    EntityID add_object(const std::string&& name){
        
        EntityID id = m_names.size();
        m_names.emplace_back(name);
        m_positions.push_back(PositionComponent());
        m_moves.push_back(MoveComponent());
        m_sprites.push_back(SpriteComponent());
        m_flags.push_back(0);
        m_borders.push_back(BorderComponent());

        return id;
    }
};
static Entities s_objects;

struct MoveSystem{
    void update(Entities& objects, float deltatime) {
        for(size_t i = 0; i < objects.size(); i++){
            
            if(objects.m_flags[i] & fEntityPosition && objects.m_flags[i] & fEntityMove){
                // std::cout << "Moving " << objects.m_names[i] << "\n";
                objects.m_positions[i].x += objects.m_moves[i].velX;
                objects.m_positions[i].y += objects.m_moves[i].velY;
                if(objects.m_positions[i].x  >= objects.m_borders[i].x_max || objects.m_positions[i].x <= objects.m_borders[i].x_min)
                    objects.m_moves[i].velX *= -1;
                if(objects.m_positions[i].y >= objects.m_borders[i].y_max || objects.m_positions[i].y <= objects.m_borders[i].y_min)
                    objects.m_moves[i].velY *= -1;
                
                objects.m_positions[i].angle += objects.m_moves[i].rotation_angle;
            }
        }

    }
};

static MoveSystem s_move_system;

void load_cursor(){
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
    SDL_Cursor* cursor = SDL_CreateColorCursor(scaled_surface, 0, 0);
    if(!cursor){
        SDL_Log("SDL_CreateColorCursor: %s", SDL_GetError());
        SDL_DestroySurface(orig_surface);
        SDL_DestroySurface(scaled_surface);
        return;
    }
    SDL_DestroySurface(orig_surface);
    SDL_DestroySurface(scaled_surface);
    SDL_SetCursor(cursor);
}

void init_render_system(){
    int res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    assert(res != 0);

    g_window = SDL_CreateWindow("Anime TD", g_width, g_height, SDL_WINDOW_OPENGL);

    g_render_system = new RenderSystem(g_window);

    load_cursor();
    
}

void init_game(){
    Level cur_level("assets/map/level1.map");
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
    auto res = cur_level.get_resolution();
    auto tokens = cur_level.get_tokens();
    auto rows = res.first;
    auto columns = res.second;

    int tile_width = std::min((g_width - 16*10) / res.second, 64UL);
    int tile_height = std::min((g_height - 9*10) / res.first, 64UL);
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < columns; c++){

            EntityID obj = s_objects.add_object("tile");
            if(tokens[columns * r + c] == "0"){
                s_objects.m_sprites[obj].index = 4;
                s_objects.m_sprites[obj].scale = 1;
                s_objects.m_sprites[obj].width = tile_width;
                s_objects.m_sprites[obj].height = tile_height;
                s_objects.m_sprites[obj].r = 0.6;
                s_objects.m_sprites[obj].g = 0.6;
                s_objects.m_sprites[obj].b = 0.6;
                if(obj % 2)
                    s_objects.m_flags[obj] |= fEntitySpriteBatch;
                else 
                    s_objects.m_flags[obj] |= fEntitySprite;
                
                s_objects.m_positions[obj].x = tile_width * c;
                s_objects.m_positions[obj].y = tile_height * r;
                s_objects.m_flags[obj] |= fEntityPosition;

                s_objects.m_moves[obj].velX = RandomFloat(.1, 3.);
                s_objects.m_moves[obj].velY = RandomFloat(.1, 3.);
                s_objects.m_moves[obj].rotation_angle = RandomFloat(-1.f, 1.f);
                s_objects.m_flags[obj] |= fEntityMove;

                s_objects.m_borders[obj].x_min = tile_width * c - 10.;
                s_objects.m_borders[obj].x_max = tile_width * c + 10.;
                s_objects.m_borders[obj].y_min = tile_height * r - 10.;
                s_objects.m_borders[obj].y_max = tile_height * r + 10.;
                s_objects.m_flags[obj] |= fEntityBorder;
            }

            // std::cout << "Pos " << s_objects.m_positions[obj].x << " " << s_objects.m_positions[obj].y << "\n";

        }

    }

}

void destroy_game(){
    delete g_render_system;
}

void game_loop(){
    Uint64 lastTime = SDL_GetTicks();
    int frames = 0;
    Uint64 interval = 100; // 1 секунда в мс
    while(g_running){
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

void handle_input(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_EVENT_QUIT:
                g_running = 0;
            break;
            case SDL_EVENT_MOUSE_MOTION:
                g_cursor_pos.x = event.motion.x;
                g_cursor_pos.y = event.motion.y;
            break;
        }
    }
}

void update_game(float deltatime){
    // std::cout << deltatime <<"\n";
    s_move_system.update(s_objects, deltatime);
    g_render_system->clean_batch_frame();
    g_render_system->clean_frame();
    for(size_t i = 0, n = s_objects.size(); i != n; i++){
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

        if(s_objects.m_flags[i] & fEntitySpriteBatch){
            g_render_system->add_sprite_to_batch(spr_data);
        }
        else if(s_objects.m_flags[i] & fEntitySprite)
            g_render_system->add_to_frame(std::move(spr_data));
    }
}

void draw_output(){
    auto r = SDL_GetRenderer(g_window);
    SdlWrapper::W_SDL_SetRenderDrawColor(r, {0x18, 0x18, 0x18, 0xFF});
    SDL_RenderClear(r);

    g_render_system->render_batch();
    g_render_system->render();
    // TextureTest::test_texture_rendering(r, "assets/rocket_tower.png");
    SDL_Color cursor_circle_color = {0xF7, 0x62, 0x18, 0x80};
    bool res = Circle::render_circle_filled(r, g_cursor_pos.x, g_cursor_pos.y, 100, cursor_circle_color);
    if(!res){
        SDL_Log("render circle: %s", SDL_GetError());
    }
    char fps_str[256];
    sprintf(fps_str, "FPS: %.2f", fps);
    SDL_RenderDebugText(r, 0, 20, fps_str);
    SDL_RenderPresent(SDL_GetRenderer(g_window));
}
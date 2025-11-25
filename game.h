#pragma once

typedef struct
{
    float posX, posY;
    float width, height;
    float scale;
    float colR, colG, colB;
    float angle;
    int sprite;
} sprite_data_t;

void init_render_system();
void init_game();
void destroy_game();
void game_loop();
void handle_input();
void update_game(float deltatime);
void draw_output();
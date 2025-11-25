#include <iostream>
#include <unistd.h>
#include "game.h"

int main(int argc, char* argv[]){
    srand(time(NULL));
    std::cout << "Hello from anime_td. PID: " << getpid() << "\n";
    
    init_render_system();
    init_game();

    game_loop();
    destroy_game();

    return 0;
}
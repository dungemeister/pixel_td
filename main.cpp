#include <iostream>
#include <unistd.h>
#include "game.h"

int main(int argc, char* argv[]){
    srand(time(NULL));
    std::cout << "Hello from anime_td. PID: " << getpid() << "\n";
    
    Game game;
    game.loop();

    return 0;
}
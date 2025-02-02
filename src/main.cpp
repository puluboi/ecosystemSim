#include "game.hpp"
int main(){
    Game game;
    while(game.isRunning()) {
        game.Update();
    }
}

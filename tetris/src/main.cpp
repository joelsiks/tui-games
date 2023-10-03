
#include <iostream>

#include "Tetris.hpp"
#include "Tetromino.hpp"

int main() {
    Tetris::Game* game = new Tetris::Game();

    // Start the game loop.
    game->loop();

    return 0;
}

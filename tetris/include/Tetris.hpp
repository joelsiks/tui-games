#pragma once

#include <curses.h>

#include "Playfield.hpp"
#include "Tetromino.hpp"
#include "Util.hpp"

// Initial tetromino position
#define INITIAL_ROW PLAYFIELD_ROWS - 1
#define INITIAL_COL 3

// How many pieces to store in the upcoming pieces queue.
#define UPCOMING_QUEUE_SIZE 3

#define DEFAULT_GRAVITY 0.05

// The following gravities are defined as multiples of DEFAULT_GRAVITY.
#define SOFT_GRAVITY 10
#define HARD_GRAVITY 500

// How many gravity downwards iterations a piece can stay at the bottom before
// being frozen into the playfield.
#define BOTTOM_ROTATES 3

namespace Tetris {

// This cannot be enum class because the values cannot be implicitly converted
// to int (for some weird reason?).
enum Key {
    MoveRight = 'd',
    MoveLeft = 'a',
    MoveSoft = 's',
    MoveHard = ' ',
    RotateRight = 'w',
    RotateLeft = 'c',
    Pause = 'p',
    Quit = 'q',
};

enum class GameState {
    // Default state is where the current piece is moving down at the gravity
    // rate and nothing more.
    Default,

    MovingRight,
    MovingLeft,

    // Temporarily increases the gravity to move the piece down faster.
    MovingSoft,

    // Temporarily increases the gravity to 20 * gravity for one frame so that
    // the piece lands immediately.
    MovingHard,

    RotatingRight,
    RotatingLeft,

    // Dead is when no more pieces can be spawned into the playfield.
    Dead,

    Pause,
};

class Game {
   private:
    size_t m_frame_counter;

    // Used to count gravity. When it reaches above a certain threshold
    // (probably 1, the gravity counter is reset and the piece moves down).
    double m_gravity_counter;

    bool m_running;
    GameState m_state;

    int m_rows_cleared;

    // Window height and width.
    int m_height;
    int m_width;

    // Upcoming pieces to be spawned.
    std::vector<Tetromino::Piece *> m_upcoming;

    Playfield *m_playfield = nullptr;
    Tetromino::Piece *m_current_tetromino = nullptr;

    // Counts how many times the current piece is able to move at the bottom
    // before being frozen into the playfield.
    int m_bottom_delay_counter;

    // The speed that the current tetromino moves downward.
    double m_gravity;

    // Generates a new tetromino and adds it to m_upcoming.
    void new_upcoming_tetromino();

    // Updates m_current_tetromino to the first piece in m_upcoming and
    // adds a new piece to m_upcoming.
    void spawn_tetromino();

    // Renders a shadow where the current tetromino would end up if it was hard
    // dropped.
    void render_tetromino_shadow();

    void handle_input();
    void handle_state();

    // Checks whether the piece is currently in an invalid position by checking
    // if it collides with the playfield or is out of bounds, either vertically
    // or horizontally. Returns true if all clear, false if its in an invalid
    // position.
    bool invalid_piece_position();

    // Tries to move the current piece sideways. The piece only gets moved
    // if the move is valid (i.e it does not hit anything or goes out of
    // bounds).
    void sideways_move_piece(Direction dir);

    // Tries to rotate the current piece. The piece only gets rotated if the
    // rotation is valid (probably is though since the SRS rules are used).
    void rotate_piece(Direction dir);

    // Downards movement of the current piece.
    void drop_piece();

   public:
    Game();

    // This is the game loop.
    void loop();
};

}  // namespace Tetris

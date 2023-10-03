
#include "Tetris.hpp"

#include <time.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>

static void clear_screen() {
    erase();
    refresh();
}

namespace Tetris {

void Game::new_upcoming_tetromino() {
    m_upcoming.push_back(
        Tetromino::Piece::random_tetromino(PLAYFIELD_ROWS - 1, 3));
}

void Game::spawn_tetromino() {
    // Freeze the piece into the playfield and start playing with a new piece.
    if (m_current_tetromino != nullptr) {
        m_playfield->insert_piece(m_current_tetromino);
    }

    // Read and remove first element in the queue.
    m_current_tetromino = m_upcoming.front();
    m_upcoming.erase(m_upcoming.begin());

    // Add a new tetromino to the upcoming queue.
    this->new_upcoming_tetromino();
}

void Game::render_tetromino_shadow() {
    // Store the previous position
    std::pair<int, int> prev_position = m_current_tetromino->get_position();

    while (!invalid_piece_position())
        m_current_tetromino->move(Direction::Down);

    m_current_tetromino->move(Direction::Up);

    m_playfield->render_piece_color(m_current_tetromino, TET_ORANGE);

    // Revert the position.
    m_current_tetromino->set_position(prev_position);
}

void Game::handle_input() {
    for (int i = 0; i < 1000; i++) {
        char input = tolower(mvgetch(0, 0));

        // If the game is paused the only input we care about is whether the
        // player wants to unpause the game, hitting the pause key again,
        // or if the player wants to quit.
        if (m_state == GameState::Pause) {
            if (input == Key::Pause) {
                m_state = GameState::Default;
            } else if (input == Key::Quit) {
                m_running = false;
            }

            return;
        }

        switch (input) {
            case Key::MoveRight:
                m_state = GameState::MovingRight;
                break;
            case Key::MoveLeft:
                m_state = GameState::MovingLeft;
                break;
            case Key::MoveSoft:
                m_state = GameState::MovingSoft;
                break;
            case Key::MoveHard:
                m_state = GameState::MovingHard;
                break;
            case Key::RotateRight:
                m_state = GameState::RotatingRight;
                break;
            case Key::RotateLeft:
                m_state = GameState::RotatingLeft;
                break;
            case Key::Pause:
                m_state = GameState::Pause;
                break;
            case Key::Quit:
                m_running = false;
                return;
        }
    }
}

void Game::handle_state() {
    switch (m_state) {
        case GameState::Default:
            break;
        case GameState::MovingRight:
            this->sideways_move_piece(Direction::Right);
            break;
        case GameState::MovingLeft:
            this->sideways_move_piece(Direction::Left);
            break;
        case GameState::MovingSoft:
            m_gravity = DEFAULT_GRAVITY * SOFT_GRAVITY;
            break;
        case GameState::MovingHard:
            m_gravity = DEFAULT_GRAVITY * HARD_GRAVITY;
            break;
        case GameState::RotatingRight:
            this->rotate_piece(Direction::Right);
            break;
        case GameState::RotatingLeft:
            this->rotate_piece(Direction::Left);
            break;
        case GameState::Dead:
            break;
        case GameState::Pause:
            break;
    }

    // Change back to default state very time.
    m_state = GameState::Default;

    this->drop_piece();
}

bool Game::invalid_piece_position() {
    return m_current_tetromino->is_out_of_bounds_col(PLAYFIELD_COLUMNS - 1) ||
           m_current_tetromino->is_out_of_bounds_row() ||
           m_playfield->piece_collision(m_current_tetromino);
}

void Game::sideways_move_piece(Direction dir) {
    // Only accounts for sideways move.
    if (dir == Direction::Up || dir == Direction::Down) {
        return;
    }

    // Start with moving the piece sideways.
    m_current_tetromino->move(dir);

    // Check if move moves the piece out of bounds or if it collides with the
    // playfield.
    if (invalid_piece_position()) {
        // If the move isn't possible, revert the sideways move.
        m_current_tetromino->move(Util::reverse_direction(dir));
    }
}

void Game::rotate_piece(Direction dir) {
    // Start with rotating the piece.
    m_current_tetromino->rotate(dir);

    // Check if the rotation moves the piece out of bounds or if it collides
    // with the playfield.
    if (invalid_piece_position()) {
        // Simple wall kick algorithm. Try moving the piece one space to the
        // right and then one space to the left, and fail if neither can be
        // done.
        // TODO: This algorithm should be replaced by the more complex wall
        //       kick system used in SRS.
        m_current_tetromino->move(Direction::Right);

        if (invalid_piece_position()) {
            m_current_tetromino->move(Direction::Left);
            m_current_tetromino->move(Direction::Left);

            if (invalid_piece_position()) {
                // If the rotation isn't possible, revert the rotation and move
                // back to the original position.
                m_current_tetromino->rotate(Util::reverse_direction(dir));
                m_current_tetromino->move(Direction::Right);
            }
        }

    } else {
        // If the rotation was possible, reset the bottom delay counter.
        m_bottom_delay_counter = 0;
    }
}

void Game::drop_piece() {
    // If the gravity counter is less than one, increment it by the current
    // gravity.
    if (m_gravity_counter < 1) {
        m_gravity_counter += m_gravity;
        return;
    }

    int cells_to_move_down = (int)m_gravity_counter;

    // Move the piece down a number of cells.
    for (int i = 0; i < cells_to_move_down; i++) {
        // If the piece has hit the bottom and its counter has started counting,
        // count down.
        if (m_bottom_delay_counter > 1) {
            m_bottom_delay_counter -= 1;
            continue;
        }

        // Start with moving the piece down.
        m_current_tetromino->move(Direction::Down);

        if (invalid_piece_position()) {
            // Move the piece back up
            m_current_tetromino->move(Direction::Up);

            // If the piece hasn't moved it means it is being blocked and no new
            // pieces can be spawned.
            if (m_current_tetromino->get_position().first == INITIAL_ROW) {
                m_state = GameState::Dead;
                return;
            }

            // If the piece could not be moved down and the bottom delay counter
            // is 1, then spawn a new tetromino and reset the counter. If the
            // counter is 0, that means it is the first time a piece has hit
            // something, and its counter should start counting.
            if (m_bottom_delay_counter == 1) {
                this->spawn_tetromino();
                m_bottom_delay_counter = 0;
                break;
            } else if (m_bottom_delay_counter == 0) {
                m_bottom_delay_counter = BOTTOM_ROTATES;
            }
        }
    }

    // Decrement the gravity counter and reset the gravity.
    m_gravity_counter -= cells_to_move_down;
    m_gravity = DEFAULT_GRAVITY;
}

Game::Game()
    : m_frame_counter(0),
      m_gravity_counter(0),
      m_running(true),
      m_state(GameState::Default),
      m_rows_cleared(0),
      m_gravity(DEFAULT_GRAVITY) {
    int mY, mX;

    initscr();
    getmaxyx(stdscr, mY, mX);

    m_height = mY;
    m_width = mX;

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    // Needed to use color.
    start_color();

    // Intiallize color pairs.
    init_pair(TET_LIGHT_BLUE, COLOR_BLACK, COLOR_CYAN);
    init_pair(TET_YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(TET_PURPLE, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(TET_GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(TET_RED, COLOR_BLACK, COLOR_RED);
    init_pair(TET_BLUE, COLOR_BLACK, COLOR_BLUE);
    init_pair(TET_ORANGE, COLOR_BLACK, COLOR_WHITE);
    init_pair(TET_BLACK, COLOR_BLACK, COLOR_BLACK);

    // Don't echo out keypresses.
    noecho();

    // Make sure all keypresses are sent directly to the program.
    raw();
    keypad(stdscr, true);

    // Do not block when waiting for input.
    nodelay(stdscr, TRUE);

    // Hide the cursor.
    curs_set(0);

    m_playfield = new Playfield(mY, mX);
    m_bottom_delay_counter = 0;

    // Initialize random seed.
    srand(time(NULL));

    // Initialize the upcoming queue with tetrominos.
    for (int i = 0; i < UPCOMING_QUEUE_SIZE; i++) {
        this->new_upcoming_tetromino();
    }

    // Get the first tetromino into m_current_tetromino.
    this->spawn_tetromino();
}

void Game::loop() {
    // Stop the game loop if the running state has been set to false.
    if (!m_running) {
        clear_screen();
        curs_set(1);
        return;
    }

    this->handle_input();

    // If the game is paused there is no need to draw anything on the screen
    // except the paused message.
    if (m_state == GameState::Pause) {
        m_playfield->render_paused_message(Key::Pause);

        this->loop();
        return;
    }

    this->handle_state();
    m_playfield->render_UI(m_upcoming);
    m_playfield->render_piece(m_current_tetromino);
    this->render_tetromino_shadow();

    m_rows_cleared += m_playfield->delete_full_rows();

    // TODO: Move this to a separate information method.
    mvprintw(11, m_width / 4, "Rows cleared: %d", m_rows_cleared);

    usleep((1 / 100.0) * 1000 * 1000);

    m_frame_counter++;
    this->loop();
}

}  // namespace Tetris

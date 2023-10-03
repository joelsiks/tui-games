#pragma once

#define PLAYFIELD_ROWS 22
#define PLAYFIELD_COLUMNS 10

#define HIDDEN_ROWS 2

#define ROW_SCL 2
#define COL_SCL 4

#include <curses.h>

#include <utility>
#include <vector>

#include "Tetromino.hpp"

namespace Tetris {

constexpr static std::pair<int, int> DEFAULT_ROW = {0, TET_BLACK};

class Playfield {
   private:
    // Height and width of the terminal window.
    int m_height;
    int m_width;

    int m_start_x;
    int m_start_y;

    // 2D-array to represent the playfield. First int is used to indicate if
    // the cell is occupied or not, and the second int is used to store the
    // color of the cell (one of TET colors defined in "Tetris.hpp").
    std::vector<std::vector<std::pair<int, int>>> m_playfield;

    // Renders a box around the playfield.
    void render_box();

    // Renders the content of m_playfield.
    void render_playfield();

    // Render the upcoming pieces to the right of the playfield.
    void render_upcoming_pieces(const std::vector<Tetromino::Piece *> &pieces);

    // Renders a cell of size ROW_SCL * COL_SCL with origin at (x, y) with
    // color.
    void render_cell_color_coordinate(int y, int x, int color);

   public:
    Playfield(int height, int width);

    // Deletes any rows that are full and returns the number of deleted rows.
    int delete_full_rows();

    // Checks if a piece at its current position is colliding with anything.
    // Returns true if it collides with anything, false if not.
    bool piece_collision(Tetromino::Piece *piece);

    // Inserts (freezes) a piece into the playfield.
    void insert_piece(Tetromino::Piece *piece);

    void render_cell(int row, int col);
    void render_cell_color(int row, int col, int color);

    void render_piece(Tetromino::Piece *piece);
    void render_piece_color(Tetromino::Piece *piece, int color);

    // Renders the UI, which includes the box, playfield and upcoming pieces.
    void render_UI(const std::vector<Tetromino::Piece *> &pieces);

    // Renders a paused message in the middle of the screen when the game is
    // paused.
    void render_paused_message(char pause_key);
};

}  // namespace Tetris

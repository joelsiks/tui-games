#pragma once

#define TET_LIGHT_BLUE 1
#define TET_YELLOW 2
#define TET_PURPLE 3
#define TET_GREEN 4
#define TET_RED 5
#define TET_BLUE 6
#define TET_ORANGE 7
#define TET_BLACK 8

#include <vector>

#include "Util.hpp"

namespace Tetromino {

enum Type { I = 1, J, L, O, S, T, Z };

class Piece {
   private:
    Type m_type;
    int m_color;

    // Contains the position on the playfield, order: (row, col).
    std::pair<int, int> m_position;

    // 2D-vector containing one of the 2D-vectors from TET_MATRICES.
    std::vector<std::vector<int>> m_representation;

   public:
    Type get_type();
    int get_color();
    const std::pair<int, int> &get_position();
    void set_position(const std::pair<int, int> &pos);
    const std::vector<std::vector<int>> &get_matrix();

    // Returns height and width of the representation matrix from get_matrix().
    int height();
    int width();

    Piece(int type, int start_row, int start_col);
    Piece(Type type, int start_row, int start_col);

    // Used to get a random tetromino piece.
    static Piece *random_tetromino(int start_row, int start_col);

    // Check if the piece is out of bounds given min & max values.
    // Only checks if out of bounds on bottom since pieces only move downwards.
    // Return true if out of bounds, false if not.
    bool is_out_of_bounds_row();

    bool is_out_of_bounds_col(int max);

    // Moves the tetromino by chaning its position according to a direction.
    // Does not account for moving the tetromino out of bounds, use
    // is_out_of_bounds_<col, row>() to check for this.
    void move(Direction dir);

    // Rotates the matrix representation to the right (1) or to the left (-1).
    void rotate(Direction dir);
};

}  // namespace Tetromino

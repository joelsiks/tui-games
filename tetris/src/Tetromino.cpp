
#include "Tetromino.hpp"

#include <stdlib.h>
#include <time.h>

#include <algorithm>

namespace Tetromino {

static const int NUMBER_OF_PIECES = 7;

static const std::vector<std::vector<std::vector<int>>> TET_MATRICES{
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0}, {1, 1, 1}, {0, 0, 0}},
    {{0, 0, 1}, {1, 1, 1}, {0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}},
    {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}},
    {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}},
};

// Shorthand getters for private members.
Type Piece::get_type() { return m_type; }

int Piece::get_color() { return m_color; }

const std::pair<int, int> &Piece::get_position() { return m_position; }

void Piece::set_position(const std::pair<int, int> &pos) { m_position = pos; }

const std::vector<std::vector<int>> &Piece::get_matrix() {
    return m_representation;
}

int Piece::height() { return m_representation.size(); }

int Piece::width() { return m_representation[0].size(); }

Piece::Piece(int type, int start_row, int start_col)
    : Piece((Type)type, start_row, start_col) {}

Piece::Piece(Type type, int start_row, int start_col)
    : m_type(type), m_color((int)type) {
    m_position = {start_row, start_col};

    m_representation = TET_MATRICES[type - 1];
}

Piece *Piece::random_tetromino(int start_row, int start_col) {
    int piece_num = rand() % Tetromino::NUMBER_OF_PIECES + 1;
    Piece *new_piece = new Tetromino::Piece(piece_num, start_row, start_col);

    return new_piece;
}

bool Piece::is_out_of_bounds_row() {
    int row = m_position.first;

    for (int i = 0; i < this->height(); i++) {
        for (int j = 0; j < this->width(); j++) {
            if (m_representation[i][j] == 1 && (row - i < 0)) {
                return true;
            }
        }
    }

    return false;
}

bool Piece::is_out_of_bounds_col(int max) {
    int col = m_position.second;

    // The minimul value is implicitly set to 0.
    for (int i = 0; i < this->height(); i++) {
        for (int j = 0; j < this->width(); j++) {
            if (m_representation[i][j] == 1 && (j + col < 0 || j + col > max)) {
                return true;
            }
        }
    }

    return false;
}

void Piece::move(Direction dir) {
    switch (dir) {
        case Direction::Left:
            m_position.second -= 1;
            break;
        case Direction::Right:
            m_position.second += 1;
            break;
        case Direction::Up:
            m_position.first += 1;
            break;
        case Direction::Down:
            m_position.first -= 1;
            break;
    }
}

void Piece::rotate(Direction dir) {
    // Return if tryng to rotate "unrotateable" pieces.
    if (m_type == Type::O) {
        return;
    }

    if (dir == Direction::Right || dir == Direction::Up) {
        Util::transpose_matrix(m_representation);
        Util::reverse_rows_matrix(m_representation);
    } else if (dir == Direction::Left || dir == Direction::Down) {
        Util::reverse_rows_matrix(m_representation);
        Util::transpose_matrix(m_representation);
    }
}

}  // namespace Tetromino

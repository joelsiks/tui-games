
#include "Playfield.hpp"

#include <string>

#include "Util.hpp"

namespace Tetris {

void Playfield::render_box() {
    // Top left corner of the box. The box surrounds the entire
    // playing field of PLAYFIELD_ROWS x PLAYFIELD_COLUMNS scaled by
    // ROW_SCL and COL_SCL.
    int start_x = m_start_x - 1;
    int start_y = m_start_y - 1;

    // As we don't render the top two rows we dont draw the box around them.
    int height = PLAYFIELD_ROWS - 2;
    int width = PLAYFIELD_COLUMNS;

    // Box corners
    mvprintw(start_y, start_x, "+");
    mvprintw(start_y, start_x + width * COL_SCL + 1, "+");

    mvprintw(start_y + height * ROW_SCL + 1, start_x, "+");
    mvprintw(start_y + height * ROW_SCL + 1, start_x + width * COL_SCL + 1,
             "+");

    // Box horizontal lines
    for (int i = 0; i < width * COL_SCL; i++) {
        mvprintw(start_y, start_x + 1 + i, "-");
        mvprintw(start_y + height * ROW_SCL + 1, start_x + 1 + i, "-");
    }

    // Box vertical lines
    for (int i = 0; i < height * ROW_SCL; i++) {
        mvprintw(start_y + 1 + i, start_x, "|");
        mvprintw(start_y + 1 + i, start_x + width * COL_SCL + 1, "|");
    }

    refresh();
}

void Playfield::render_playfield() {
    // Clear playfield.
    for (int i = 0; i < PLAYFIELD_ROWS - 2; i++) {
        for (int j = 0; j < PLAYFIELD_COLUMNS; j++) {
            this->render_cell_color(i, j, TET_BLACK);
        }
    }

    for (int i = 0; i < PLAYFIELD_ROWS - 2; i++) {
        for (int j = 0; j < PLAYFIELD_COLUMNS; j++) {
            if (m_playfield[i][j].first == 1) {
                this->render_cell(i, j);
            }
        }
    }
}

Playfield::Playfield(int height, int width) : m_height(height), m_width(width) {
    // Initialize the playfield 2d-array.
    m_playfield.resize(PLAYFIELD_ROWS, std::vector<std::pair<int, int>>(
                                           PLAYFIELD_COLUMNS, DEFAULT_ROW));

    m_start_x = (m_width - PLAYFIELD_COLUMNS * COL_SCL) / 2;
    m_start_y = (m_height - PLAYFIELD_ROWS * ROW_SCL) / 2;
}

bool Playfield::piece_collision(Tetromino::Piece *piece) {
    std::pair<int, int> piece_pos = piece->get_position();
    std::vector<std::vector<int>> representation = piece->get_matrix();

    for (int i = 0; i < piece->height(); i++) {
        for (int j = 0; j < piece->width(); j++) {
            if (representation[i][j] == 1 &&
                m_playfield[piece_pos.first - i][piece_pos.second + j].first ==
                    1) {
                return true;
            }
        }
    }

    return false;
}

int Playfield::delete_full_rows() {
    int full_rows = 0;

    for (int i = 0; i < PLAYFIELD_ROWS; i++) {
        bool full = true;

        for (int j = 0; j < PLAYFIELD_COLUMNS; j++) {
            // If a single cell is 0, the row cannot be full.
            if (m_playfield[i][j].first == 0) {
                full = false;
            }
        }

        if (full) {
            // Increment the counter.
            full_rows += 1;

            // Remove the row from the playfield and insert a blank row at the
            // top.
            m_playfield.erase(m_playfield.begin() + i);
            m_playfield.push_back(std::vector<std::pair<int, int>>(
                PLAYFIELD_COLUMNS, DEFAULT_ROW));
        }
    }

    return full_rows;
}

void Playfield::insert_piece(Tetromino::Piece *piece) {
    std::vector<std::vector<int>> rep = piece->get_matrix();
    std::pair<int, int> pos = piece->get_position();
    int color = piece->get_color();

    for (size_t i = 0; i < rep.size(); i++) {
        for (size_t j = 0; j < rep[0].size(); j++) {
            if (rep[i][j] == 1) {
                m_playfield[pos.first - i][pos.second + j] = {1, color};
            }
        }
    }
}

void Playfield::render_cell(int row, int col) {
    this->render_cell_color(row, col, m_playfield[row][col].second);
}

void Playfield::render_cell_color(int row, int col, int color) {
    // If the position is outside of the playfield, do nothing.
    if (row > PLAYFIELD_ROWS - 3 || row < 0 || col > PLAYFIELD_COLUMNS - 1 ||
        col < 0) {
        return;
    }

    // We want to render the with respect to the bottom left corner as origin.
    int start_y =
        m_start_y + (PLAYFIELD_ROWS - 2) * ROW_SCL - row * ROW_SCL - 1;
    int start_x = m_start_x + col * COL_SCL;

    this->render_cell_color_coordinate(start_y, start_x, color);
}

void Playfield::render_piece(Tetromino::Piece *piece) {
    render_piece_color(piece, piece->get_color());
}

void Playfield::render_piece_color(Tetromino::Piece *piece, int color) {
    std::vector<std::vector<int>> rep = piece->get_matrix();
    std::pair<int, int> pos = piece->get_position();

    for (int i = 0; i < piece->height(); i++) {
        for (int j = 0; j < piece->width(); j++) {
            if (rep[i][j] == 1) {
                render_cell_color(pos.first - i, pos.second + j, color);
            }
        }
    }
}

void Playfield::render_cell_color_coordinate(int y, int x, int color) {
    for (int i = 0; i < ROW_SCL; i++) {
        for (int j = 0; j < COL_SCL; j++) {
            attron(COLOR_PAIR(color));
            mvprintw(y - i, x + j, " ");
            attroff(COLOR_PAIR(color));
        }
    }
}

void Playfield::render_upcoming_pieces(
    const std::vector<Tetromino::Piece *> &pieces) {
    int left_offset = 10;
    int x_offset = m_start_x + PLAYFIELD_COLUMNS * COL_SCL + left_offset;
    int y_offset = m_start_y - 1;
    int top_offset = 3;

    int row_offset = 5;

    // Clear any previous drawn pieces.
    for (size_t i = 0; i < row_offset * pieces.size() * ROW_SCL; i++) {
        for (int j = 0; j < 4 * COL_SCL; j++) {
            this->render_cell_color_coordinate(y_offset + top_offset + i,
                                               x_offset + j, TET_BLACK);
        }
    }

    // Print header.
    mvprintw(y_offset, x_offset, "Upcoming pieces:");

    // Draw all upcoming pieces.
    for (size_t i = 0; i < pieces.size(); i++) {
        Tetromino::Piece *current = pieces[i];

        for (int j = 0; j < current->height(); j++) {
            for (int k = 0; k < current->width(); k++) {
                if (current->get_matrix()[j][k] == 1) {
                    this->render_cell_color_coordinate(
                        y_offset + top_offset + i * ROW_SCL * row_offset +
                            j * ROW_SCL,
                        x_offset + k * COL_SCL, current->get_color());
                }
            }
        }
    }
}

void Playfield::render_UI(const std::vector<Tetromino::Piece *> &pieces) {
    this->render_upcoming_pieces(pieces);
    this->render_box();
    this->render_playfield();
}

void Playfield::render_paused_message(char pause_key) {
    std::string message = "Paused. Press '' to resume";
    message.insert(15, 1, pause_key);

    // Print text message.
    mvprintw(m_height / 2, m_width / 2 - message.length() / 2, message.c_str());
}

}  // namespace Tetris

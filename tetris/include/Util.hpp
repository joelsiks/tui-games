#pragma once

#include <vector>

// Used to indicate direction when performing moves and rotates for example.
enum class Direction { Left = -1, Right = 1, Up = -2, Down = 2 };

// Definition(s) of utility functions.
namespace Util {

// Gets the reverse direction of a given direction. Example:
// reverse_direction(Direction::Left) -> Direction::Right
Direction reverse_direction(Direction direction);

// Transposes a square matrix of size NxN.
void transpose_matrix(std::vector<std::vector<int>> &matrix);

// Reverses the rows of a matrix.
void reverse_rows_matrix(std::vector<std::vector<int>> &matrix);

}  // namespace Util

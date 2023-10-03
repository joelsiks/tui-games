
#include "Util.hpp"

Direction Util::reverse_direction(Direction direction) {
    // Since the values of left & right and up & down differs only
    // by sign we can convert between them by casting to an int.
    return (Direction)(-(int)direction);
}

void Util::transpose_matrix(std::vector<std::vector<int>> &matrix) {
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = i; j < matrix[0].size(); j++) {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
}

void Util::reverse_rows_matrix(std::vector<std::vector<int>> &matrix) {
    size_t width = matrix.size();

    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < matrix[0].size() / 2; j++) {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[i][width - 1 - j];
            matrix[i][width - 1 - j] = temp;
        }
    }
}
#include <iostream>
#include <tuple>
#include "array2d.h"

// parse command line arguments
// return: size of board, number of iterations and the density of the board
// default: size = 50, nr_iteration = 10, density = 0.3
auto parse_args(int argc, char* argv[]) {
    int size {50};
    int nr_iteration {10};
    float density {0.3f};
    if (argc > 1) {
        size = std::stoi(argv[1]);
    }
    if (argc > 2) {
        nr_iteration = std::stoi(argv[2]);
    }
    if (argc > 3) {
        density = std::stof(argv[3]);
    }
    return std::make_tuple(size, nr_iteration, density);
}

void initialize_board(Array2D<int>& board, const float density) {
    for (int i = 0; i < board.height(); ++i) {
        for (int j = 0; j < board.width(); ++j) {
            board(i, j) = (rand() % 100) < (density * 100) ? 1 : 0;
        }
    }
}

void print_board(const Array2D<int>& board) {
    for (int i = 0; i < board.height(); ++i) {
        for (int j = 0; j < board.width(); ++j) {
            std::cout << (board(i, j) == 1 ? "*" : " ") << " ";
        }
        std::cout << std::endl;
    }
}

int get_number_of_neigbours(const Array2D<int>& board, const int i, const int j) {
    int nr_neighbours {0};
    for (int k = -1; k <= 1; ++k) {
        for (int l = -1; l <= 1; ++l) {
            if (k == 0 && l == 0) {
                continue;
            }
            int x {i + k};
            int y {j + l};
            if (x < 0 || x >= board.height() || y < 0 || y >= board.width()) {
                continue;
            }
            if (board(x, y) == 1) {
                ++nr_neighbours;
            }
        }
    }
    return nr_neighbours;
}

float get_board_density(const Array2D<int>& board) {
    int nr_alive {0};
    for (int i = 0; i < board.height(); ++i) {
        for (int j = 0; j < board.width(); ++j) {
            if (board(i, j) == 1) {
                ++nr_alive;
            }
        }
    }
    return static_cast<float>(nr_alive)/(board.height()*board.width());
}

void iterate_board(Array2D<int>& board) {
    Array2D<int> new_board(board.height(), board.width());
    for (int i = 0; i < board.height(); ++i) {
        for (int j = 0; j < board.width(); ++j) {
            int nr_neighbours = get_number_of_neigbours(board, i, j);
            if (board(i, j) == 1) {
                new_board(i, j) = (nr_neighbours == 2 || nr_neighbours == 3) ? 1 : 0;
            } else {
                new_board(i, j) = (nr_neighbours == 3) ? 1 : 0;
            }
        }
    }
    board = new_board;
}

int main(int argc, char* argv[]) {
    auto [size, nr_iteration, density] = parse_args(argc, argv);
    Array2D<int> board(size, size);
    int iteration {0};
    initialize_board(board, density);
    std::cout << iteration << " " << get_board_density(board) << std::endl;
    for (iteration = 1; iteration < nr_iteration; ++iteration) {
        iterate_board(board);
        std::cout << iteration << " " << get_board_density(board) << std::endl;
    }
    return 0;
}

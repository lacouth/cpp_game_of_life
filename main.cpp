#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

typedef std::vector<uint> Row;
typedef std::vector<Row> Board;
const std::string ALIVE = " o ";
const std::string DEAD = " _ ";

void generates_board_initial_state(Board &board, size_t number_of_cells) {

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> random_number(
      0, board.size() - 1);

  for (int i = 0; i < number_of_cells; ++i) {
    board[random_number(rng)][random_number(rng)] = 1;
  }
}

void print_board(const Board &board) {

  std::for_each(board.begin(), board.end(), [](const Row &row) {
    std::for_each(row.begin(), row.end(),
                  [](const uint &cell) { std::cout << (cell ? ALIVE : DEAD); });
    std::cout << std::endl;
  });
}

int main() {

  size_t board_width = 10;
  size_t number_initial_live_cells = 20;

  Board board(board_width, std::vector<uint>(board_width, 0));

  generates_board_initial_state(board, number_initial_live_cells);
  print_board(board);
}
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

enum class Cell { dead, alive };

typedef std::vector<Cell> Row;
typedef std::vector<Row> Board;

const std::string ALIVE_SYMBOL = " o ";
const std::string DEAD_SYMBOL = " _ ";
const int MIN_NEIGHBOURS = 2;
const int MAX_NEIGHBOURS = 3;

const std::vector<std::vector<int>> neighbours{{-1, 0}, {0, -1}, {1, 0}, {0, 1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};

Board board_factory(size_t n_rows, size_t n_cols, Cell initial_value = Cell::dead);
void generates_board_initial_state(Board &board, size_t number_of_cells);
void print_board(const Board &board);
void update_board(Board &board);
bool is_everybody_dead(const Board &board);

int main(int argc, char **argv) {

  size_t board_size = 50;
  uint number_initial_living_cells = 20;
  size_t generations = 0;
  size_t max_generations = 100;

  while (true) {
    switch (getopt(argc, argv, "s:n:m:")) {
    case 's':
      board_size = std::stoi(optarg);
      continue;
    case 'n':
      number_initial_living_cells = std::stoi(optarg);
      continue;
    case 'm':
      max_generations = std::stoi(optarg);
      continue;
    default:
      break;
    }
    break;
  }

  Board board = board_factory(board_size, board_size);

  generates_board_initial_state(board, number_initial_living_cells);
  while (!is_everybody_dead(board) && generations < max_generations) {
    print_board(board);
    update_board(board);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    generations++;
  }
  print_board(board);
  if (generations < max_generations)
    std::cout << "GAME OVER - No Cells Alive\n";
  else
    std::cout << generations << " generations\n";
}

/*

    Utility Functions

*/

Board board_factory(size_t n_rows, size_t n_cols, Cell initial_value) {
  return Board(n_rows, Row(n_cols, initial_value));
}

void generates_board_initial_state(Board &board, size_t number_of_cells) {

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> random_number(
      0, board.size() - 1);

  for (size_t i = 0; i < number_of_cells; ++i) {
    board[random_number(rng)][random_number(rng)] = Cell::alive;
  }
}

void print_board(const Board &board) {

  std::system("clear");
  std::for_each(board.begin(), board.end(), [](const Row &row) {
    std::for_each(row.begin(), row.end(), [&](Cell cell) {
      std::cout << (cell == Cell::alive ? ALIVE_SYMBOL : DEAD_SYMBOL);
    });
    std::cout << std::endl;
  });
}

std::pair<int, int> check_neighbor_position(const std::vector<int> &coord,
                                            const std::vector<size_t> positions,
                                            size_t board_size) {

  int new_coords[2];

  for (int i = 0; i < 2; ++i) {
    new_coords[i] = positions[i] + coord[i];
    if (new_coords[i] < 0 || new_coords[i] >= board_size) {
      new_coords[i] = new_coords[i] < 0 ? board_size - 1 : 0;
    }
  }

  return std::make_pair(new_coords[0], new_coords[1]);
}

void update_board(Board &board) {
  Board temporary_board = board_factory(board.size(), board.size());
  uint count_neighbors;
  std::pair<int, int> pos;

  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < board.size(); ++j) {
      count_neighbors = 0;
      std::for_each(neighbours.begin(), neighbours.end(),
                    [&](std::vector<int> neighbor) {
                      pos = check_neighbor_position(
                          neighbor, std::vector<size_t>{i, j}, board.size());
                      if (board[pos.first][pos.second] == Cell::alive)
                        count_neighbors++;
                    });

      if (count_neighbors >= MIN_NEIGHBOURS &&
          count_neighbors <= MAX_NEIGHBOURS) {
        temporary_board[i][j] = Cell::alive;
      } else {
        temporary_board[i][j] = Cell::dead;
      }
    }
  }
  board = std::move(temporary_board);
}

bool is_everybody_dead(const Board &board) {
  bool flag = true;
  std::for_each(board.begin(), board.end(), [&](const Row &row) {
    std::for_each(row.begin(), row.end(), [&](const Cell cell) {
      if (cell == Cell::alive)
        flag = false;
    });
  });
  return flag;
}
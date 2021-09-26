/*
 * This file is part of a C++ Generative Art Project.
 *
 *
 * Developed by Patric Lacouth;
 *
 * This file implements Conway's Game of Life using C++  leveraging the STL's
 * power. The game can only be used in the terminal, which imposes a restricted
 * world size depending on the user environment.
 *
 * The universe of the Game of Life is an infinite, two-dimensional orthogonal
 * grid of square cells, each of which is in one of two possible states, live or
 * dead, (or populated and unpopulated, respectively). Every cell interacts with
 * its eight neighbours, which are the cells that are horizontally, vertically,
 * or diagonally adjacent. At each step in time, the following transitions
 * occur:
 *
 *   Any live cell with fewer than two live neighbours dies, as if by
 * underpopulation. Any live cell with two or three live neighbours lives on to
 * the next generation. Any live cell with more than three live neighbours dies,
 * as if by overpopulation. Any dead cell with exactly three live neighbours
 * becomes a live cell, as if by reproduction.
 *
 * @see https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 *
 * Basic build instructions
 *
 * compile: g++ main.cpp -std=c++17 -o main
 * run it: ./main
 *
 * Usage
 *
 * Once build the binary can be used standalone or with some specific flags:
 *  -s sets the size of the board.
 *      ./main -s 50 //for a board with 50x50 cells
 *  -n sets the initial number of living cells
 *      ./main -n 20 //to start with 20 living cells
 *  -m sets the maximum number of generations
 *      ./main -m 100 //to run the game for maximum of 100 generations
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details. To know more about GNU GPL,
 * see <https://www.gnu.org/licenses/>.
 *
 * You can see further developments accessing:
 * @see github.com/lacouth/cpp_game_of_life
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

/**
 * Possible cell's states in the game.
 * These values are used to impose the only two states a cell can assume during
 * the game.
 */
enum class Cell { dead, alive };

// Type definition of the Row being a std::vector of Cells
typedef std::vector<Cell> Row;

// Type definition of the Board being a std::vector of Rows
typedef std::vector<Row> Board;

const std::string ALIVE_SYMBOL = " o "; ///< Symbol used in terminal to represents a live cell
const std::string DEAD_SYMBOL = " _ "; ///<  Symbol used in terminal to represents a dead cell
const int MIN_NEIGHBOURS = 2; ///< Game constant to define underpopulation
const int MAX_NEIGHBOURS = 3; ///< Game constant to define overpopuplation

/**
 * Creates a square board to the game of life
 *
 * @param size size_t whose value is used to create a square board.
 * @param initial_value Cell indicates the initial state of all cells in the
 * board, the default value is set to Cell::dead
 * @return a new Board with the configurations established by the parameters
 * above.
 */

Board board_factory(size_t size, Cell initial_value = Cell::dead);

/**
 * Populates the board with living cells
 * @param board Board passsed by reference to be populated.
 * @param number_of_cells size_t defines the number of random generated
 * positions should be set as Cell::alive.
 */

void generates_board_initial_state(Board &board, size_t number_of_cells);

/**
 * Prints the Board on the standard output
 *
 * Uses the ALIVE_SYMBOL and the DEAD_SYMBOL to represents the cells state.
 *
 * @param board Board passed as const reference to be printed
 */

void print_board(const Board &board);

/**
 * Updates a board using the three rules of Conway's Game of Life.
 *
 * @param board Board passed by referece to be updated.
 */
void update_board(Board &board);

/**
 * Gets a valid position of the neighbour of a cell
 *
 * @param coord const Container with the Board indexes of a cell.
 * @param positions const Container with the directions of the neighbour to be
 * calculated.
 * @param board_size size_t with the size of the Board.
 * @return std::pair<int,int> with the indexes of the neighbour in the given
 * direction.
 */
std::pair<int, int> neighbour_position(const std::vector<int> &coord,
                                       const std::vector<size_t> positions,
                                       size_t board_size);

/**
 * Runs through a Board looking for a living cell
 *
 * @param board Board passed as a const reference
 * @return bool return true if all cells are marked as dead, false if there is
 * at least one cell alive.
 * @note if the cell is on the edge of the board the neighbours will be set as
 * the cells in the on the other side of the board, creating the illusion of a
 * round board.
 */

bool is_everybody_dead(const Board &board);

int main(int argc, char **argv) {

  size_t board_size = 50;
  uint number_initial_living_cells = 200;
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

  Board board = board_factory(board_size);

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
    Implementations
*/

Board board_factory(size_t size, Cell initial_value) {
  return Board(size, Row(size, initial_value));
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

std::pair<int, int> neighbour_position(const std::vector<int> &coord,
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
  const std::vector<std::vector<int>> neighbourhood{
      {-1, 0}, {0, -1}, {1, 0}, {0, 1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};
  Board temp_board = board_factory(board.size());
  uint count_neighbors;
  std::pair<int, int> pos;

  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < board.size(); ++j) {
      count_neighbors = 0;
      std::for_each(neighbourhood.begin(), neighbourhood.end(),
                    [&](std::vector<int> neighbor) {
                      pos = neighbour_position(
                          neighbor, std::vector<size_t>{i, j}, board.size());
                      if (board[pos.first][pos.second] == Cell::alive)
                        count_neighbors++;
                    });

      if (board[i][j] == Cell::alive && count_neighbors < MIN_NEIGHBOURS) {
        temp_board[i][j] = Cell::dead;
      } else if (board[i][j] == Cell::alive &&
                 count_neighbors >= MIN_NEIGHBOURS and
                 count_neighbors <= MAX_NEIGHBOURS) {
        temp_board[i][j] = Cell::alive;
      } else if (board[i][j] == Cell::alive &&
                 count_neighbors > MAX_NEIGHBOURS) {
        temp_board[i][j] = Cell::dead;
      } else if (board[i][j] == Cell::dead &&
                 count_neighbors == MAX_NEIGHBOURS) {
        temp_board[i][j] = Cell::alive;
      }
    }
  }
  board = std::move(temp_board);
}

bool is_everybody_dead(const Board &board) {
  for (const Row &row : board) {
    for (const Cell &cell : row) {
      if (cell == Cell::alive)
        return false;
    }
  }
  return true;
}
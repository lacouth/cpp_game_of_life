#include <iostream>
#include <vector>
#include <string>
#include <random>

typedef std::vector<std::vector<uint>> Board;

void generates_board_initial_state(Board &board, size_t number_of_cells){

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> random_number(0,board.size() - 1);
    
    for(int i=0;i<number_of_cells;++i){
        board[random_number(rng)][random_number(rng)] = 1;
    }

}

int main(){

    size_t board_width = 25;
    size_t number_initial_live_cells = 10;
    
    Board board(board_width, std::vector<uint>(board_width,0));

    generates_board_initial_state(board,number_initial_live_cells);
    
}
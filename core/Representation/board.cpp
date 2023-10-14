#include "board.h"



void Board::clearBoard()
{
    for (int i = 0; i < 64; i++)
    {
        board[i] = 0;
    }
}

Board::Board()
{
    clearBoard();
}

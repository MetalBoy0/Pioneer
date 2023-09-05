//
// Created by WVGarrison on 8/18/2023.
//

#ifndef CHESSENGINE_MAIN_H
#define CHESSENGINE_MAIN_H


#include <iostream>
#include <list>
#include "legal.h"
#include "moves.h"
using namespace std;

int toBoardIndex(int x, int y);




// The main board class
class Board {
public:
    int squares[64]{};
    int kingSquares[2] = {4, 60};
    bool isWhiteMove;
    int enPassantSquare;
    int pastMoveIndex = 0;
    Board();

    void printBoard();
    // Simple function to convert a from square and to square to a move on the board
    Move getMove(int from, int to) {
        Move move = *new Move(from, to, squares[from], squares[to]);
        return move;
    }
    void makeMove(Move move);
    void undoMove();
    void updatePieceLists();
    void updateBoard();
    list<Move> generateLegalMoves();
private:
    Move pastMoves[100]; // Stores the past moves
};





#endif //CHESSENGINE_MAIN_H

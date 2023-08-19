//
// Created by WVGarrison on 8/18/2023.
//

#ifndef CHESSENGINE_MAIN_H
#define CHESSENGINE_MAIN_H


#include <iostream>

#include "legal.h"
#include "moves.h"
using namespace std;

int toBoardIndex(int x, int y);




// The main board class
class Board {
public:
    int squares[64]{};
    bool isWhiteMove;

    Board();

    void printBoard() {
        cout << "test" << endl;
        for (int i = 0; i < 64; i++) {
            int square = squares[i];
            string pieces[] = {" . ", " p ", " n ", " b ", " r ", " q ", " k ", " P ", " N ", " B ", " R ", " Q ",
                               " K "};
            int pieceIndex = 0;
            pieceIndex += Piece::getPieceType(square);
            if (Piece::getSide(square) == Piece::White) {
                pieceIndex = pieceIndex + 6;
            }
            string pieceChar = pieces[pieceIndex];
            cout << pieceChar;
            // Newline if next row of chess board
            if ((i + 1) % 8 == 0) {
                cout << endl;
            }
        }
    }

    Move getMove(int from, int to) {
        Move move = *new Move(from, to, squares[from], squares[to]);
        return move;
    }
    void makeMove(Move move);

};





#endif //CHESSENGINE_MAIN_H

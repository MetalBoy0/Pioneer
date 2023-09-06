//
// Created by Will on 8/18/2023.
//

#ifndef CHESSENGINE_MOVES_H
#define CHESSENGINE_MOVES_H

#include "pieces.h"

// A move class that stores the from square, to square, and the piece that is moving
class Move {

public:
    int from;
    int to;
    int movePiece;
    int toPiece;
    bool isWhiteToMove;
    bool isCapture;
    bool promotion;

    Move* copy();

    Move(int fromSquare, int toSquare, int movePiece, int toPiece);

    Move(); // overloaded constructor for Board::pastMove array
};


#endif //CHESSENGINE_MOVES_H

//
// Created by Will on 8/18/2023.
//

#ifndef CHESSENGINE_MOVES_H
#define CHESSENGINE_MOVES_H
#include "pieces.h"

class Move {
public:
    int from{};
    int to{};
    int movePiece{};
    int toPiece{};
    bool isWhiteToMove{};
    bool isCapture{};

    Move(int fromSquare, int toSquare, int movePiece, int toPiece);
};




#endif //CHESSENGINE_MOVES_H

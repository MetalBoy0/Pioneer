//
// Created by WVGarrison on 8/18/2023.
//

#ifndef CHESSENGINE_LEGAL_H
#define CHESSENGINE_LEGAL_H

#include "main.h"
#include "moves.h"

class Rules {
public:
    static bool pawnLegal(Move move);

    static bool isLegal(Move move);
};


#endif //CHESSENGINE_LEGAL_H
